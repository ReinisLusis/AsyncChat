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

namespace async_chat
{
    class ChatServer : public ChatClientController
    {
    public:
        ChatServer(boost::asio::io_service& io_service, const ServerOptions & options);
        
        virtual void ClientConnected(std::shared_ptr<ChatConnection> client, const std::string & name) override;
        
        virtual void ClientDisconnected(std::shared_ptr<ChatConnection> client, const std::string & name, bool inactivity) override;
        
        virtual void ClientError(std::shared_ptr<ChatConnection> client, const std::string & name) override;
        
        virtual void WriteCompleted(std::shared_ptr<ChatConnection> client) override;
        
        virtual void TimerExpired(std::shared_ptr<ChatConnection> client, const std::string & name) override;
        
        virtual void TextReceived(std::shared_ptr<ChatConnection> client, const std::string name, const std::string & text) override;
        
        virtual bool SusspendRead(std::shared_ptr<ChatConnection> client) override;
        
        virtual void NotifySusspended(std::shared_ptr<ChatConnection> client) override;

        inline const ServerOptions& Options() const { return options_; }

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
        boost::asio::ip::tcp::socket socket_;
        boost::asio::signal_set signal_set_;
        boost::asio::io_service& io_service_;
        std::set<std::shared_ptr<ChatConnection>> clients_;
        std::set<std::shared_ptr<ChatConnection>> susspended_clients_;
        std::set<std::string> client_names_;
        std::set<std::shared_ptr<ChatConnection>> disconnect_on_write_completed_clients_;
        const ServerOptions & options_;
        bool susspend_read_;
        boost::asio::deadline_timer resume_read_timer_;
    };
}

#endif /* chat_server_h */
