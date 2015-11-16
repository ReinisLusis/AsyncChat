//
//  chat_client.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "client_app.h"

#include "chat_client.h"

#include "../messages/chat_message_text.h"

#include <boost/bind.hpp>

namespace async_chat {
    
ChatClient::ChatClient(boost::asio::io_service& io_service) :
    socket_(io_service),
    signal_set_(io_service, SIGINT, SIGTERM),
    io_service_(io_service),
    connect_timer_(io_service),
    reconnect_timer_(io_service)
{
    auto signalHandler = boost::bind(&ChatClient::OnSignal, this, boost::asio::placeholders::error, boost::asio::placeholders::signal_number);
    signal_set_.async_wait(signalHandler);
}

void ChatClient::OnSignal(const boost::system::error_code& error, int signal_number)
{
    if (error) {
        APP->Info(boost::format("ChatClient::OnSignal() error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    io_service_.stop();
}


void ChatClient::Connect()
{
    APP->Info(boost::format("ChatClient::Connect() endpoint: %1%") % APP->Options().Endpoint());
  
    auto connectTimeoutHandler = boost::bind(&ChatClient::OnConnectTimer, this, boost::asio::placeholders::error);
    connect_timer_.expires_from_now(boost::posix_time::milliseconds(APP->Options().ConnectTimeoutMilliseconds()));
    connect_timer_.async_wait(connectTimeoutHandler);
    
    auto connectHandler = boost::bind(&ChatClient::OnConnect, this, boost::asio::placeholders::error);
    socket_.async_connect(APP->Options().Endpoint(), connectHandler);
}

void ChatClient::OnConnect(const boost::system::error_code& error)
{
    if (error) {
        APP->Info(boost::format("ChatClient::OnConnect() error: %1%") % boost::system::system_error(error).what());

        Reconnect();
        return;
    }
    
    connect_timer_.cancel();
    
    client_connection_ = std::make_shared<ChatClientConnection>(io_service_, APP->Options().Name(), std::move(socket_));
    client_connection_->Start();
}

void ChatClient::OnConnectTimer(const boost::system::error_code& error)
{
    if (error) {
        APP->Info(boost::format("ChatClient::OnConnectTimer() error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    APP->Info("ChatClient::OnConnectTimer() closing connection due to timeout");
    socket_.close();
}

void ChatClient::OnReconnectTimer(const boost::system::error_code& error)
{
    if (error) {
        APP->Info(boost::format("ChatClient::OnReconnectTimer() error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    Connect();
}

void ChatClient::Reconnect()
{
    APP->Info("ChatClient::Reconnect()");
    
    auto reconnectHandler = boost::bind(&ChatClient::OnReconnectTimer, this, boost::asio::placeholders::error);
    reconnect_timer_.expires_from_now(boost::posix_time::milliseconds(APP->Options().ReconnectWaitMilliseconds()));
    reconnect_timer_.async_wait(reconnectHandler);
}

void ChatClient::ClientConnected(std::shared_ptr<ChatConnection> client, const std::string & name)
{
    APP->Output(boost::format("%1% joined chat") % name);
}

void ChatClient::ClientDisconnected(std::shared_ptr<ChatConnection> client, const std::string & name, bool inactivity)
{
    if (!name.empty())
    {
        if (!inactivity)
        {
            APP->Output(boost::format("%1% left the chat, connection lost") % name);
        }
        else
        {
            APP->Output(boost::format("%1% left the chat due inactivity") % name);
        }
    }
    else
    {
        io_service_.stop();
    }
}
    
void ChatClient::StartInput()
{
    input_handler_ = std::make_shared<InputHandler>(io_service_);
}

void ChatClient::ClientError(std::shared_ptr<ChatConnection> client)
{
    client_connection_->Disconnect();
    io_service_.stop();
}

void ChatClient::TextReceived(std::shared_ptr<ChatConnection> client, const std::string name, const std::string & text)
{
    APP->Output(boost::format("%1% : %2%") % name % text);
}
    
void ChatClient::TextInput(const std::string & text)
{
    client_connection_->Write(ChatDataPacket::Create(ChatMessageText(text)));
}

void ChatClient::NameAlreadyInUse()
{
    APP->Output(boost::format("Nickname %1% is already taken") % APP->Options().Name());
}

bool ChatClient::SusspendRead(std::shared_ptr<ChatConnection> client)
{
    return false;
}

void ChatClient::NotifySusspended(std::shared_ptr<ChatConnection> client)
{
}

void ChatClient::WriteCompleted(std::shared_ptr<ChatConnection> client)
{
}

}