#pragma once

# include <iostream>
#include <vector>
# include "Client.hpp"
#include "main.hpp"
#include "Server.hpp"
#include "Message.hpp"

#define DATA_BUFFER 5000

#define MAX_CONNECTIONS 1024

class Client;

class Server
{

public:

Server(int port, std::string pass);
Server(Server const & src);
virtual ~Server();

Server &  operator=( Server const & rhs );

int launch(void);
int create_tcp_server_socket(int port);
std::string executionner(char buf[5000], Message &message);
//getters n setters

std::vector<Client> getClients(void) const;
void setClients(std::vector<Client> src);
int getPort(void) const;
std::string getPass(void) const;

private:

Server();
int const _portnum;
std::string const _password;
std::vector<Client> _clients;

};

// std::ostream &			operator<<( std::ostream & o, Server const & i );
