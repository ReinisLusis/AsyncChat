//
//  client_app.cpp
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include <stdio.h>

#include "client_app.h"

ClientApp::ClientApp(int argc, char ** argv) :
    options_(io_service_)
{
    client_ = std::make_shared<chat_client>(io_service_, options_);
    options_result_ = options_.CreateOptions(argc, argv);
}

int ClientApp::run()
{
    set_instance(this);
    
    if (options_result_ != 0)
    {
        return options_result_;
    }
    
    client_->connect();
    
    io_service_.run();

    return 0;
}

const ClientOptions & ClientApp::Options() const
{
    return options_;
}

std::shared_ptr<chat_client> ClientApp::controller()
{
    return client_;
}

ClientApp::~ClientApp()
{
    set_instance(nullptr);
}

std::shared_ptr<chat_client_controller> ClientApp::get_controller() const
{
    return client_;
}
