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

class chat_connection : public std::enable_shared_from_this<chat_connection>
{
public:
    
    virtual void start() = 0;
    
    void write(std::shared_ptr<chat_data_packet> packet);
    
    virtual void disconnect();

    size_t queued_message_count() const { return write_queue_.size(); }
    
    virtual ~chat_connection();

protected:
    chat_connection(boost::asio::ip::tcp::socket socket);
    
    void read();
    
    virtual bool process_message(std::shared_ptr<chat_message> message) = 0;
    
    virtual void connection_closed() = 0;
    
    void on_error();
    
private:
    enum class ReadState { Header, Body, Checksum };
    
    void write_internal();
    
    void read_handler(const boost::system::error_code& error, const size_t bytes_transferred);
    
    void write_handler(const boost::system::error_code& error, const size_t bytes_transferred);
    
    std::size_t completion_handler(const boost::system::error_code& error, std::size_t bytes_transferred);
    
    std::queue<std::shared_ptr<chat_data_packet>> write_queue_;
    ReadState read_state_;
    boost::asio::streambuf read_buffer_;
    size_t body_size_;
    boost::asio::ip::tcp::socket socket_;
};

#endif /* chat_connection_h */