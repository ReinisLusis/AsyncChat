//
//  chat_message_text2.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_text2.h"

chat_message_text2::chat_message_text2(const std::string & name, const std::string & text) : name_(name), chat_message_text(text) {}

const std::string & chat_message_text2::Name() const { return name_; }

chat_message_text2::chat_message_text2() {}


chat_message_text2::~chat_message_text2() {}





