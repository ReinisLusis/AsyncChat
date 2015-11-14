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

class App
{
public:
    std::shared_ptr<chat_client_controller> controller() const;
    
    void info(const std::string & message) const;
    
    void info(const boost::format & format) const { info(boost::str(format)); }
    
    void error(const std::string & message) const;
    
    void error(const boost::format & format) const { error(boost::str(format)); }
    
    void output(const std::string & message) const;
    
    void output(const boost::format & format) const { output(boost::str(format)); }
    
    static App* instance();
    
protected:
    
    App();
    
    virtual ~App();
    
    void set_instance(App *instance);
    
    virtual std::shared_ptr<chat_client_controller> get_controller() const = 0;
    
private:
    
    static App *instance_;
};

#define APP App::instance()

#endif /* App_h */
