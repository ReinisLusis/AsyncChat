//
//  chat_message_text2.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_message_text2_h
#define chat_message_text2_h

#include "chat_message_text.h"

class chat_message_text2 : public chat_message_text
{
public:
    chat_message_text2(const std::string & name, const std::string & text) : chat_message_text(text), name_(name) {}
    
    const std::string & Name() const { return name_; }
    
    virtual ~chat_message_text2() {}
protected:
    chat_message_text2() {}
    
    std::string name_;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<chat_message_text>(*this);
        ar & name_;
    }
};
BOOST_CLASS_VERSION(chat_message_text2, 0)
BOOST_CLASS_EXPORT_GUID(chat_message_text2, "6e977a2bf9b540c7a5731711b4853ae5")

#endif /* chat_message_text2_h */
