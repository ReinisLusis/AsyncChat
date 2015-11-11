#include <cstdio>
#include <iostream>
#include <queue>

#include "chat_server.h"

#include "server_options.hpp"

using boost::asio::ip::tcp;

int main(int argc, char ** argv)
{
    try {
        ServerOptions options;
        auto init_options_result = options.CreateOptions(argc, argv);
        if (init_options_result != 0) {
            return init_options_result;
        }
        
        boost::asio::io_service io_service;
        
        chat_server server(io_service, options);
        
        io_service.run();
    } catch( std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << "Program has ended" << std::endl;
    getchar();
    return 0;
}