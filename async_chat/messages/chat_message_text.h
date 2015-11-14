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

namespace async_chat
{
    class ChatMessageText : public ChatMessage
    {
    public:
        ChatMessageText(const std::string & text);
        
        const std::string & Text() const;
        
        virtual ~ChatMessageText();
    protected:
        ChatMessageText();
        
        std::string text_;
        
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ChatMessage);
            ar & BOOST_SERIALIZATION_NVP(text_);
        }
    };
}
#endif /* chat_message_text_h */
