//
//  chat_client_connection.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_client_connection_h
#define chat_client_connection_h

#include "../shared/chat_connection.h"

namespace async_chat
{
    class ChatClientConnection : public ChatConnection
    {
    public:
        ChatClientConnection(boost::asio::io_service& io_service, const std::string & name, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
        
        virtual void Start() override;

    protected:
        virtual bool ProcessMessage(std::shared_ptr<ChatMessage> message) override;
        virtual void OnConnectionClosed() override;
        
    private:
        bool connect_notice_sent_;
        std::string name_;
        boost::asio::deadline_timer timer_;
    };
}

#endif /* chat_client_connection_h */
