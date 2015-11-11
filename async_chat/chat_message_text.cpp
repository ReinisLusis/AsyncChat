//
//  chat_message_text.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_text.h"

chat_message_text::chat_message_text(const std::string & text) : text_(text) {}

const std::string & chat_message_text::Text() const { return text_; }

chat_message_text::~chat_message_text() {}

chat_message_text::chat_message_text() {}




