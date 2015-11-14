//
//  server_app.h
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef server_app_h
#define server_app_h

#include "app.h"

#include "server_options.h"
#include "chat_server.h"

#include <boost/asio.hpp>

class ServerApp : public App
{
public:
    ServerApp(int argc, char ** argv);
    
    int run();
    
    const ServerOptions & Options() const;
    
    std::shared_ptr<chat_server> controller();
    
    virtual ~ServerApp() override;
    
private:
    
    virtual std::shared_ptr<chat_client_controller> get_controller() const override;
    
    boost::asio::io_service io_service_;
    ServerOptions options_;
    std::shared_ptr<chat_server> server_;
    int options_result_;
};

#undef APP
#define APP (dynamic_cast<ServerApp*>(App::instance()))

#endif /* server_app_h */
