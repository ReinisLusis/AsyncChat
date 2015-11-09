//
//  client_options.hpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef client_options_hpp
#define client_options_hpp

#include <stdio.h>
#include <string>

#include <boost/asio.hpp>

class ClientOptions
{
public:
    ClientOptions(boost::asio::io_service & io_service);
    
    int CreateOptions(int argc, char ** argv);
    
    int Port() const;
    
    boost::asio::ip::tcp::endpoint Endpoint() const;
    
    std::string Name() const;
    
    int ConnectTimeoutMilliseconds() const;
    
    int ReconnectWaitMilliseconds() const;
private:
    
    int port_;
    boost::asio::ip::tcp::endpoint endpoint_;
    std::string name_;
    
    boost::asio::ip::tcp::resolver resolver_;
};

#endif /* client_options_hpp */
