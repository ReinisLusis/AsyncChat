//
//  chat_connection.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

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
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}

chat_connection::chat_connection(std::shared_ptr<chat_client_controller> controller, tcp::socket socket) :
controller_(controller),
socket_ (std::move(socket)),
read_state_(ReadState::Header)
{
}

void chat_connection::read()
{
    auto readHandler = boost::bind(&chat_connection::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    auto completionHandler = boost::bind(&chat_connection::completion_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    boost::asio::async_read(socket_, read_buffer_, completionHandler, readHandler);
}

void chat_connection::process_error(const std::string & message)
{
    
}

void chat_connection::write_internal()
{
    auto writeHandler = boost::bind(&chat_connection::write_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    socket_.async_send(write_queue_.front()->Buffer(), writeHandler);
}

void chat_connection::read_handler(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (error) {
        std::cerr << "read error: " << boost::system::system_error(error).what() << std::endl;
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
                std::cerr << "maximum message size exceeded " << std::endl;
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
                std::istream is(&read_buffer_);
                auto packet = chat_data_packet::Create(is);
                if (!process_message(packet->Message()))
                {
                    return;
                }
            } catch (...) {
                std::cerr << "error in chat_data_packet::Create " << std::endl;
                return;
            }
            
            read_state_ = ReadState::Header;
            break;
        }
    }
    
    if (!controller_->SusspendRead(std::shared_ptr<chat_connection>(this)))
    {
        this->read();
    } else
    {
        controller_->NotifySusspended(std::shared_ptr<chat_connection>(this));
    }
}

void chat_connection::write_handler(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (error) {
        std::cerr << "write error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    write_queue_.pop();
    
    if (!write_queue_.empty())
    {
        write_internal();
    }
}

std::size_t chat_connection::completion_handler(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (error) {
        std::cerr << "completion_handler error: " << boost::system::system_error(error).what() << std::endl;
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
