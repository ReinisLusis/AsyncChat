//
//  chat_data_packet.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_data_packet_h
#define chat_data_packet_h

#include "chat_message.h"

#include <boost/asio.hpp>

namespace async_chat
{
    class ChatDataPacket
    {
    public:
        ChatDataPacket(std::shared_ptr<ChatMessage> message);
        
        static std::shared_ptr<ChatDataPacket> Create(const ChatMessage & msg);
        
        static std::shared_ptr<ChatDataPacket> Create(boost::asio::streambuf & buf);
        
        boost::asio::const_buffers_1 Buffer() const { return boost::asio::buffer(buf_.data(), buf_.size()); }
        
        std::shared_ptr<ChatMessage> Message() const { return message_; }
        
        static const size_t kHeaderSize;
        static const size_t kChecksumSize;
        static const size_t kMaxSizeLimit;

    private:
        ChatDataPacket();
        
        std::shared_ptr<ChatMessage> message_;
        boost::asio::streambuf buf_;
    };
}
#endif /* chat_data_packet_h */
