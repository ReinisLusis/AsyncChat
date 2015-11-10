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
    
    bool Serialize(std::ostream & stream) const override;
    
    chat_message_client_notice(const NoticeTypeEnum & noticeType, const std::string & name);
    
    NoticeTypeEnum NoticeType() const;
    
    const std::string & Name() const;
    
    virtual ~chat_message_client_notice();
protected:
    chat_message_client_notice();
    
    NoticeTypeEnum notice_type_;
    std::string name_;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif /* chat_message_client_notice_h */
