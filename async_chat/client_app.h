//
//  client_app.h
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef client_app_h
#define client_app_h

#include "app.h"

#include "client_options.hpp"
#include "chat_client.h"

#include <boost/asio.hpp>

class ClientApp : public App
{
public:
    ClientApp(int argc, char ** argv);
    
    int run();
    
    const ClientOptions & Options() const;
    
    std::shared_ptr<chat_client> controller();
    
    virtual ~ClientApp() override;
    
private:
    
    virtual std::shared_ptr<chat_client_controller> get_controller() const override;
    
    boost::asio::io_service io_service_;
    ClientOptions options_;
    std::shared_ptr<chat_client> client_;
    int options_result_;
};

#undef APP
#define APP (dynamic_cast<ClientApp*>(App::instance()))

#endif /* client_app_h */
