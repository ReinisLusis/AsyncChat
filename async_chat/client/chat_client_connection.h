//
//  chat_client_connection.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_client_connection_h
#define chat_client_connection_h

#include "../shared/chat_connection.h"

class chat_client_connection : public chat_connection
{
public:
    chat_client_connection(boost::asio::io_service& io_service, const std::string & name, boost::asio::ip::tcp::socket socket);
    
    virtual void start() override;

protected:
    virtual bool process_message(std::shared_ptr<chat_message> message) override;
    virtual void connection_closed() override;
    
private:
    bool connect_notice_sent_;
    std::string name_;
    boost::asio::deadline_timer timer_;
};

#endif /* chat_client_connection_h */
