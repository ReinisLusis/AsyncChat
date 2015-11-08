//
//  chat_message_client_notice.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_message_client_notice_h
#define chat_message_client_notice_h

#include "chat_message.h"

class chat_message_client_notice : public chat_message
{
public:
    enum class NoticeTypeEnum { Connected, Disconnected };
    
    chat_message_client_notice(const NoticeTypeEnum & noticeType, const std::string & name) : notice_type_(noticeType), name_(name) {}
    
    NoticeTypeEnum NoticeType() const { return notice_type_; };
    
    const std::string & Name() const { return name_; }
    
    virtual ~chat_message_client_notice() {}
protected:
    chat_message_client_notice() {}
    
    NoticeTypeEnum notice_type_;
    std::string name_;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<chat_message>(*this);
        ar & notice_type_;
        ar & name_;
    }
};
BOOST_CLASS_VERSION(chat_message_client_notice, 0)
BOOST_CLASS_EXPORT_GUID(chat_message_client_notice, "e466249933824d6189848eb760904f3f")

#endif /* chat_message_client_notice_h */
