//
//  chat_message_text2.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_text2.h"

BOOST_CLASS_VERSION(chat_message_text2, 0)
BOOST_CLASS_EXPORT_GUID(chat_message_text2, "6e977a2bf9b540c7a5731711b4853ae5")

chat_message_text2::chat_message_text2(const std::string & name, const std::string & text) : chat_message_text(text), name_(name) {}

const std::string & chat_message_text2::Name() const { return name_; }

chat_message_text2::~chat_message_text2() {}

chat_message_text2::chat_message_text2() {}

template<class Archive>
void chat_message_text2::serialize(Archive & ar, const unsigned int version)
{
    ar & boost::serialization::base_object<chat_message_text>(*this);
    ar & name_;
}


