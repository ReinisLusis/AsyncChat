//
//  chat_data_packet.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_data_packet.h"

#include "chat_message_client_notice.h"
#include "chat_message_text.h"
#include "chat_message_text2.h"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/iostreams/copy.hpp>
#include <cryptopp/md5.h>

BOOST_SERIALIZATION_SHARED_PTR(async_chat::ChatMessage)
BOOST_CLASS_EXPORT_GUID(async_chat::ChatMessage, "chat_message")
BOOST_CLASS_EXPORT_GUID(async_chat::ChatMessageText, "chat_message_text")
BOOST_CLASS_EXPORT_GUID(async_chat::ChatMessageText2, "chat_message_text2")
BOOST_CLASS_EXPORT_GUID(async_chat::ChatMessageClientNotice, "chat_message_client_notice")

namespace async_chat {

const size_t ChatDataPacket::kHeaderSize = 4;
const size_t ChatDataPacket::kChecksumSize = CryptoPP::MD5::DIGESTSIZE;
const size_t ChatDataPacket::kMaxSizeLimit = 65535;

ChatDataPacket::ChatDataPacket(std::shared_ptr<ChatMessage> message) : message_(message)
{
}

ChatDataPacket::ChatDataPacket()
{
}

std::shared_ptr<ChatDataPacket> ChatDataPacket::Create(const ChatMessage & msg)
{
    boost::asio::streambuf msg_buf;
    std::ostream msg_stream(&msg_buf);
    boost::archive::xml_oarchive msg_archive(msg_stream);
    msg_archive.template register_type<ChatMessage>();
    msg_archive.template register_type<ChatMessageClientNotice>();
    msg_archive.template register_type<ChatMessageText>();
    msg_archive.template register_type<ChatMessageText2>();
    const ChatMessage * msg_p = &msg;
    msg_archive << boost::serialization::make_nvp("item", msg_p);
    
    CryptoPP::MD5 hash;
    byte checksum [ CryptoPP::MD5::DIGESTSIZE ];
    hash.CalculateDigest(checksum, boost::asio::buffer_cast<const byte*>(msg_buf.data()), msg_buf.size());
    
    auto retVal = std::shared_ptr<ChatDataPacket>(new ChatDataPacket());
    std::ostream os(&retVal->buf_);
    
    os << static_cast<unsigned char>((msg_buf.size() >> 24) & 0xFF);
    os << static_cast<unsigned char>((msg_buf.size() >> 16) & 0xFF);
    os << static_cast<unsigned char>((msg_buf.size() >> 8) & 0xFF);
    os << static_cast<unsigned char>((msg_buf.size() >> 0) & 0xFF);

    boost::iostreams::copy(msg_buf, retVal->buf_);
    
    os.write(reinterpret_cast<char*>(checksum), CryptoPP::MD5::DIGESTSIZE);
    
    return retVal;
}

std::shared_ptr<ChatDataPacket> ChatDataPacket::Create(boost::asio::streambuf & buf)
{
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

    ChatMessage *message;
    boost::archive::xml_iarchive i(in);
    i.template register_type<ChatMessage>();
    i.template register_type<ChatMessageClientNotice>();
    i.template register_type<ChatMessageText>();
    i.template register_type<ChatMessageText2>();
    i >> boost::serialization::make_nvp("item", message);
    
    if (message == nullptr) {
        throw std::exception();
    }
    
    buf.consume(CryptoPP::MD5::DIGESTSIZE);
    
    return std::make_shared<ChatDataPacket>(std::shared_ptr<ChatMessage>(message));
}
    
}
