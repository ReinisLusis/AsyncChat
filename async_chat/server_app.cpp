//
//  server_app.cpp
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "server_app.h"

ServerApp::ServerApp(int argc, char ** argv)
{
    options_result_ = options_.CreateOptions(argc, argv);
    server_ = std::make_shared<chat_server>(io_service_, options_);
}

int ServerApp::run()
{
    set_instance(this);
    
    if (options_result_ != 0)
    {
        return options_result_;
    }
    
    server_->accept();
    
    io_service_.run();
    
    return 0;
}

const ServerOptions & ServerApp::Options() const
{
    return options_;
}

std::shared_ptr<chat_server> ServerApp::controller()
{
    return server_;
}

ServerApp::~ServerApp()
{
    set_instance(nullptr);
}

std::shared_ptr<chat_client_controller> ServerApp::get_controller() const
{
    return server_;
}
