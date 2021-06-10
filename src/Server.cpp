
#include "../inc/Server.hpp"

Server::Server() : only_cli("test", "test", "test", "test"){
}

Server::Server( Server const & src ) : only_cli(src.only_cli){
        *this = src;
}

Server::~Server(){
}

Server &    Server::operator=(Server const & rhs ){
        this->only_cli = rhs.only_cli;
        return(*this);
}
