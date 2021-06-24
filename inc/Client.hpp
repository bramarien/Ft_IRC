#pragma once

#include <iostream>
#include <iostream>
#include <vector>
#include <map>
#include "Client.hpp"
#include "main.hpp"
#include "Server.hpp"
#include "Message.hpp"

class Client
{

public:

Client(sockaddr_in inf, socklen_t len, int fd);
Client( Client const & src );
virtual ~Client();

Client &  operator=( Client const & rhs );


private:
Client();

sockaddr_in info;
socklen_t len;
int fd;
std::string nickname;
std::string username;
std::string realname;
std::string password;

};

// std::ostream &			operator<<( std::ostream & o, Client const & i );
