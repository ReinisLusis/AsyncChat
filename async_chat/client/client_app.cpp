//
//  client_app.cpp
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include <stdio.h>

#include "client_app.h"

namespace async_chat {
    
ClientApp::ClientApp(int argc, char ** argv) :
    options_(io_service_)
{
    client_ = std::make_shared<ChatClient>(io_service_, options_);
    options_result_ = options_.CreateOptions(argc, argv);
}

int ClientApp::run()
{
    SetInstance(this);
    
    if (options_result_ != 0)
    {
        return options_result_;
    }
    
    client_->Connect();
    
    io_service_.run();

    return 0;
}

const ClientOptions & ClientApp::Options() const
{
    return options_;
}

std::shared_ptr<ChatClient> ClientApp::controller()
{
    return client_;
}

ClientApp::~ClientApp()
{
    SetInstance(nullptr);
}

std::shared_ptr<ChatClientController> ClientApp::GetController() const
{
    return client_;
}

}