//
//  server_options.hpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef server_options_hpp
#define server_options_hpp

#include <stdio.h>

#include <boost/asio.hpp>

class ServerOptions
{
public:
    ServerOptions();
    
    int CreateOptions(int argc, char ** argv);
    
    int Port() const;
    
    boost::asio::ip::tcp::endpoint Endpoint() const;
    
private:
    
    int port_;
};

#endif /* server_options_hpp */
