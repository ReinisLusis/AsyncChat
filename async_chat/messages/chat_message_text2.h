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

namespace async_chat
{
    class ChatMessageText2 : public ChatMessageText
    {
    public:
        ChatMessageText2(const std::string & name, const std::string & text);
        
        const std::string & Name() const;
        
        
        virtual ~ChatMessageText2();
    protected:
        ChatMessageText2();
        
        std::string name_;    
       
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ChatMessageText);
            ar & BOOST_SERIALIZATION_NVP(name_);
            ar & BOOST_SERIALIZATION_NVP(text_);
        }
    };
}

#endif /* chat_message_text2_h */
