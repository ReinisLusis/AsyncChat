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
    chat_server_connection(std::shared_ptr<chat_client_controller> controller, boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket);
    
    void start() override;
    
protected:
    bool process_message(std::shared_ptr<chat_message> message) override;
    
private:
    void setTimer();    
    void on_timer(const boost::system::error_code& error);
    
    bool client_notice_received_;
    boost::asio::deadline_timer timer_;
    std::string name_;
};

#endif /* chat_server_connection_h */
