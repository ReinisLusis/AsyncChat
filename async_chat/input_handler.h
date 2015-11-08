//
//  input_handler.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef input_handler_h
#define input_handler_h

#include <boost/asio.hpp>

class InputHandler {
public:
    InputHandler( boost::asio::io_service& io_service);

private:
    void read();
    void read_handler(const boost::system::error_code& error, const size_t bytes_transferred);
    
    boost::asio::streambuf _input_buffer;
    boost::asio::posix::stream_descriptor _input;
    char _command;
};

#endif /* input_handler_h */
