#include "../inc/nick.hpp"

bool  nick_check(std::string nick){
        return(false);
}

int   nick_fct(Message msg, Server serv){
        if ("NICK" ==  msg.getCmd()) {
                nick_check(msg.getParams().front());
        }
        return(0);
}

// verif if username already exists
// otherwise send error
// verif nickname format -> taille / character
