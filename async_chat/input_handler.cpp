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
        std::istream is( &_input_buffer );
        std::string text;
        is >> text;
        
        controller_.TextReceived(nullptr, std::string(), text);
    }
    else
    {
        std::ostream os (&_input_buffer);
        os << _command;
    }
    
    this->read();
}
