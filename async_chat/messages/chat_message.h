//
//  chat_message.h
//  async_chat
//
//  Created by Reinis on 08/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef chat_message_h
#define chat_message_h

#include <boost/serialization/export.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

namespace async_chat
{
    class ChatMessage
    {
    public:
        virtual ~ChatMessage();

    protected:
        ChatMessage();
        
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {}
    };
}
#endif /* chat_message_h */
