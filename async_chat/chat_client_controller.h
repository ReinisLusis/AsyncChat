//
//  chat_client_controller.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_client_controller_h
#define chat_client_controller_h

#include <string>

class chat_connection;

class chat_client_controller
{
public:
    virtual void ClientConnected(std::shared_ptr<chat_connection> client, const std::string & name) = 0;
    virtual void ClientDisconnected(std::shared_ptr<chat_connection> client, const std::string & name, bool inactivity) = 0;
    virtual void ClientError(std::shared_ptr<chat_connection> client, const std::string & name) = 0;
    virtual void WriteCompleted(std::shared_ptr<chat_connection> client) = 0;
    virtual void TimerExpired(std::shared_ptr<chat_connection> client, const std::string & name) = 0;
    virtual void TextReceived(std::shared_ptr<chat_connection> client, const std::string name, const std::string & text) = 0;
    virtual bool SusspendRead(std::shared_ptr<chat_connection> client) = 0;
    virtual void NotifySusspended(std::shared_ptr<chat_connection> client) = 0;
};

#endif /* chat_client_controller_h */
