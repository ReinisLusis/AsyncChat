//
//  input_handler.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "client_app.h"

#include "input_handler.h"

#include <boost/bind.hpp>

namespace async_chat {
    
InputHandler::InputHandler( boost::asio::io_service& io_service) : input_(io_service)
{
    input_.assign( STDIN_FILENO );
    Read();
}

void InputHandler::Read()
{
    auto readHandler = boost::bind(&InputHandler::OnRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    boost::asio::async_read(input_, boost::asio::buffer(&command_, sizeof(command_)), readHandler);
}

void InputHandler::OnRead(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (error) {
        APP->Error(boost::format("inputHandler::OnRead() error %1%") % boost::system::system_error(error).what());
        return;
    }
    
    if (command_ == '\n')
    {
        boost::asio::streambuf::const_buffers_type buf = input_buffer_.data();
        std::string str(boost::asio::buffers_begin(buf), boost::asio::buffers_end(buf));
        input_buffer_.consume(input_buffer_.size());
        
        APP->controller()->TextInput(str);
    }
    else
    {
        std::ostream os (&input_buffer_);
        os << command_;
    }
    
    this->Read();
}

}