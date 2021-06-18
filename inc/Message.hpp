
#pragma once

# include <iostream>
# include <list>
# include <stdlib.h>

class Message
{
  private:
    typedef std::list<std::string> t_list;
    std::string split_first_char(std::string &str, char separator);
    std::list<std::string> split_char(std::string str, char separator);
    std::string command;
    t_list params;

public:
  Message();
  Message(std::string nick, std::string user, std::string real, std::string pass);
  Message( Message const & src );
  virtual ~Message();
  Message &operator=( Message const & rhs );

  void disp_mess(Message mess);
  void parsing_cmd(char buf[5000]);

  //Getter & Setter
  t_list getParams(void) { return (this->params); }
  std::string getCmd(void) { return (this->command); }
  void setParams(t_list v) { this->params = v; }
  void setCmd(std::string str) { this->command = str; }
};

// std::ostream &			operator<<( std::ostream & o, Message const & i );
