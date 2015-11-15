//
//  chat_server_connection.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "server_app.h"
#include "chat_server_connection.h"
#include "chat_server.h"

#include "../messages/chat_message_text.h"
#include "../messages/chat_message_client_notice.h"

namespace async_chat {
    
ChatServerConnection::ChatServerConnection(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket) :
    ChatConnection(std::move(socket)),
    timer_(io_service),
    client_notice_received_(false)
{
}

ChatServerConnection::~ChatServerConnection()
{
    APP->Info(boost::format("ChatServerConnection::~ChatServerConnection() %1%") % Name());
}
    
void ChatServerConnection::Start()
{
    SetTimer();
    Read();
};

bool ChatServerConnection::ProcessMessage(std::shared_ptr<ChatMessage> message)
{
    if (!client_notice_received_)
    {
        if (auto notice = std::dynamic_pointer_cast<ChatMessageClientNotice>(message))
        {
            if (notice->NoticeType() != ChatMessageClientNotice::NoticeTypeEnum::Connected)
            {
                APP->Info("ChatServerConnection::ProcessMessage() first message Connected notice expected");
                return false;
            }
            
            if (notice->Name().empty())
            {
                APP->Info("ChatServerConnection::ProcessMessage() nick name is empty");
                return false;
            }
            
            name_ = notice->Name();
            
            client_notice_received_ = true;
            
            APP->controller()->ClientConnected(shared_from_this(), name_);
        }
        else
        {
            APP->Info("ChatServerConnection::ProcessMessage() first message connect notice expected");
            return false;
        }
    }
    else
    {
        if (auto text = std::dynamic_pointer_cast<ChatMessageText>(message))
        {
            APP->controller()->TextReceived(shared_from_this(), name_, text->Text());
        }
        else
        {
            APP->Info("ChatServerConnection::ProcessMessage() only chat_message_text expected after notice");
            return false;
        }
    }
    
    SetTimer();
    
    return true;
}

void ChatServerConnection::OnConnectionClosed()
{
    APP->controller()->ClientDisconnected(shared_from_this(), name_, false);
}

void ChatServerConnection::SetTimer()
{
    auto timerHandler = boost::bind(&ChatServerConnection::OnTimer, std::dynamic_pointer_cast<ChatServerConnection>(shared_from_this()), boost::asio::placeholders::error);
    timer_.expires_from_now(boost::posix_time::seconds(APP->Options().ClientInactivityTimeoutSeconds()));
    timer_.async_wait(timerHandler);
}

void ChatServerConnection::OnTimer(const boost::system::error_code& error)
{
    if (error) {
        APP->Info(boost::format("ChatServerConnection::OnTimer() error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    APP->controller()->TimerExpired(shared_from_this(), name_);
}

void ChatServerConnection::Disconnect()
{
    timer_.cancel();
    ChatConnection::Disconnect();
}

void ChatServerConnection::ResumeRead()
{
    Read();
}

}