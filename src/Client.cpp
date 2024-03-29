
#include "../inc/Client.hpp"
// #include "../inc/main.hpp"


Client::Client() : info(), len(), fd(), nickname(), username(), realname(), password(){

}

Client::Client(sockaddr_in inf, socklen_t len, int fd) : info(inf), len(len), fd(fd), nickname(), username(), realname(), password(){
  is_registered = false;
}

Client::Client( Client const & src )
{
        *this = src;
}

Client::~Client(){
}

Client &    Client::operator=(Client const & rhs ){
        this->info = rhs.info;
        this->len = rhs.len;
        this->fd = rhs.fd;
        this->nickname = rhs.nickname;
        this->username = rhs.username;
        this->realname = rhs.realname;
        this->password = rhs.password;
        return(*this);
}
