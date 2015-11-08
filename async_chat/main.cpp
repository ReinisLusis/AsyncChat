#include <cstdio>
#include <iostream>
#include <queue>

#include "chat_server.h"
#include "input_handler.h"

#include <boost/asio/write.hpp>
#include <boost/program_options.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>



#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

using boost::asio::ip::tcp;

static int _port_number;

static int init_options(int argc, char ** argv)
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
        _port_number = vm["p"].as<int>();
        if (_port_number < 1 || _port_number > 65535)
        {
            std::cout << "Port number " << _port_number << " exceeded allowable range 1 - 65535 " << std::endl;
            return 1;
        }
        
    } else {
        std::cout << "Port number was not set." << std::endl;
        return 1;
    }
    
    return 0;
}

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
        auto init_options_result = init_options(argc, argv);
        if (init_options_result != 0) {
            return init_options_result;
        }
        
        tcp::endpoint endpoint(tcp::v4(), _port_number);
        
        boost::asio::io_service io_service;
        InputHandler inputHandler(io_service);
        io_service.run();
    } catch( std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << "Program has ended" << std::endl;
    getchar();
    return 0;
}