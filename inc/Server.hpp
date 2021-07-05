#pragma once

#include "main.hpp"
#include "Message.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

#define DATA_BUFFER 5000

#define MAX_CONNECTIONS 1024

class Server
{
private:
Server();
//Real Server Param
int const _portnum;
std::string const _password;
char _buf[DATA_BUFFER];
Message mess;
std::vector<int> _socket_fd;     // i->0 = Server fd
struct sockaddr_in _saddr;

//tools for Server
std::vector<Client> _v_clients;
std::map<int, std::string> _m_fdprefix; // from fd to prefix ()
std::map<std::string, Client> _m_prefixclient; // from prefix to definitive clients


public:
// HOW TO USE : chan["#channel"][10] -> accees au 10eme client du chan "#channel"
std::map<std::string, std::vector<Client*> > chan;
std::map<std::string, std::string> chan_flag; // see todo pour flags possible sur channel
std::map<std::string, std::string> chan_pass; // la on store les mdp

Server(int port, std::string pass);
Server(Server const & src);
virtual ~Server();
Server &  operator=( Server const & rhs );

std::string   ft_itoa(int nbr);
int create_tcp_server_socket(int port);
int launch(void);
void loop(void);
void on_connection(sockaddr_in new_addr, socklen_t addrlen, int new_fd);
void on_message(std::vector<int>::iterator it_fd, int* ret_val);
std::string executionner(char buf[5000], Message &message, int fd);
int send_privmsg(int fd, std::string str);
void send_err(int fd, int err, std::string msg);
Client find_CfromFd(int fd);

//getters & setters
std::vector<Client> getClients(void) const {
        return(this->_v_clients);
}
void setClients(std::vector<Client> src) {
        this->_v_clients = src;
}
int getPort(void) const {
        return(this->_portnum);
}
std::string getPass(void) const {
        return(this->_password);
}

//Functions

void clearClient(int fd);
bool   nick_check(std::string &nick, int fd);
int   nickcmd(Message msg, int fd);
int passcmd(Message & msg, int fd);
int usercmd(Message &msg, int fd);
int do_cmd(Message msg, int fd);
int joincmd(Message &msg, int fd);
bool  find_Cinchan(int fd, std::vector<Client*> vect);
};

// std::ostream &			operator<<( std::ostream & o, Server const & i );
