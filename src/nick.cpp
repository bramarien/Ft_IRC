#include "../inc/nick.hpp"

bool  nick_check(std::string nick){
        return(false);
}

int   nick_fct(Message msg){
        if ("NICK" ==  msg.getCmd()) {
                // msg.getParams().front();
        }
        return(0);
}

// verif if username already exists
// otherwise send error
//
