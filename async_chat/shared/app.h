//
//  App.h
//  async_chat
//
//  Created by Reinis on 13/11/15.
//  Copyright © 2015 Reinis. All rights reserved.
//

#ifndef App_h
#define App_h

#include "chat_client_controller.h"

#include <boost/format.hpp>

namespace async_chat
{
    class App
    {
    public:
        std::shared_ptr<ChatClientController> controller() const;
        
        void Info(const std::string & message) const;
        
        void Info(const boost::format & format) const { Info(boost::str(format)); }
        
        void Error(const std::string & message) const;
        
        void Error(const boost::format & format) const { Error(boost::str(format)); }
        
        void Output(const std::string & message) const;
        
        void Output(const boost::format & format) const { Output(boost::str(format)); }
        
        static App* instance();
        
    protected:
        
        App();
        
        virtual ~App();
        
        void SetInstance(App *instance);
        
        virtual std::shared_ptr<ChatClientController> GetController() const = 0;
        
    private:
        
        static App *instance_;
    };
}
#define APP App::instance()

#endif /* App_h */
