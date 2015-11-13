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
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

void chat_connection::write(std::shared_ptr<chat_data_packet> packet)
{
    if (write_queue_.empty())
    {
        write_queue_.push(packet);
        write_internal();
    }
    else
    {
        write_queue_.push(packet);
    }
}

void chat_connection::disconnect()
{
    if (socket_.is_open())
    {
        socket_.close();
    }
}

chat_connection::chat_connection(tcp::socket socket) :
    socket_ (std::move(socket)),
    read_state_(ReadState::Header){
}

void chat_connection::read()
{
    auto readHandler = boost::bind(&chat_connection::read_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    auto completionHandler = boost::bind(&chat_connection::completion_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    boost::asio::async_read(socket_, read_buffer_, completionHandler, readHandler);
}

void chat_connection::on_error()
{
    APP->controller()->ClientError(shared_from_this(), std::string());
}

void chat_connection::write_internal()
{
    auto writeHandler = boost::bind(&chat_connection::write_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    socket_.async_send(write_queue_.front()->Buffer(), writeHandler);
}

void chat_connection::read_handler(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (error) {
        APP->info(boost::format("chat_connection::read_handler() read error: %1%") % boost::system::system_error(error).what());
        
        if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
        {
            connection_closed();
        }
        else
        {
            on_error();
        }
        
        return;
    }
    
    switch (read_state_)
    {
        case ReadState::Header:
        {
            auto data = boost::asio::buffer_cast<const unsigned char*>(read_buffer_.data());
            body_size_ = static_cast<size_t>(data[0]) << 24 | static_cast<size_t>(data[1]) << 16 | static_cast<size_t>(data[2]) << 8 | static_cast<size_t>(data[3]) << 0;
            
            if (body_size_ > 65535)
            {
                APP->info("chat_connection::read_handler() maximum message size exceeded");
                on_error();
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
                auto packet = chat_data_packet::Create(read_buffer_);
                if (!process_message(packet->Message()))
                {
                    APP->info("chat_connection::read_handler() process_message failed");
                    on_error();
                    return;
                }
            } catch (...) {
                APP->info("chat_connection::read_handler() error in chat_data_packet::Create");
                on_error();
                return;
            }
            
            read_state_ = ReadState::Header;
            break;
        }
    }
    
    if (!APP->controller()->SusspendRead(shared_from_this()))
    {
        this->read();
    } else
    {
        APP->controller()->NotifySusspended(shared_from_this());
    }
}

void chat_connection::write_handler(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (error) {
        APP->info(boost::format("chat_connection::write_handler() write error: %1%") % boost::system::system_error(error).what());
        
        if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
        {
            connection_closed();
        }
        else
        {
            on_error();
        }
        
        return;
    }
    
    write_queue_.pop();
    
    auto this_ptr = shared_from_this();
    
    APP->controller()->WriteCompleted(this_ptr);
    
    if (!write_queue_.empty())
    {
        write_internal();
    }
}

std::size_t chat_connection::completion_handler(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (error) {
        APP->info(boost::format("chat_connection::completion_handler() error: %1%") % boost::system::system_error(error).what());
        
        if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
        {
            connection_closed();
        }
        else
        {
            on_error();
        }
        
        return 0;
    }
    
    auto total_bytes = read_buffer_.size() + bytes_transferred;
    size_t expected_size = 0;
    
    switch (read_state_)
    {
        case ReadState::Header:
            expected_size = chat_data_packet::HeaderSize;
            break;
            
        case ReadState::Body:
            expected_size = chat_data_packet::HeaderSize + body_size_;
            break;
        case ReadState::Checksum:
            expected_size = chat_data_packet::HeaderSize + body_size_ + chat_data_packet::ChecksumSize;
            break;
    }
    
    return total_bytes > expected_size ? 0 : expected_size - total_bytes;
}

chat_connection::~chat_connection()
{
}
