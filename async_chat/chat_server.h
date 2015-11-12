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

#include "server_options.hpp"
#include "chat_client_controller.h"
#include "chat_data_packet.h"

#include <set>
#include <boost/asio.hpp>

class chat_server : public chat_client_controller
{
public:
    chat_server(boost::asio::io_service& io_service, const ServerOptions & options);
    
    virtual void ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    virtual void ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name, bool inactivity) override;
    
    virtual void ClientError(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    virtual void WriteCompleted(std::shared_ptr<chat_connection> client) override;
    
    virtual void TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name) override;
    
    virtual void TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text) override;
    
    virtual bool SusspendRead(std::shared_ptr<chat_connection> client) override;
    
    virtual void NotifySusspended(std::shared_ptr<chat_connection> client) override;

    inline const ServerOptions& Options() const { return options_; }

    void accept();
    
private:
    void BroadcastPacket(std::shared_ptr<chat_data_packet> packet, std::shared_ptr<chat_connection> excludeClient);
    
    void accept_handler(const boost::system::error_code& error);
    
    void on_signal(const boost::system::error_code& error, int signal_number);
    
    void on_resume_read_timer(const boost::system::error_code& error);
    
    void RemoveClient(std::shared_ptr<chat_connection> client, const std::string & name);
    
    void SusspendRead();
    
    void ResumeRead();
    
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::signal_set signal_set_;
    boost::asio::io_service& io_service_;
    std::set<std::shared_ptr<chat_connection>> clients_;
    std::set<std::shared_ptr<chat_connection>> susspended_clients_;
    std::set<std::string> client_names_;
    std::set<std::shared_ptr<chat_connection>> disconnect_on_write_completed_clients_;
    const ServerOptions & options_;
    bool susspend_read_;
    boost::asio::deadline_timer resume_read_timer;
};


#endif /* chat_server_h */
