#include <cstdio>
#include <iostream>
#include <queue>

#include "chat_server.h"

#include "server_options.hpp"

using boost::asio::ip::tcp;

int main(int argc, char ** argv)
{
    /*boost::asio::streambuf buf;

    {
        std::ostream os(&buf);
        auto msg = chat_message_client_notice(chat_message_client_notice::NoticeTypeEnum::Connected, "Reinis");
        msg.Serialize(os);
    }
    
    {
        std::istream is(&buf);
        auto msg = chat_message::Deserialize(is);
        
        if (auto notice = std::dynamic_pointer_cast<chat_message_client_notice>(msg))
        {
            std::cout << notice->Name() << std::endl;
        }

    }*/
    
    try {
        ServerOptions options;
        auto init_options_result = options.CreateOptions(argc, argv);
        if (init_options_result != 0) {
            return init_options_result;
        }
        
        tcp::endpoint endpoint(tcp::v4(), options.Port());
        
        boost::asio::io_service io_service;
        
        chat_server server(io_service, endpoint);
        
        io_service.run();
    } catch( std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << "Program has ended" << std::endl;
    getchar();
    return 0;
}