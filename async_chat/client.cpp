#include <cstdio>
#include <iostream>
#include <queue>

#include "chat_client.h"

#include "client_options.hpp"

using boost::asio::ip::tcp;

int main(int argc, char ** argv)
{
    try {
        boost::asio::io_service io_service;
        
        ClientOptions options(io_service);
        auto init_options_result = options.CreateOptions(argc, argv);
        if (init_options_result != 0) {
            return init_options_result;
        }
        
        chat_client client(io_service, options);
        client.connect();
        
        io_service.run();
    } catch( std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << "Program has ended" << std::endl;
    getchar();
    return 0;
}