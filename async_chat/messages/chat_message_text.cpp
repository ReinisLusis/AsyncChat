//
//  chat_message_text.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_text.h"

namespace async_chat {

ChatMessageText::ChatMessageText(const std::string & text) : text_(text) {}

const std::string & ChatMessageText::Text() const { return text_; }

ChatMessageText::~ChatMessageText() {}

ChatMessageText::ChatMessageText() {}

}



