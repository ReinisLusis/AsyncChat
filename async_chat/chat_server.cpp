//
//  chat_server.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

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
    susspend_read_(false)
{
    auto signalHandler = boost::bind(&chat_server::on_signal, this, boost::asio::placeholders::error, boost::asio::placeholders::signal_number);
    signal_set_.async_wait(signalHandler);
    
    accept();
}

void chat_server::on_signal(const boost::system::error_code& error, int signal_number)
{
    if (error) {
        std::cerr << "chat_client::on_signal error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    io_service_.stop();
}

void chat_server::ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name)
{
};

void chat_server::ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name, bool inactivity)
{
    clients_.erase(client);
    client->disconnect();
    
    chat_message_client_notice disconnect_notice(chat_message_client_notice::NoticeTypeEnum::Disconnected, name);
    auto packet = chat_data_packet::Create(&disconnect_notice);
    BroadcastPacket(packet, client);
};

void chat_server::ClientError(std::shared_ptr<chat_connection> client, const std::string & name)
{
    clients_.erase(client);
    client->disconnect();
    
    chat_message_client_notice disconnect_notice(chat_message_client_notice::NoticeTypeEnum::Disconnected, name);
    auto packet = chat_data_packet::Create(&disconnect_notice);
    BroadcastPacket(packet, client);
};

void chat_server::TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name)
{
    clients_.erase(client);
    client->disconnect();
    
    if (!name.empty())
    {
        chat_message_client_notice disconnect_notice(chat_message_client_notice::NoticeTypeEnum::DisconnectedDueInactivity, name);
        auto packet = chat_data_packet::Create(&disconnect_notice);
        BroadcastPacket(packet, client);
    }
}

void chat_server::TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text)
{
    chat_message_text2 text_message(name, text);
    auto packet = chat_data_packet::Create(&text_message);
    BroadcastPacket(packet, nullptr);
};

bool chat_server::SusspendRead(std::shared_ptr<chat_connection> client)
{
    return false;
};

void chat_server::NotifySusspended(std::shared_ptr<chat_connection> client)
{
    susspended_clients_.insert(client);
};

void chat_server::BroadcastPacket(std::shared_ptr<chat_data_packet> packet, std::shared_ptr<chat_connection> excludeClient)
{
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
        }
    }
}

void chat_server::accept()
{
    std::cerr << "chat_server::accept(), endpoint = " << options_.Endpoint() << std::endl;

    auto acceptHandler = boost::bind(&chat_server::accept_handler, this, boost::asio::placeholders::error);
    acceptor_.async_accept(socket_, acceptHandler);
}

void chat_server::accept_handler(const boost::system::error_code& error)
{
    if (!error)
    {
        auto client = std::make_shared<chat_server_connection>(this, io_service_, std::move(socket_));
        clients_.insert(client);
        client->start();
    }
    else
    {
        std::cerr << "chat_server::accept_handler error: " << boost::system::system_error(error).what() << std::endl;
    }
    
    accept();
}
