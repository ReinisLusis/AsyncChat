//
//  chat_server_connection.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_server_connection_h
#define chat_server_connection_h

#include "../shared/chat_connection.h"

namespace async_chat
{
    class ChatServerConnection : public ChatConnection
    {
    public:
        ChatServerConnection(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket);
        
        virtual void Start() override;

        void ResumeRead();
        
        const std::string & Name() const { return name_; }
        
        virtual void Disconnect() override;
        
        virtual ~ChatServerConnection() override;
    protected:
        virtual bool ProcessMessage(std::shared_ptr<ChatMessage> message) override;
        virtual void OnConnectionClosed() override;
        
    private:
        void SetTimer();
        void OnTimer(const boost::system::error_code& error);
        
        bool client_notice_received_;
        boost::asio::deadline_timer timer_;
        std::string name_;
    };
}
#endif /* chat_server_connection_h */
