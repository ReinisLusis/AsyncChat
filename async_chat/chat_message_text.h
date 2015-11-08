//
//  chat_message_text.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_message_text_h
#define chat_message_text_h

#include "chat_message.h"

class chat_message_text : public chat_message
{
public:
    chat_message_text(const std::string & text) : text_(text) {}
    
    const std::string & Text() const { return text_; }
    
    virtual ~chat_message_text() {}
protected:
    chat_message_text() {}
    
    std::string text_;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<chat_message>(*this);
        ar & text_;
    }
};
BOOST_CLASS_VERSION(chat_message_text, 0)
BOOST_CLASS_EXPORT_GUID(chat_message_text, "9749d49edf3d4b01847ab53158f18b79")

#endif /* chat_message_text_h */
