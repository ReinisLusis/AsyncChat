//
//  input_handler.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef input_handler_h
#define input_handler_h

#include "chat_client_controller.h"

#include <boost/asio.hpp>

namespace async_chat
{
    class InputHandler {
    public:
        InputHandler(boost::asio::io_service& io_service);

    private:
        void Read();
        void OnRead(const boost::system::error_code& error, const size_t bytes_transferred);
        
        boost::asio::streambuf input_buffer_;
        boost::asio::posix::stream_descriptor input_;
        char command_;
    };
}
#endif /* input_handler_h */
