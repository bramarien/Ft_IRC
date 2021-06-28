#include "../inc/Functions.hpp"

bool  nick_check(std::string nick, Server &serv){
        return(false);
}

int   nick_fct(Message msg, Server &serv){
        if ("NICK" ==  msg.getCmd()) {
                if (nick_check(msg.getParams().front(), serv) == false) {
                        std::cout << "Already one" << '\n';
                }
        }
        return(0);
}

// verif if username already exists
// otherwise send error
//
