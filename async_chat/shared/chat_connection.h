//
//  chat_connection.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_connection_h
#define chat_connection_h

#include "../messages/chat_data_packet.h"
#include "chat_client_controller.h"

#include <queue>
#include <boost/asio.hpp>

namespace async_chat
{
    class ChatConnection : public std::enable_shared_from_this<ChatConnection>
    {
    public:
        
        virtual void Start() = 0;
        
        void Write(std::shared_ptr<ChatDataPacket> packet);
        
        virtual void Disconnect();

        size_t queued_message_count() const { return write_queue_.size(); }
        
        const boost::asio::ip::tcp::endpoint Endpoint() const;
        
        virtual ~ChatConnection();

    protected:
        ChatConnection(boost::asio::ip::tcp::socket socket);
        
        void Read();
        
        virtual bool ProcessMessage(std::shared_ptr<ChatMessage> message) = 0;
        
        virtual void OnConnectionClosed() = 0;
        
        void OnError();
        
    private:
        enum class ReadState { Header, Body, Checksum };
        
        void WriteInternal();
        
        void OnRead(const boost::system::error_code& error, const size_t bytes_transferred);
        
        void OnWrite(const boost::system::error_code& error, const size_t bytes_transferred);
        
        std::size_t OnCompletion(const boost::system::error_code& error, std::size_t bytes_transferred);
        
        std::queue<std::shared_ptr<ChatDataPacket>> write_queue_;
        ReadState read_state_;
        boost::asio::streambuf read_buffer_;
        size_t body_size_;
        boost::asio::ip::tcp::socket socket_;
    };
}

#endif /* chat_connection_h */
