//
//  chat_server.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_server_h
#define chat_server_h

#include <stdio.h>

#include "chat_client_controller.h"
#include "chat_data_packet.h"

#include <set>
#include <boost/asio.hpp>

class chat_server : public chat_client_controller
{
public:
    chat_server(boost::asio::io_service& io_service,
                const boost::asio::ip::tcp::endpoint& endpoint);
    
    void ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    void ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    void ClientError(std::shared_ptr<chat_connection> client, const std::string & name)override;
    
    void TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    void TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text) override;
    
    bool SusspendRead(std::shared_ptr<chat_connection> client) override;
    
    void NotifySusspended(std::shared_ptr<chat_connection> client) override;

private:
    void BroadcastPacket(std::shared_ptr<chat_data_packet> packet, std::shared_ptr<chat_connection> excludeClient);
    
    void accept();
    
    void accept_handler(const boost::system::error_code& error);
    
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_service& io_service_;
    std::set<std::shared_ptr<chat_connection>> clients_;
    std::set<std::shared_ptr<chat_connection>> susspended_clients_;
    bool susspend_read_;
};


#endif /* chat_server_h */
