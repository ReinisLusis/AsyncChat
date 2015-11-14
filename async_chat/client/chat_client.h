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

#include "../shared/chat_client_controller.h"
#include "../shared/input_handler.h"
#include "../messages/chat_data_packet.h"

#include "client_options.h"
#include "chat_client_connection.h"

#include <set>
#include <boost/asio.hpp>

namespace async_chat
{
    class ChatClient : public ChatClientController
    {
    public:
        ChatClient(boost::asio::io_service& io_service, const ClientOptions & options);
        
        virtual void ClientConnected(std::shared_ptr<ChatConnection> client, const std::string & name) override;
        
        virtual void ClientDisconnected(std::shared_ptr<ChatConnection> client, const std::string & name, bool inactivity) override;
        
        virtual void ClientError(std::shared_ptr<ChatConnection> client, const std::string & name) override;
        
        virtual void WriteCompleted(std::shared_ptr<ChatConnection> client) override;
        
        virtual void TimerExpired(std::shared_ptr<ChatConnection> client, const std::string & name) override;
        
        virtual void TextReceived(std::shared_ptr<ChatConnection> client, const std::string name, const std::string & text) override;
        
        virtual bool SusspendRead(std::shared_ptr<ChatConnection> client) override;
        
        virtual void NotifySusspended(std::shared_ptr<ChatConnection> client) override;

        void NameAlreadyInUse();
        
        void Connect();
        
    private:
        void OnConnect(const boost::system::error_code& error);
        
        void OnConnectTimer(const boost::system::error_code& error);
        
        void OnReconnectTimer(const boost::system::error_code& error);
        
        void Reconnect();
        
        void OnSignal(const boost::system::error_code& error, int signal_number);
        
        boost::asio::ip::tcp::socket socket_;
        boost::asio::io_service& io_service_;
        boost::asio::signal_set signal_set_;
        boost::asio::deadline_timer connect_timer_;
        boost::asio::deadline_timer reconnect_timer_;
        const ClientOptions & options_;
        std::shared_ptr<ChatClientConnection> client_connection_;
        std::shared_ptr<InputHandler> input_handler_;
    };
}

#endif /* chat_client_hpp */
