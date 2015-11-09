//
//  chat_client.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_client.h"

#include "chat_message_text.h"

#include <boost/bind.hpp>

chat_client::chat_client(boost::asio::io_service& io_service, const ClientOptions & options) :
    socket_(io_service),
    io_service_(io_service),
    connect_timer_(io_service),
    reconnect_timer_(io_service),
    options_(options)
{

}

void chat_client::connect()
{
    auto connectTimeoutHandler = boost::bind(&chat_client::on_connect_timeout, this, boost::asio::placeholders::error);
    connect_timer_.async_wait(connectTimeoutHandler);
    connect_timer_.expires_from_now(boost::posix_time::microseconds(options_.ConnectTimeoutMilliseconds()));
    
    auto connectHandler = boost::bind(&chat_client::handle_connect, this, boost::asio::placeholders::error);
    socket_.async_connect(options_.Endpoint(), connectHandler);
}

void chat_client::handle_connect(const boost::system::error_code& error)
{
    if (error) {
        std::cerr << "handle_connect error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    client_connection_ = std::make_shared<chat_client_connection>(this, io_service_, options_.Name(), std::move(socket_));
    client_connection_->start();
}

void chat_client::on_connect_timeout(const boost::system::error_code& error)
{
    if (error) {
        std::cerr << "on_connect_timeout error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    socket_.close();
    
    reconnect();
}

void chat_client::on_reconnect_timer(const boost::system::error_code& error)
{
    if (error) {
        std::cerr << "on_reconnect_timer error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    connect();
}

void chat_client::reconnect()
{
    auto reconnectHandler = boost::bind(&chat_client::on_connect_timeout, this, boost::asio::placeholders::error);
    reconnect_timer_.async_wait(reconnectHandler);
    reconnect_timer_.expires_from_now(boost::posix_time::microseconds(options_.ReconnectWaitMilliseconds()));
}

void chat_client::ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name)
{
    std::cout << name << " joined chat" << std::endl;
}

void chat_client::ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name)
{
    if (client != client_connection_)
    {
        std::cout << name << " left chat" << std::endl;
    }
}

void chat_client::ClientError(std::shared_ptr<chat_connection> client, const std::string & name)
{
    
}

void chat_client::TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name)
{
}

void chat_client::TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text)
{
    // InputHandler calls this function passing client nullptr
    if (client == nullptr)
    {
        if (client_connection_ != nullptr)
        {
            // TODO: prepare and send text message
            auto msg = chat_message_text(text);
            //client_connection_->write(<#std::shared_ptr<chat_data_packet> packet#>);
        }
    }
    else
    {
        std::cout << name << " : " << text << std::endl;
    }
}

bool chat_client::SusspendRead(std::shared_ptr<chat_connection> client)
{
    return false;
}

void chat_client::NotifySusspended(std::shared_ptr<chat_connection> client)
{
    
}
