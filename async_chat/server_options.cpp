//
//  server_options.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "server_options.hpp"

#include <cstdio>
#include <iostream>
#include <boost/program_options.hpp>

using boost::asio::ip::tcp;

ServerOptions::ServerOptions()
{
    
}

int ServerOptions::CreateOptions(int argc, char ** argv)
{
    using namespace boost::program_options;
    options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("p", value<int>(), "port number to listen for incoming connections")
    ;
    
    variables_map vm;
    auto parse_style =
    command_line_style::unix_style |
    command_line_style::allow_long_disguise ;
    store(parse_command_line(argc, argv, desc, parse_style), vm);
    notify(vm);
    
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    if (vm.count("p")) {
        port_ = vm["p"].as<int>();
        if (port_ < 1 || port_ > 65535)
        {
            std::cout << "Port number " << port_ << " exceeded allowable range 1 - 65535 " << std::endl;
            return 1;
        }
        
    } else {
        std::cout << "Port number was not set." << std::endl;
        return 1;
    }
    
    return 0;
}

int ServerOptions::Port() const
{
    return port_;
}

tcp::endpoint ServerOptions::Endpoint() const
{
    return tcp::endpoint(tcp::v4(), port_);
}

int ServerOptions::ClientInactivityTimeoutSeconds() const
{
    return 20;
}

int ServerOptions::ClientMaxMessageCount() const
{
    return 5;
}

int ServerOptions::AverageWriteQueueLengthPerClientBeforeSusspend() const
{
    return 3;
}

int ServerOptions::ResumeReadTimerTimeoutSeconds() const
{
    return 2;
}

