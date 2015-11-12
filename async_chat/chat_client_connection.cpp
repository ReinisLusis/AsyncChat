//
//  chat_client_connection.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_client_connection.h"

#include "chat_client.h"
#include "chat_message_client_notice.h"
#include "chat_message_text2.h"

using boost::asio::ip::tcp;

chat_client_connection::chat_client_connection(chat_client_controller *controller, boost::asio::io_service& io_service, const std::string & name, tcp::socket socket) :
chat_connection(controller, std::move(socket)), timer_(io_service), name_(name), connect_notice_sent_(false)
{
}

void chat_client_connection::start()
{
    std::cerr << "chat_client_connection::start()" << std::endl;
    
    if (!connect_notice_sent_)
    {
        connect_notice_sent_ = true;
        write(chat_data_packet::Create(chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::Connected, name_)));
        Controller()->ClientConnected(shared_from_this(), std::string());
    }
    
    read();
};

bool chat_client_connection::process_message(std::shared_ptr<chat_message> message)
{
    if (auto notice = std::dynamic_pointer_cast<chat_message_client_notice>(message))
    {
        if (notice->NoticeType() == chat_message_client_notice::NoticeTypeEnum::Connected)
        {
            Controller()->ClientConnected(shared_from_this(), notice->Name());
        }
        else if (notice->NoticeType() == chat_message_client_notice::NoticeTypeEnum::Disconnected || notice->NoticeType() == chat_message_client_notice::NoticeTypeEnum::DisconnectedDueInactivity)
        {
            Controller()->ClientDisconnected(shared_from_this(), notice->Name(), notice->NoticeType() == chat_message_client_notice::NoticeTypeEnum::DisconnectedDueInactivity);
        }
        else if (notice->NoticeType() == chat_message_client_notice::NoticeTypeEnum::NameAlreadyInUse)
        {
            auto client = dynamic_cast<chat_client*>(Controller());
            client->NameAlreadyInUse();
        }
    }
    else if (auto text = std::dynamic_pointer_cast<chat_message_text2>(message))
    {
        Controller()->TextReceived(shared_from_this(), text->Name(), text->Text());
    }
    else
    {
        process_error("unsupported message received");
        return false;
    }
    
    return true;
}

void chat_client_connection::connection_closed()
{
    Controller()->ClientDisconnected(shared_from_this(), std::string(), false);
}
