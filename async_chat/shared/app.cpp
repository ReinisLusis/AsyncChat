//
//  app.cpp
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "app.h"

#include <stdio.h>
#include <iostream>

namespace async_chat {
    
App *App::instance_ = nullptr;

App::App()
{
    
}

App::~App()
{
    
}

void App::Info(const std::string & message) const
{
    std::cerr << message << std::endl;
}

void App::Error(const std::string & message) const
{
    std::cerr << message << std::endl;
}

std::shared_ptr<ChatClientController> App::controller() const
{
    return GetController();
}

void App::Output(const std::string & message) const
{
    std::cout << message << std::endl;
}

App* App::instance()
{
    return instance_;
}

void App::SetInstance(App *instance)
{
    instance_ = instance;
}

}