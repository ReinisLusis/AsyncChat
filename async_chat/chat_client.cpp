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
    signal_set_(io_service, SIGINT, SIGTERM),
    io_service_(io_service),
    connect_timer_(io_service),
    reconnect_timer_(io_service),
    options_(options)
{
    auto signalHandler = boost::bind(&chat_client::on_signal, this, boost::asio::placeholders::error, boost::asio::placeholders::signal_number);
    signal_set_.async_wait(signalHandler);
}

void chat_client::on_signal(const boost::system::error_code& error, int signal_number)
{
    if (error) {
        std::cerr << "chat_client::on_signal error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    io_service_.stop();
}


void chat_client::connect()
{
    std::cerr << "chat_client::connect(), endpoint = " << options_.Endpoint() << std::endl;
  
    auto connectTimeoutHandler = boost::bind(&chat_client::on_connect_timeout, this, boost::asio::placeholders::error);
    connect_timer_.expires_from_now(boost::posix_time::milliseconds(options_.ConnectTimeoutMilliseconds()));
    connect_timer_.async_wait(connectTimeoutHandler);
    
    auto connectHandler = boost::bind(&chat_client::handle_connect, this, boost::asio::placeholders::error);
    socket_.async_connect(options_.Endpoint(), connectHandler);
}

void chat_client::handle_connect(const boost::system::error_code& error)
{
    if (error) {
        std::cerr << "handle_connect error: " << boost::system::system_error(error).what() << std::endl;
        
        reconnect();
        return;
    }
    
    connect_timer_.cancel();
    
    client_connection_ = std::make_shared<chat_client_connection>(this, io_service_, options_.Name(), std::move(socket_));
    client_connection_->start();
}

void chat_client::on_connect_timeout(const boost::system::error_code& error)
{
    if (error) {
        std::cerr << "on_connect_timeout error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    std::cerr << "closing connection due to timeout" << std::endl;
    socket_.close();
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
    std::cerr << "chat_client::reconnect()" << std::endl;
    
    auto reconnectHandler = boost::bind(&chat_client::on_reconnect_timer, this, boost::asio::placeholders::error);
    reconnect_timer_.expires_from_now(boost::posix_time::milliseconds(options_.ReconnectWaitMilliseconds()));
    reconnect_timer_.async_wait(reconnectHandler);
}

void chat_client::ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name)
{
    if (!name.empty())
    {
        std::cout << name << " joined chat" << std::endl;

    }
    else
    {
        input_handler_ = std::make_shared<InputHandler>(io_service_, *this);
    }
}

void chat_client::ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name, bool inactivity)
{
    if (!name.empty())
    {
        if (!inactivity)
        {
            std::cout << name << " left the chat, connection lost" << std::endl;
        }
        else
        {
            std::cout << name << " left the chat due inactivity" << std::endl;
        }
    }
    else
    {
        io_service_.stop();
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
    if (client == nullptr)
    {
        if (client_connection_ != nullptr)
        {
            client_connection_->write(chat_data_packet::Create(chat_message_text(text)));
        }
    }
    else
    {
        std::cout << name << " : " << text << std::endl;
    }
}

void chat_client::NameAlreadyInUse()
{
    std::cout << "Nickname " << options_.Name() << " is already taken" << std::endl;
}

bool chat_client::SusspendRead(std::shared_ptr<chat_connection> client)
{
    return false;
}

void chat_client::NotifySusspended(std::shared_ptr<chat_connection> client)
{
    
}

void chat_client::WriteCompleted(std::shared_ptr<chat_connection> client)
{

}
