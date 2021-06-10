
#include "../inc/Client.hpp"

Client::Client() : nickname(), username(), realname(), password(){
}
Client::Client(std::string nick, std::string user, std::string real, std::string pass) : nickname(nick), username(user), realname(real), password(pass){
}

Client::Client( Client const & src ){
        *this = src;
}

Client::~Client(){
}

Client &    Client::operator=(Client const & rhs ){
        this->nickname = rhs.nickname;
        this->username = rhs.username;
        this->realname = rhs.realname;
        this->password = rhs.password;
        return(*this);
}
