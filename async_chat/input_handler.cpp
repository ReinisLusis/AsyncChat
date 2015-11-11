//
//  input_handler.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "input_handler.h"

#include <cstdio>
#include <iostream>

#include <boost/bind.hpp>

InputHandler::InputHandler( boost::asio::io_service& io_service, chat_client_controller &controller) : _input(io_service), controller_(controller)
{
    _input.assign( STDIN_FILENO );
    read();
}

void InputHandler::read()
{
    auto readHandler = boost::bind(&InputHandler::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    boost::asio::async_read(_input, boost::asio::buffer(&_command, sizeof(_command)), readHandler);
}

void InputHandler::read_handler(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if (error) {
        std::cerr << "read error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }
    
    if (_command == '\n')
    {
        boost::asio::streambuf::const_buffers_type buf = _input_buffer.data();
        std::string str(boost::asio::buffers_begin(buf), boost::asio::buffers_end(buf));
        _input_buffer.consume(_input_buffer.size());
        
        controller_.TextReceived(nullptr, std::string(), str);
    }
    else
    {
        std::ostream os (&_input_buffer);
        os << _command;
    }
    
    this->read();
}
