//
//  chat_server_connection.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_server_connection.h"

#include "chat_message_client_notice.h"
#include "chat_message_text.h"

chat_server_connection::chat_server_connection(chat_client_controller *controller, boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket) :
chat_connection(controller, std::move(socket)), timer_(io_service), client_notice_received_(false)
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
                process_error("first message Connected notice expected");
                return false;
            }
            
            if (notice->Name().empty())
            {
                process_error("nick name is empty");
                return false;
            }
            
            name_ = notice->Name();
            
            client_notice_received_ = true;
            
            Controller()->ClientConnected(shared_chat_connection(), name_);
        }
        else
        {
            process_error("first message connect notice expected");
            return false;
        }
    }
    else
    {
        if (auto text = std::dynamic_pointer_cast<chat_message_text>(message))
        {
            Controller()->TextReceived(shared_chat_connection(), name_, text->Text());
        }
        else
        {
            process_error("only chat_message_text expected after notice");
            return false;
        }
    }
    
    setTimer();
    
    return true;
}

void chat_server_connection::setTimer()
{
    auto timerHandler = boost::bind(&chat_server_connection::on_timer, this, boost::asio::placeholders::error);
    timer_.expires_from_now(boost::posix_time::milliseconds(10000));
    timer_.async_wait(timerHandler);
}

void chat_server_connection::on_timer(const boost::system::error_code& error)
{
    if (error) {
        std::cerr << "on_timer error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    Controller()->TimerExpired(shared_chat_connection(), name_);
}
