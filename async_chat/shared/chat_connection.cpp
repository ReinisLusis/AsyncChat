//
//  chat_connection.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "app.h"

#include "chat_connection.h"

#include <cstdio>
#include <functional>
#include <boost/asio/write.hpp>

namespace async_chat {
using boost::asio::ip::tcp;

void ChatConnection::Write(std::shared_ptr<ChatDataPacket> packet)
{
    if (write_queue_.empty())
    {
        write_queue_.push(packet);
        WriteInternal();
    }
    else
    {
        write_queue_.push(packet);
    }
}

void ChatConnection::Disconnect()
{
    if (socket_.is_open())
    {
        socket_.close();
    }
}

ChatConnection::ChatConnection(tcp::socket socket) :
    socket_ (std::move(socket)),
    read_state_(ReadState::Header){
}

void ChatConnection::Read()
{
    auto readHandler = std::bind(&ChatConnection::OnRead, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
    auto completionHandler = std::bind(&ChatConnection::OnCompletion, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
    boost::asio::async_read(socket_, read_buffer_, completionHandler, readHandler);
}

void ChatConnection::OnError()
{
    APP->controller()->ClientError(shared_from_this());
}

void ChatConnection::WriteInternal()
{
    auto writeHandler = std::bind(&ChatConnection::OnWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
    boost::asio::async_write(socket_, write_queue_.front()->Buffer(), writeHandler);
    //socket_.async_send(write_queue_.front()->Buffer(), writeHandler);
}

void ChatConnection::OnRead(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (error) {
        APP->Info(boost::format("ChatConnection::OnRead() error: %1%") % boost::system::system_error(error).what());
        
        if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
        {
            OnConnectionClosed();
        }
        else
        {
            OnError();
        }
        
        return;
    }
    
    switch (read_state_)
    {
        case ReadState::Header:
        {
            auto data = boost::asio::buffer_cast<const unsigned char*>(read_buffer_.data());
            body_size_ = static_cast<size_t>(data[0]) << 24 | static_cast<size_t>(data[1]) << 16 | static_cast<size_t>(data[2]) << 8 | static_cast<size_t>(data[3]) << 0;
            
            if (body_size_ > ChatDataPacket::kMaxSizeLimit)
            {
                APP->Info("ChatConnection::OnRead() maximum message size exceeded");
                OnError();
                return;
            }
            
            read_state_ = ReadState::Body;
            break;
        }
        case ReadState::Body:
        {
            read_state_ = ReadState::Checksum;
            break;
        }
        case ReadState::Checksum:
        {
            try {
                auto packet = ChatDataPacket::Create(read_buffer_);
                if (!ProcessMessage(packet->Message()))
                {
                    APP->Info("ChatConnection::OnRead() process_message failed");
                    OnError();
                    return;
                }
            } catch (...) {
                APP->Info("ChatConnection::OnRead() error in chat_data_packet::Create");
                OnError();
                return;
            }
            
            read_state_ = ReadState::Header;
            break;
        }
    }
    
    if (!APP->controller()->SusspendRead(shared_from_this()))
    {
        this->Read();
    } else
    {
        APP->controller()->NotifySusspended(shared_from_this());
    }
}

void ChatConnection::OnWrite(const boost::system::error_code& error, const size_t bytes_transferred)
{
    APP->Info(boost::format("ChatConnection::OnWrite() bytes_transferred: %1%") % bytes_transferred);
    if (error) {
        APP->Info(boost::format("ChatConnection::OnWrite() error: %1%") % boost::system::system_error(error).what());
        
        if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
        {
            OnConnectionClosed();
        }
        else
        {
            OnError();
        }
        
        return;
    }
    
    write_queue_.pop();
    
    auto this_ptr = shared_from_this();
    
    APP->controller()->WriteCompleted(this_ptr);
    
    if (!write_queue_.empty())
    {
        WriteInternal();
    }
}

std::size_t ChatConnection::OnCompletion(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (error) {
        APP->Info(boost::format("ChatConnection::OnCompletion() error: %1%") % boost::system::system_error(error).what());
        
        if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
        {
            OnConnectionClosed();
        }
        else
        {
            OnError();
        }
        
        return 0;
    }
    
    auto total_bytes = read_buffer_.size() + bytes_transferred;
    size_t expected_size = 0;
    
    switch (read_state_)
    {
        case ReadState::Header:
            expected_size = ChatDataPacket::kHeaderSize;
            break;
            
        case ReadState::Body:
            expected_size = ChatDataPacket::kHeaderSize + body_size_;
            break;
        case ReadState::Checksum:
            expected_size = ChatDataPacket::kHeaderSize + body_size_ + ChatDataPacket::kChecksumSize;
            break;
    }
    
    return total_bytes > expected_size ? 0 : expected_size - total_bytes;
}

ChatConnection::~ChatConnection()
{
}

const boost::asio::ip::tcp::endpoint ChatConnection::Endpoint() const
{
    return socket_.remote_endpoint();
}
    
}
