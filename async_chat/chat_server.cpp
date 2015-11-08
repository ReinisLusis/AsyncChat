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

chat_server::chat_server(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint) :
    acceptor_(io_service, endpoint),
    io_service_(io_service),
    socket_(io_service),
    susspend_read_(false)
{
    accept();
}

void chat_server::ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name)
{
};

void chat_server::ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name)
{
    clients_.erase(client);
    client->disconnect();
    
    auto packet = chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::Disconnected, name));
    BroadcastPacket(packet, client);
};

void chat_server::ClientError(std::shared_ptr<chat_connection> client, const std::string & name)
{
    clients_.erase(client);
    client->disconnect();
    
    auto packet = chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::Disconnected, name));
    BroadcastPacket(packet, client);
};

void chat_server::TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name)
{
    clients_.erase(client);
    client->disconnect();
    
    if (!name.empty())
    {
        auto packet = chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::Disconnected, name));
        BroadcastPacket(packet, client);
    }
}

void chat_server::TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text)
{
    auto packet = chat_data_packet::Create(chat_message_text2(name, text));
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
            client->write(packet);
        }
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
        auto client = std::make_shared<chat_server_connection>(std::shared_ptr<chat_client_controller>(this), io_service_, std::move(socket_));
        clients_.insert(client);
        client->start();
    }
    
    accept();
}
