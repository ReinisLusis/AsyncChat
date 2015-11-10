//
//  chat_message.cpp
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message.h"

#include "chat_message_client_notice.h"
#include "chat_message_text.h"
#include "chat_message_text2.h"

BOOST_CLASS_VERSION(chat_message, 0)
BOOST_CLASS_EXPORT_GUID(chat_message, "4f6d42c335884c3898f329d2854b94fe")

std::shared_ptr<chat_message> chat_message::Deserialize(std::istream & stream)
{
    chat_message * message;
    try {
        boost::archive::text_iarchive i(stream);
        i.template register_type<chat_message_client_notice>();
        i.template register_type<chat_message_text>();
        i.template register_type<chat_message_text2>();
        i >> message;
        return std::shared_ptr<chat_message>(message);
    } catch (...) {
        return std::shared_ptr<chat_message>(nullptr);
    }
}

bool chat_message::Serialize(std::ostream & stream) const
{
    try {
        boost::archive::text_oarchive a(stream);
        a << this;
        return true;
    } catch (...) {
        return false;
    }
};

chat_message::~chat_message() {}

chat_message::chat_message() {}
