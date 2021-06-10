
#pragma once

# include <iostream>
# include <vector>
# include <stdlib.h>

class Message
{

public:

    Message();
    Message(std::string nick, std::string user, std::string real, std::string pass);
    Message( Message const & src );
    virtual ~Message();

    Message &   operator=( Message const & rhs );


private:

    std::string command;
    std::vector<std::string> params;

};

// std::ostream &			operator<<( std::ostream & o, Message const & i );
