//
//  chat_message_client_notice.cpp
//  async_chat
//
//  Created by Reinis on 09/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#include "chat_message_client_notice.h"

BOOST_CLASS_VERSION(chat_message_client_notice, 0)
BOOST_CLASS_EXPORT_GUID(chat_message_client_notice, "e466249933824d6189848eb760904f3f")

chat_message_client_notice::chat_message_client_notice(const NoticeTypeEnum & noticeType, const std::string & name) : notice_type_(noticeType), name_(name) {}

chat_message_client_notice::NoticeTypeEnum chat_message_client_notice::NoticeType() const { return notice_type_; };

const std::string & chat_message_client_notice::Name() const { return name_; }

chat_message_client_notice::~chat_message_client_notice() {}

chat_message_client_notice::chat_message_client_notice() {}

bool chat_message_client_notice::Serialize(std::ostream & stream) const
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
void chat_message_client_notice::serialize(Archive & ar, const unsigned int version)
{
    ar & boost::serialization::base_object<chat_message>(*this);
    ar & notice_type_;
    ar & name_;
}


