//
//  server_app.cpp
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "server_app.h"

namespace async_chat {
    
ServerApp::ServerApp(int argc, char ** argv)
{
    options_result_ = options_.CreateOptions(argc, argv);
    server_ = std::make_shared<ChatServer>(io_service_);
}

int ServerApp::run()
{
    SetInstance(this);
    
    if (options_result_ != 0)
    {
        return options_result_;
    }
    
    server_->Accept();
    
    io_service_.run();
    
    return 0;
}

const ServerOptions & ServerApp::Options() const
{
    return options_;
}

std::shared_ptr<ChatServer> ServerApp::controller()
{
    return server_;
}

ServerApp::~ServerApp()
{
    SetInstance(nullptr);
}

std::shared_ptr<ChatClientController> ServerApp::GetController() const
{
    return server_;
}

}