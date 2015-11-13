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
#include "chat_message_client_notice.h"
#include "chat_message_text.h"

chat_server_connection::chat_server_connection(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket) :
    chat_connection(std::move(socket)),
    timer_(io_service),
    client_notice_received_(false)
{
}

void chat_server_connection::start()
{
    setTimer();
    read();
};

bool chat_server_connection::process_message(std::shared_ptr<chat_message> message)
{
    if (!client_notice_received_)
    {
        if (auto notice = std::dynamic_pointer_cast<chat_message_client_notice>(message))
        {
            if (notice->NoticeType() != chat_message_client_notice::NoticeTypeEnum::Connected)
            {
                APP->info("chat_server_connection::process_message() first message Connected notice expected");
                return false;
            }
            
            if (notice->Name().empty())
            {
                APP->info("chat_server_connection::process_message() nick name is empty");
                return false;
            }
            
            name_ = notice->Name();
            
            client_notice_received_ = true;
            
            APP->controller()->ClientConnected(shared_from_this(), name_);
        }
        else
        {
            APP->info("chat_server_connection::process_message() first message connect notice expected");
            return false;
        }
    }
    else
    {
        if (auto text = std::dynamic_pointer_cast<chat_message_text>(message))
        {
            APP->controller()->TextReceived(shared_from_this(), name_, text->Text());
        }
        else
        {
            APP->info("chat_server_connection::process_message() only chat_message_text expected after notice");
            return false;
        }
    }
    
    setTimer();
    
    return true;
}

void chat_server_connection::connection_closed()
{
    APP->controller()->ClientDisconnected(shared_from_this(), name_, false);
}

void chat_server_connection::setTimer()
{
    auto timerHandler = boost::bind(&chat_server_connection::on_timer, std::dynamic_pointer_cast<chat_server_connection>(shared_from_this()), boost::asio::placeholders::error);
    timer_.expires_from_now(boost::posix_time::seconds(APP->Options().ClientInactivityTimeoutSeconds()));
    timer_.async_wait(timerHandler);
}

void chat_server_connection::on_timer(const boost::system::error_code& error)
{
    if (error) {
        APP->info(boost::format("chat_server_connection::on_timer() on_timer error: %1%") % boost::system::system_error(error).what());
        return;
    }
    
    APP->controller()->TimerExpired(shared_from_this(), name_);
}

void chat_server_connection::disconnect()
{
    timer_.cancel();
    chat_connection::disconnect();
}

void chat_server_connection::resumeRead()
{
    read();
}

