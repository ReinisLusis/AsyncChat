//
//  chat_server_connection.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_server_connection_h
#define chat_server_connection_h

#include "chat_connection.h"

#include <boost/bind.hpp>

class chat_server_connection : public chat_connection
{
public:
    chat_server_connection(std::shared_ptr<chat_client_controller> controller, boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket) :
    chat_connection(controller, std::move(socket)), timer_(io_service), client_notice_received_(false)
    {
    }
    
    void start() override
    {
        setTimer();
        read();
    };
protected:
    bool process_message(std::shared_ptr<chat_message> message) override
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
                
                Controller()->ClientConnected(std::shared_ptr<chat_connection>(this), name_);
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
                Controller()->TextReceived(std::shared_ptr<chat_connection>(this), name_, text->Text());
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
    
private:
    void setTimer()
    {
        auto timerHandler = boost::bind(&chat_server_connection::on_timer, this, boost::asio::placeholders::error);
        timer_.expires_from_now(boost::posix_time::milliseconds(10000));
        timer_.async_wait(timerHandler);
    }
    
    void on_timer(const boost::system::error_code& error)
    {
        if (error) {
            std::cerr << "on_timer error: " << boost::system::system_error(error).what() << std::endl;
            return;
        }
        
        Controller()->TimerExpired(std::shared_ptr<chat_connection>(this), name_);
    }
    
    bool client_notice_received_;
    boost::asio::deadline_timer timer_;
    std::string name_;
};

#endif /* chat_server_connection_h */
