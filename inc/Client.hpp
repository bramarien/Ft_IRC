#pragma once

#include <iostream>
#include <iostream>
#include <vector>
#include <map>
#include "main.hpp"
// #include "Server.hpp"
// #include "Message.hpp"

class Client
{

public:

Client(sockaddr_in inf, socklen_t len, int fd);
Client( Client const & src );
virtual ~Client();

Client &  operator=( Client const & rhs );

sockaddr_in getInfo(void) { return(this->info); }
std::string getNick(void) { return(this->nickname); }
std::string getPass(void) { return(this->password); }
std::string getReal(void) { return(this->realname); }
std::string getUser(void) { return(this->username); }

private:
Client();

sockaddr_in info;
socklen_t len;
int fd;
std::string nickname;
std::string username;
std::string realname;
std::string password;
bool  is_registered;
bool  corr_pass;

};

// std::ostream &			operator<<( std::ostream & o, Client const & i );
