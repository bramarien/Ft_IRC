
#include "../inc/Message.hpp"

Message::Message(){
}

Message::Message( Message const & src ) : params(src.params){
        this->command = src.command;
}

Message::~Message(){
}

Message &   Message::operator=(Message const & rhs ){
        this->command = rhs.command;
        return(*this);
}
