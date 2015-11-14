//
//  chat_message_text2.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_text2.h"

namespace async_chat {
    
ChatMessageText2::ChatMessageText2(const std::string & name, const std::string & text) : name_(name), ChatMessageText(text) {}

const std::string & ChatMessageText2::Name() const { return name_; }

ChatMessageText2::ChatMessageText2() {}


ChatMessageText2::~ChatMessageText2() {}

}





