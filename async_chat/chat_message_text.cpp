//
//  chat_message_text.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_text.h"

BOOST_CLASS_VERSION(chat_message_text, 0)
BOOST_CLASS_EXPORT_GUID(chat_message_text, "9749d49edf3d4b01847ab53158f18b79")

chat_message_text::chat_message_text(const std::string & text) : text_(text) {}

const std::string & chat_message_text::Text() const { return text_; }

chat_message_text::~chat_message_text() {}

chat_message_text::chat_message_text() {}

bool chat_message_text::Serialize(std::ostream & stream) const
{
    try {
        boost::archive::text_oarchive a(stream);
        a << this;
        return true;
    } catch (...) {
        return false;
    }
};

template<class Archive>
void chat_message_text::serialize(Archive & ar, const unsigned int version)
{
    ar & boost::serialization::base_object<chat_message>(*this);
    ar & text_;
}


