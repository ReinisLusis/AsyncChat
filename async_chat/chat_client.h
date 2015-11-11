//
//  chat_client.hpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_client_hpp
#define chat_client_hpp

#include <stdio.h>

#include "client_options.hpp"

#include "chat_client_controller.h"
#include "chat_data_packet.h"

#include "chat_client_connection.h"

#include <set>
#include <boost/asio.hpp>

class chat_client : public chat_client_controller
{
public:
    chat_client(boost::asio::io_service& io_service, const ClientOptions & options);
    
    void ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    void ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name, bool inactivity) override;
    
    void ClientError(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    void TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    void TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text) override;
    
    bool SusspendRead(std::shared_ptr<chat_connection> client) override;
    
    void NotifySusspended(std::shared_ptr<chat_connection> client) override;

    void connect();
    
private:
    void handle_connect(const boost::system::error_code& error);
    
    void on_connect_timeout(const boost::system::error_code& error);
    
    void on_reconnect_timer(const boost::system::error_code& error);
    
    void reconnect();
    
    void on_signal(const boost::system::error_code& error, int signal_number);
    
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_service& io_service_;
    boost::asio::signal_set signal_set_;
    boost::asio::deadline_timer connect_timer_;
    boost::asio::deadline_timer reconnect_timer_;
    const ClientOptions & options_;
    std::shared_ptr<chat_client_connection> client_connection_;
};

#endif /* chat_client_hpp */
