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
    chat_message_text2(const std::string & name, const std::string & text);
    
    const std::string & Name() const;
    
    virtual ~chat_message_text2();
protected:
    chat_message_text2();
    
    std::string name_;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif /* chat_message_text2_h */
