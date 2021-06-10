
#pragma once

# include <iostream>
# include <list>
# include <stdlib.h>

class Message
{

public:

    typedef std::list<std::string> t_list;

    Message();
    Message(std::string nick, std::string user, std::string real, std::string pass);
    Message( Message const & src );
    virtual ~Message();

    Message &   operator=( Message const & rhs );

    t_list getParams(void){
      return (this->params);
    }
    void setParams(t_list v) {
      this->params = v;
    }
    std::string getCmd(void){
      return (this->command);
    }
    void setCmd(std::string str) {
      this->command = str;
    }

private:

    std::string command;
    t_list params;

};

// std::ostream &			operator<<( std::ostream & o, Message const & i );
