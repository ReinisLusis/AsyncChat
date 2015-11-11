//
//  chat_message_client_notice.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_client_notice.h"

chat_message_client_notice::chat_message_client_notice(const NoticeTypeEnum & noticeType, const std::string & name) : notice_type_(noticeType), name_(name) {}

chat_message_client_notice::NoticeTypeEnum chat_message_client_notice::NoticeType() const { return notice_type_; };

const std::string & chat_message_client_notice::Name() const { return name_; }

chat_message_client_notice::~chat_message_client_notice() {}

chat_message_client_notice::chat_message_client_notice() {}



