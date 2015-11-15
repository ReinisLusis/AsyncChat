//
//  chat_client_connection.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "client_app.h"

#include "chat_client_connection.h"

#include "chat_client.h"
#include "../messages/chat_message_client_notice.h"
#include "../messages/chat_message_text2.h"

namespace async_chat {
using boost::asio::ip::tcp;

ChatClientConnection::ChatClientConnection(boost::asio::io_service& io_service, const std::string & name, tcp::socket socket) :
    ChatConnection(std::move(socket)),
    timer_(io_service),
    name_(name),
    connect_notice_sent_(false)
{
}

void ChatClientConnection::Start()
{
    APP->Info("ChatClientConnection::Start()");
    
    if (!connect_notice_sent_)
    {
        connect_notice_sent_ = true;
        Write(ChatDataPacket::Create(ChatMessageClientNotice(ChatMessageClientNotice::NoticeTypeEnum::Connected, name_)));
        APP->controller()->ClientConnected(shared_from_this(), std::string());
    }
    
    Read();
};

bool ChatClientConnection::ProcessMessage(std::shared_ptr<ChatMessage> message)
{
    if (auto notice = std::dynamic_pointer_cast<ChatMessageClientNotice>(message))
    {
        if (notice->NoticeType() == ChatMessageClientNotice::NoticeTypeEnum::Connected)
        {
            APP->controller()->ClientConnected(shared_from_this(), notice->Name());
        }
        else if (notice->NoticeType() == ChatMessageClientNotice::NoticeTypeEnum::Disconnected || notice->NoticeType() == ChatMessageClientNotice::NoticeTypeEnum::DisconnectedDueInactivity)
        {
            if (!notice->Name().empty())
            {
                APP->controller()->ClientDisconnected(shared_from_this(), notice->Name(), notice->NoticeType() == ChatMessageClientNotice::NoticeTypeEnum::DisconnectedDueInactivity);
            }
        }
        else if (notice->NoticeType() == ChatMessageClientNotice::NoticeTypeEnum::NameAlreadyInUse)
        {
            APP->controller()->NameAlreadyInUse();
        }
    }
    else if (auto text = std::dynamic_pointer_cast<ChatMessageText2>(message))
    {
        APP->controller()->TextReceived(shared_from_this(), text->Name(), text->Text());
    }
    else
    {
        APP->Info("ChatClientConnection::process_message() unsupported message received");
        return false;
    }
    
    return true;
}

void ChatClientConnection::OnConnectionClosed()
{
    APP->controller()->ClientDisconnected(shared_from_this(), std::string(), false);
}
    
}
