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

class chat_data_packet
{
public:
    chat_data_packet(std::shared_ptr<chat_message> message);
    
    static std::shared_ptr<chat_data_packet> Create(const chat_message * msg);
    
    static std::shared_ptr<chat_data_packet> Create(boost::asio::streambuf & buf);
    
    boost::asio::const_buffers_1 Buffer() const { return boost::asio::buffer(buf_.data(), buf_.size()); }
    
    std::shared_ptr<chat_message> Message() const { return message_; }
    
    static const size_t HeaderSize;
    static const size_t ChecksumSize;

private:
    chat_data_packet();
    
    void write_data(boost::asio::streambuf & buf);
    
    std::shared_ptr<chat_message> message_;
    boost::asio::streambuf buf_;
};

#endif /* chat_data_packet_h */
