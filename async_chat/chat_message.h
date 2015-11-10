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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class chat_message
{
public:
    static std::shared_ptr<chat_message> Deserialize(std::istream & stream);
    virtual bool Serialize(std::ostream & stream) const;
    virtual ~chat_message();
protected:
    chat_message();
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {}
};

#endif /* chat_message_h */
