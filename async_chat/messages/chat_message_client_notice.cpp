//
//  chat_message_client_notice.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_client_notice.h"

namespace async_chat {
    
ChatMessageClientNotice::ChatMessageClientNotice(const NoticeTypeEnum & noticeType, const std::string & name) : notice_type_(noticeType), name_(name) {}

ChatMessageClientNotice::NoticeTypeEnum ChatMessageClientNotice::NoticeType() const { return notice_type_; };

const std::string & ChatMessageClientNotice::Name() const { return name_; }

ChatMessageClientNotice::~ChatMessageClientNotice() {}

ChatMessageClientNotice::ChatMessageClientNotice() {}

}