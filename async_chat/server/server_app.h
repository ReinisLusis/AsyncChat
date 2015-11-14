//
//  server_app.h
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef server_app_h
#define server_app_h

#include "../shared/app.h"

#include "server_options.h"
#include "chat_server.h"

#include <boost/asio.hpp>

namespace async_chat
{
    class ServerApp : public App
    {
    public:
        ServerApp(int argc, char ** argv);
        
        int run();
        
        const ServerOptions & Options() const;
        
        std::shared_ptr<ChatServer> controller();
        
        virtual ~ServerApp() override;
        
    private:
        
        virtual std::shared_ptr<ChatClientController> GetController() const override;
        
        boost::asio::io_service io_service_;
        ServerOptions options_;
        std::shared_ptr<ChatServer> server_;
        int options_result_;
    };
}
#undef APP
#define APP (dynamic_cast<ServerApp*>(App::instance()))

#endif /* server_app_h */
