#pragma once

# include <iostream>
#include <vector>
#include <map>
# include "Client.hpp"
#include "main.hpp"
#include "Server.hpp"
#include "Message.hpp"

#define DATA_BUFFER 5000

#define MAX_CONNECTIONS 1024

class Client;

class Server
{
  private:
    Server();
    //Real Server Param
    int const _portnum;
    std::string const _password;

    std::vector<int> _socket_fd; // i->0 = Server fd
    struct sockaddr_in _saddr;

    //tools for Server
    std::vector<Client> _clients;

  public:
    // HOW TO USE : chan["#channel"][10] -> accees au 10eme client du chan "#channel"
  std::map<std::string, std::vector<Client*> > chan;
  //std::map<std::map<std::string, std::vector<Client*>>, std::string> chan_flag;

  Server(int port, std::string pass);
  Server(Server const & src);
  virtual ~Server();
  Server &  operator=( Server const & rhs );

  int launch(void);
  void loop(void);
  int create_tcp_server_socket(int port);
  std::string executionner(char buf[5000], Message &message);

  //getters & setters
  std::vector<Client> getClients(void) const { return(this->_clients); }
  void setClients(std::vector<Client> src) { this->_clients = src; }
  int getPort(void) const { return(this->_portnum); }
  std::string getPass(void) const { return(this->_password); }


};

// std::ostream &			operator<<( std::ostream & o, Server const & i );
