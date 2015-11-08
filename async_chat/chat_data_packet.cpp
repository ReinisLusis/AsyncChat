//
//  chat_data_packet.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_data_packet.h"

#include <boost/iostreams/copy.hpp>
#include <cryptopp/md5.h>

const size_t chat_data_packet::HeaderSize = 4;
const size_t chat_data_packet::ChecksumSize = CryptoPP::MD5::DIGESTSIZE;

chat_data_packet::chat_data_packet()
{
    
}

std::shared_ptr<chat_data_packet> chat_data_packet::Create(const chat_message & msg)
{
    // TODO: use filtering stream to avoid copy of data
    boost::asio::streambuf stream_buf;
    std::ostream os(&stream_buf);
    msg.Serialize(os);
    
    auto retVal = std::shared_ptr<chat_data_packet>(new chat_data_packet());
    retVal->write_data(stream_buf);
    return retVal;
}

std::shared_ptr<chat_data_packet> chat_data_packet::Create(std::istream & stream)
{
    auto body_size = static_cast<size_t>(stream.get()) << 24 | static_cast<size_t>(stream.get()) << 16 | static_cast<size_t>(stream.get()) << 8 | static_cast<size_t>(stream.get()) << 0;
    byte body_checksum [ CryptoPP::MD5::DIGESTSIZE ];
    
    // TODO: do not copy buffers
    boost::asio::streambuf body_buf;
    boost::iostreams::copy(stream, body_buf, body_size);
    
    boost::asio::streambuf hash_buf;
    boost::iostreams::copy(stream, hash_buf, CryptoPP::MD5::DIGESTSIZE);
    
    CryptoPP::MD5 hash;
    hash.CalculateDigest(body_checksum, boost::asio::buffer_cast<const byte*>(body_buf.data()), body_size);
    
    if (memcmp(body_checksum, boost::asio::buffer_cast<const byte*>(hash_buf.data()), CryptoPP::MD5::DIGESTSIZE) != 0)
    {
        throw std::exception();
    }
    
    auto retVal = std::shared_ptr<chat_data_packet>(new chat_data_packet());
    std::istream is(&body_buf);
    retVal->message_ = chat_message::Deserialize(is);
    
    if (retVal->message_ == nullptr)
    {
        throw std::exception();
    }
    
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
