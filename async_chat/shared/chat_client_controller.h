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

namespace async_chat
{
    class ChatConnection;

    class ChatClientController : public std::enable_shared_from_this<ChatClientController>
    {
    public:
        virtual void ClientError(std::shared_ptr<ChatConnection> client) = 0;
        virtual void WriteCompleted(std::shared_ptr<ChatConnection> client) = 0;
        virtual bool SusspendRead(std::shared_ptr<ChatConnection> client) = 0;
        virtual void NotifySusspended(std::shared_ptr<ChatConnection> client) = 0;
    };
}
#endif /* chat_client_controller_h */
