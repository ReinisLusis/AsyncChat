//
//  chat_data_packet.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_data_packet.h"

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <cryptopp/md5.h>

const size_t chat_data_packet::HeaderSize = 4;
const size_t chat_data_packet::ChecksumSize = CryptoPP::MD5::DIGESTSIZE;

chat_data_packet::chat_data_packet()
{
    
}

#include <boost/iostreams/categories.hpp> // input_filter_tag

struct body_size_filter {
    size_t body_size;
    size_t bytes_read;
    
    typedef char char_type;
    typedef boost::iostreams::multichar_input_filter_tag category;
    
    template<typename Source>
    std::streamsize read(Source& src, char* s, std::streamsize n)
    {
        if (bytes_read >= body_size + 4)
        {
            return -1;
        }
        
        int c;
        
        // read body size
        while (bytes_read < 4 && (c = boost::iostreams::get(src)) != EOF && c != boost::iostreams::WOULD_BLOCK)
        {
            body_size = body_size | (static_cast<size_t>(c) << (24 - 8 * bytes_read));
            bytes_read++;
        }
        
        if (bytes_read < 4)
        {
            return c;
        }
        
        char* first = s;
        char* last  = s + n;
        while (bytes_read < body_size + 4 && first != last && (c = boost::iostreams::get(src)) != EOF && c != boost::iostreams::WOULD_BLOCK )
        {
            *first++ = c;
            bytes_read++;
        }
        
        std::streamsize result = static_cast<std::streamsize>(first - s);
        return result == 0 && c != boost::iostreams::WOULD_BLOCK ? -1 : result;
    }
    
    body_size_filter()
    {
        body_size = 0;
        bytes_read = 0;
    }
    

};

std::shared_ptr<chat_data_packet> chat_data_packet::Create(const chat_message * msg)
{
    // TODO: use filtering stream to avoid copy of data
    boost::asio::streambuf stream_buf;
    std::ostream os(&stream_buf);
    msg->Serialize(os);
    
    auto retVal = std::shared_ptr<chat_data_packet>(new chat_data_packet());
    retVal->write_data(stream_buf);
    return retVal;
}

std::shared_ptr<chat_data_packet> chat_data_packet::Create(boost::asio::streambuf & buf)
{
    /*boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(body_size_filter());
    in.push(buf);*/
    
    std::istream in(&buf);
    
    size_t body_size = (static_cast<size_t>(in.get()) << 24) | (static_cast<size_t>(in.get()) << 16) | (static_cast<size_t>(in.get()) << 8) | (static_cast<size_t>(in.get()) << 0);
    
    auto data_ptr = boost::asio::buffer_cast<const byte*>(buf.data());
   
    CryptoPP::MD5 hash;
    byte checksum [ CryptoPP::MD5::DIGESTSIZE ];
    hash.CalculateDigest(checksum, data_ptr, body_size);
    
    if (memcmp(checksum, data_ptr + body_size, CryptoPP::MD5::DIGESTSIZE) != 0)
    {
        throw std::exception();
    }
   
    auto retVal = std::shared_ptr<chat_data_packet>(new chat_data_packet());
    retVal->message_ = chat_message::Deserialize(in);
    
    if (retVal->message_ == nullptr)
    {
        throw std::exception();
    }
    
    buf.consume(CryptoPP::MD5::DIGESTSIZE);
    
    return retVal;
}

void chat_data_packet::write_data(boost::asio::streambuf & buf)
{
    std::ostream os(&buf_);
    
    os << static_cast<unsigned char>((buf.size() >> 24) & 0xFF);
    os << static_cast<unsigned char>((buf.size() >> 16) & 0xFF);
    os << static_cast<unsigned char>((buf.size() >> 8) & 0xFF);
    os << static_cast<unsigned char>((buf.size() >> 0) & 0xFF);
    
    CryptoPP::MD5 hash;
    byte checksum [ CryptoPP::MD5::DIGESTSIZE ];
    
    hash.CalculateDigest(checksum, boost::asio::buffer_cast<const byte*>(buf.data()), buf.size());
    
    std::istream is(&buf);
    boost::iostreams::copy(is, os);
    
    os.write(reinterpret_cast<char*>(checksum), CryptoPP::MD5::DIGESTSIZE);
}
