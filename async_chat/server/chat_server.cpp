//
//  chat_server.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "server_app.h"

#include "chat_server.h"

#include "chat_connection.h"
#include "chat_message_client_notice.h"
#include "chat_message_text2.h"
#include "chat_server_connection.h"

#include <boost/bind.hpp>

chat_server::chat_server(boost::asio::io_service& io_service, const ServerOptions & options) :
    options_(options),
    signal_set_(io_service, SIGINT, SIGTERM),
    acceptor_(io_service, options.Endpoint()),
    io_service_(io_service),
    socket_(io_service),
    susspend_read_(false),
    resume_read_timer(io_service)
{
    auto signalHandler = boost::bind(&chat_server::on_signal, this, boost::asio::placeholders::error, boost::asio::placeholders::signal_number);
    signal_set_.async_wait(signalHandler);
}

void chat_server::on_signal(const boost::system::error_code& error, int signal_number)
{
    if (error) {
        APP->info(boost::format("chat_server::on_signal() error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    susspended_clients_.clear();
    clients_.clear();    
    
    io_service_.stop();
}

void chat_server::ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name)
{
    if (client_names_.find(name) == client_names_.end())
    {
        client_names_.insert(name);
    }
    else
    {
        disconnect_on_write_completed_clients_.insert(client);
        client->write(chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::NameAlreadyInUse, name)));
    }
};

void chat_server::ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name, bool inactivity)
{
    RemoveClient(client, name);
    
    auto packet = chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::Disconnected, name));
    BroadcastPacket(packet, client);
};

void chat_server::WriteCompleted(std::shared_ptr<chat_connection> client)
{
    if (disconnect_on_write_completed_clients_.find(client) != disconnect_on_write_completed_clients_.end())
    {
        client->disconnect();
    }
}

void chat_server::ClientError(std::shared_ptr<chat_connection> client, const std::string & name)
{
    RemoveClient(client, name);
    client->disconnect();
    
    auto packet = chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::Disconnected, name));
    BroadcastPacket(packet, client);
};

void chat_server::TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name)
{
    RemoveClient(client, name);
    client->disconnect();
    
    if (!name.empty())
    {
        auto packet = chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::DisconnectedDueInactivity, name));
        BroadcastPacket(packet, client);
    }
}

void chat_server::RemoveClient(std::shared_ptr<chat_connection> client, const std::string & name)
{
    client_names_.erase(name);
    susspended_clients_.erase(client);
    disconnect_on_write_completed_clients_.erase(client);
    clients_.erase(client);
}

void chat_server::TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text)
{
    APP->info(boost::format("chat_server::TextReceived() %1%, %2%") % name % text);
    
    auto packet = chat_data_packet::Create(chat_message_text2(name, text));
    BroadcastPacket(packet, nullptr);
};

bool chat_server::SusspendRead(std::shared_ptr<chat_connection> client)
{
    if (disconnect_on_write_completed_clients_.find(client) != disconnect_on_write_completed_clients_.end())
    {
        return true;
    }
    
    return susspend_read_;
};

void chat_server::NotifySusspended(std::shared_ptr<chat_connection> client)
{
    if (disconnect_on_write_completed_clients_.find(client) == disconnect_on_write_completed_clients_.end())
    {
        susspended_clients_.insert(client);
    }
};

void chat_server::BroadcastPacket(std::shared_ptr<chat_data_packet> packet, std::shared_ptr<chat_connection> excludeClient)
{
    size_t total_queued_message_count = 0;
    
    for (auto client : clients_)
    {
        if (client != excludeClient)
        {
            if (client->queued_message_count() < Options().ClientMaxMessageCount())
            {
                client->write(packet);
            }
            else
            {
                client->disconnect();
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

void chat_server::on_resume_read_timer(const boost::system::error_code& error)
{
    if (error) {
        APP->info(boost::format("chat_server::on_resume_read_timer() error: %1%") % boost::system::system_error(error).what());
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
        auto timerHandler = boost::bind(&chat_server::on_resume_read_timer, this, boost::asio::placeholders::error);
        resume_read_timer.expires_from_now(boost::posix_time::seconds(options_.ResumeReadTimerTimeoutSeconds()));
        resume_read_timer.async_wait(timerHandler);
    }
    else
    {
        ResumeRead();
    }
}

void chat_server::SusspendRead()
{
    if (!susspend_read_)
    {
        APP->info("chat_server::SusspendRead()");
    
        auto timerHandler = boost::bind(&chat_server::on_resume_read_timer, this, boost::asio::placeholders::error);
        resume_read_timer.expires_from_now(boost::posix_time::seconds(options_.ResumeReadTimerTimeoutSeconds()));
        resume_read_timer.async_wait(timerHandler);
        
        susspend_read_ = true;
    }
}

void chat_server::ResumeRead()
{
    if (susspend_read_)
    {
        APP->info("chat_server::ResumeRead()");
        
        resume_read_timer.cancel();
        susspend_read_ = false;
        for (auto client : susspended_clients_)
        {
            std::dynamic_pointer_cast<chat_server_connection>(client)->resumeRead();
        }
        susspended_clients_.clear();
    }
}

void chat_server::accept()
{
    auto acceptHandler = boost::bind(&chat_server::accept_handler, this, boost::asio::placeholders::error);
    acceptor_.async_accept(socket_, acceptHandler);
}

void chat_server::accept_handler(const boost::system::error_code& error)
{
    if (!error)
    {
        auto client = std::make_shared<chat_server_connection>(io_service_, std::move(socket_));
        clients_.insert(client);
        client->start();
    }
    else
    {
        APP->info(boost::format("chat_server::accept_handler error: %1%") % boost::system::system_error(error).what());
        io_service_.stop();
    }
    
    accept();
}
