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

App *App::instance_ = nullptr;

App::App()
{
    
}

App::~App()
{
    
}

void App::info(const std::string & message) const
{
    std::cerr << message << std::endl;
}

void App::error(const std::string & message) const
{
    std::cerr << message << std::endl;
}

std::shared_ptr<chat_client_controller> App::controller() const
{
    return get_controller();
}

void App::output(const std::string & message) const
{
    std::cout << message << std::endl;
}

App* App::instance()
{
    return instance_;
}

void App::set_instance(App *instance)
{
    instance_ = instance;
}

