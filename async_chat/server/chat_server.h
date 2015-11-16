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

#include "server_options.h"
#include "../shared/chat_client_controller.h"
#include "../messages/chat_data_packet.h"

#include <set>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>

namespace async_chat
{
    class ChatServer : public ChatClientController
    {
    public:
        ChatServer(boost::asio::io_service& io_service);
        

        virtual void ClientError(std::shared_ptr<ChatConnection> client) override;
        
        virtual void WriteCompleted(std::shared_ptr<ChatConnection> client) override;
        
        virtual bool SusspendRead(std::shared_ptr<ChatConnection> client) override;
        
        virtual void NotifySusspended(std::shared_ptr<ChatConnection> client) override;
        
        void TimerExpired(std::shared_ptr<ChatConnection> client, const std::string & name);
        
        void ClientConnected(std::shared_ptr<ChatConnection> client, const std::string & name);
        
        void ClientDisconnected(std::shared_ptr<ChatConnection> client, const std::string & name);
        
        void TextReceived(std::shared_ptr<ChatConnection> client, const std::string name, const std::string & text);

        void Accept();
        
    private:
        void BroadcastPacket(std::shared_ptr<ChatDataPacket> packet, std::shared_ptr<ChatConnection> excludeClient);
        
        void OnAccept(const boost::system::error_code& error);
        
        void OnSignal(const boost::system::error_code& error, int signal_number);
        
        void OnResumeReadTimer(const boost::system::error_code& error);
        
        void RemoveClient(std::shared_ptr<ChatConnection> client, const std::string & name);
        
        void SusspendRead();
        
        void ResumeRead();
        
        boost::asio::ip::tcp::acceptor acceptor_;
        std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
        boost::asio::signal_set signal_set_;
        boost::asio::io_service& io_service_;
        std::set<std::shared_ptr<ChatConnection>> clients_;
        std::set<std::shared_ptr<ChatConnection>> susspended_clients_;
        std::set<std::string> client_names_;
        std::set<std::shared_ptr<ChatConnection>> disconnect_on_write_completed_clients_;
        bool susspend_read_;
        boost::asio::deadline_timer resume_read_timer_;
    };
}

#endif /* chat_server_h */
