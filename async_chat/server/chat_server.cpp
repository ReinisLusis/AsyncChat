//
//  chat_server.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "server_app.h"
#include "chat_server.h"
#include "chat_server_connection.h"

#include "../shared/chat_connection.h"
#include "../messages/chat_message_client_notice.h"
#include "../messages/chat_message_text2.h"


#include <boost/bind.hpp>

namespace async_chat {
    
ChatServer::ChatServer(boost::asio::io_service& io_service, const ServerOptions & options) :
    options_(options),
    signal_set_(io_service, SIGINT, SIGTERM),
    acceptor_(io_service, options.Endpoint()),
    io_service_(io_service),
    socket_(io_service),
    susspend_read_(false),
    resume_read_timer_(io_service)
{
    auto signalHandler = boost::bind(&ChatServer::OnSignal, this, boost::asio::placeholders::error, boost::asio::placeholders::signal_number);
    signal_set_.async_wait(signalHandler);
}

void ChatServer::OnSignal(const boost::system::error_code& error, int signal_number)
{
    if (error) {
        APP->Info(boost::format("ChatServer::OnSignal() error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    susspended_clients_.clear();
    clients_.clear();    
    
    io_service_.stop();
}

void ChatServer::ClientConnected(std::shared_ptr<ChatConnection> client, const std::string & name)
{
    if (client_names_.find(name) == client_names_.end())
    {
        client_names_.insert(name);
    }
    else
    {
        disconnect_on_write_completed_clients_.insert(client);
        client->Write(ChatDataPacket::Create(ChatMessageClientNotice(ChatMessageClientNotice::NoticeTypeEnum::NameAlreadyInUse, name)));
    }
};

void ChatServer::ClientDisconnected(std::shared_ptr<ChatConnection> client, const std::string & name, bool inactivity)
{
    RemoveClient(client, name);
    
    auto packet = ChatDataPacket::Create(ChatMessageClientNotice(ChatMessageClientNotice::NoticeTypeEnum::Disconnected, name));
    BroadcastPacket(packet, client);
};

void ChatServer::WriteCompleted(std::shared_ptr<ChatConnection> client)
{
    if (disconnect_on_write_completed_clients_.find(client) != disconnect_on_write_completed_clients_.end())
    {
        client->Disconnect();
    }
}

void ChatServer::ClientError(std::shared_ptr<ChatConnection> client, const std::string & name)
{
    RemoveClient(client, name);
    client->Disconnect();
    
    auto packet = ChatDataPacket::Create(ChatMessageClientNotice(ChatMessageClientNotice::NoticeTypeEnum::Disconnected, name));
    BroadcastPacket(packet, client);
};

void ChatServer::TimerExpired(std::shared_ptr<ChatConnection> client, const std::string & name)
{
    RemoveClient(client, name);
    client->Disconnect();
    
    if (!name.empty())
    {
        auto packet = ChatDataPacket::Create(ChatMessageClientNotice(ChatMessageClientNotice::NoticeTypeEnum::DisconnectedDueInactivity, name));
        BroadcastPacket(packet, client);
    }
}

void ChatServer::RemoveClient(std::shared_ptr<ChatConnection> client, const std::string & name)
{
    client_names_.erase(name);
    susspended_clients_.erase(client);
    disconnect_on_write_completed_clients_.erase(client);
    clients_.erase(client);
}

void ChatServer::TextReceived(std::shared_ptr<ChatConnection> client, const std::string name, const std::string & text)
{
    APP->Info(boost::format("ChatServer::TextReceived() %1%, %2%") % name % text);
    
    auto packet = ChatDataPacket::Create(ChatMessageText2(name, text));
    BroadcastPacket(packet, nullptr);
};

bool ChatServer::SusspendRead(std::shared_ptr<ChatConnection> client)
{
    if (disconnect_on_write_completed_clients_.find(client) != disconnect_on_write_completed_clients_.end())
    {
        return true;
    }
    
    return susspend_read_;
};

void ChatServer::NotifySusspended(std::shared_ptr<ChatConnection> client)
{
    if (disconnect_on_write_completed_clients_.find(client) == disconnect_on_write_completed_clients_.end())
    {
        susspended_clients_.insert(client);
    }
};

void ChatServer::BroadcastPacket(std::shared_ptr<ChatDataPacket> packet, std::shared_ptr<ChatConnection> excludeClient)
{
    size_t total_queued_message_count = 0;
    
    for (auto client : clients_)
    {
        if (client != excludeClient)
        {
            if (client->queued_message_count() < Options().ClientMaxMessageCount())
            {
                client->Write(packet);
            }
            else
            {
                client->Disconnect();
            }
            
            total_queued_message_count += client->queued_message_count();
        }
    }
    
    if (clients_.size() > 0 &&
        (total_queued_message_count / clients_.size() > Options().AverageWriteQueueLengthPerClientBeforeSusspend()))
    {
        SusspendRead();
    }
    else
    {
        ResumeRead();
    }
}

void ChatServer::OnResumeReadTimer(const boost::system::error_code& error)
{
    if (error) {
        APP->Info(boost::format("ChatServer::OnResumeReadTimer() error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    size_t total_queued_message_count = 0;
    
    for (auto client : clients_)
    {
        total_queued_message_count += client->queued_message_count();
    }
    
    if (clients_.size() > 0 &&
        (total_queued_message_count / clients_.size() > Options().AverageWriteQueueLengthPerClientBeforeSusspend()))
    {
        // keep susspended, just retriger timer
        auto timerHandler = boost::bind(&ChatServer::OnResumeReadTimer, this, boost::asio::placeholders::error);
        resume_read_timer_.expires_from_now(boost::posix_time::seconds(options_.ResumeReadTimerTimeoutSeconds()));
        resume_read_timer_.async_wait(timerHandler);
    }
    else
    {
        ResumeRead();
    }
}

void ChatServer::SusspendRead()
{
    if (!susspend_read_)
    {
        APP->Info("ChatServer::SusspendRead()");
    
        auto timerHandler = boost::bind(&ChatServer::OnResumeReadTimer, this, boost::asio::placeholders::error);
        resume_read_timer_.expires_from_now(boost::posix_time::seconds(options_.ResumeReadTimerTimeoutSeconds()));
        resume_read_timer_.async_wait(timerHandler);
        
        susspend_read_ = true;
    }
}

void ChatServer::ResumeRead()
{
    if (susspend_read_)
    {
        APP->Info("ChatServer::ResumeRead()");
        
        resume_read_timer_.cancel();
        susspend_read_ = false;
        for (auto client : susspended_clients_)
        {
            std::dynamic_pointer_cast<ChatServerConnection>(client)->ResumeRead();
        }
        susspended_clients_.clear();
    }
}

void ChatServer::Accept()
{
    auto acceptHandler = boost::bind(&ChatServer::OnAccept, this, boost::asio::placeholders::error);
    acceptor_.async_accept(socket_, acceptHandler);
}

void ChatServer::OnAccept(const boost::system::error_code& error)
{
    if (!error)
    {
        auto client = std::make_shared<ChatServerConnection>(io_service_, std::move(socket_));
        clients_.insert(client);
        client->Start();
    }
    else
    {
        APP->Info(boost::format("ChatServer::OnAccept() error: %1%") % boost::system::system_error(error).what());
        io_service_.stop();
    }
    
    Accept();
}
    
}
