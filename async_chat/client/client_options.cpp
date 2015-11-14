//
//  client_options.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "client_options.h"

#include <cstdio>
#include <iostream>
#include <boost/program_options.hpp>

using boost::asio::ip::tcp;

ClientOptions::ClientOptions(boost::asio::io_service & io_service) : resolver_(io_service)
{
    
}

int ClientOptions::CreateOptions(int argc, char ** argv)
{
    using namespace boost::program_options;
    options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("p", value<int>(), "port number to listen for incoming connections")
        ("a", value<std::string>(), "server address")
        ("n", value<std::string>(), "nick name")
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
    
    std::string address;
    if (vm.count("a")) {
        address = vm["a"].as<std::string>();
        if (address.empty())
        {
            std::cout << "Empty address provided." << std::endl;
            return 1;
        }
    } else {
        std::cout << "Server address was not provided." << std::endl;
        return 1;
    }
    
    if (vm.count("n")) {
        name_ = vm["n"].as<std::string>();
        
        if (name_.empty())
        {
            std::cout << "Nick name not valid" << std::endl;
            return 1;
        }
    } else {
        std::cout << "Nick name was not specified." << std::endl;
        return 1;
    }
    
    tcp::resolver::query query(address, boost::lexical_cast<std::string>(port_));
    tcp::resolver::iterator endpoint_iterator = resolver_.resolve(query);
    tcp::resolver::iterator end;
    if (endpoint_iterator == end)
    {
        std::cout << "Unable to resolve server" << std::endl;
        return 1;
    }
    
    endpoint_ = *endpoint_iterator;
    
    return 0;
}

int ClientOptions::Port() const
{
    return port_;
}

std::string ClientOptions::Name() const
{
    return name_;
}

tcp::endpoint ClientOptions::Endpoint() const
{
    return endpoint_;
}

int ClientOptions::ConnectTimeoutMilliseconds() const
{
    return 3000;
}

int ClientOptions::ReconnectWaitMilliseconds() const
{
    return 5000;
}


