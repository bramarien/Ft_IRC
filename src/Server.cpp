#include "../inc/Server.hpp"


Server::Server() : _portnum(6667), _password("qwe"){
}

Server::Server(int port, std::string pass) : _portnum(port), _password(pass)
{
        _socket_fd.clear();
}
Server::Server( Server const & src ) : _v_clients(src.getClients()), _portnum(src.getPort()), _password(src.getPass()){
}

Server::~Server(){
}

Server &    Server::operator=(Server const & rhs ){
        this->_v_clients = rhs._v_clients;
        return(*this);
}

void Server::on_connection(sockaddr_in new_addr, socklen_t addrlen, int new_fd) {
        Client* new_user = new Client(new_addr, addrlen, new_fd);
        std::cout << "Accepted a new connection with fd " << new_fd << std::endl;
        this->_socket_fd.push_back(new_fd);
        this->_v_clients.push_back(*new_user);
        std::cout << inet_ntoa(_v_clients.back().getInfo().sin_addr) << " -> Ip adress of new_user" << std::endl;
}

void Server::loop(void)
{
        fd_set read_fd_set;
        int ret_val;
        int new_fd;

        struct sockaddr_in new_addr;
        socklen_t addrlen;

        while (1)
        {
                FD_ZERO(&read_fd_set); /* Set the fd_set before passing it to the select call */
                for (std::vector<int>::iterator it = _socket_fd.begin(); it != _socket_fd.end(); it++)
                        FD_SET(*it, &read_fd_set);

                /* Invoke select() and then wait! */
                std::cout << "Poll with select to listen for entry" << std::endl;
                ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

                /* select() woke up. Identify the fd that has events */
                if (ret_val >= 0)
                {
                        std::cout << "Return of select : " << ret_val << std::endl;
                        /* Check if the fd with event is the server fd */
                        if (FD_ISSET(_socket_fd[0], &read_fd_set))
                        {
                                std::cout << "The server find a new connection" << std::endl;
                                new_fd = accept(_socket_fd[0], (struct sockaddr*)&new_addr, &addrlen);
                                if (new_fd >= 0 && _socket_fd.size() <= 1024)
                                {
                                        on_connection(new_addr, addrlen, new_fd);
                                }
                                else
                                {
                                        std::cerr << "accept failed " << strerror(errno) << std::endl;
                                        //Throw Here.
                                }
                                ret_val--;
                                if (!ret_val) continue;
                        }

                        /* Check if the fd with event is a non-server fd */
                        for (std::vector<int>::iterator it_fd = _socket_fd.begin() + 1; it_fd != _socket_fd.end(); it_fd++)
                        {
                                if (FD_ISSET(*it_fd, &read_fd_set))
                                {
                                        // on_message(it_fd, &ret_val);
                                        /* read incoming data */
                                        std::cout << "Listening from fd-> " << *it_fd << std::endl;
                                        ret_val = recv(*it_fd, _buf, DATA_BUFFER, 0);
                                        _buf[ret_val - 1] = 0;
                                        if (ret_val == 0)
                                        {
                                                std::cout << "Closing connection for fd-> " << *it_fd << std::endl;
                                                close(*it_fd);
                                                _socket_fd.erase(it_fd); /* Connection is now closed */
                                                break;
                                        }
                                        if (ret_val > 0)
                                        {
                                                std::cout << _buf << std::endl;
                                                std::string response = executionner(_buf, mess, *it_fd);
                                                // ret_val = send(all_connections[i],"KOUKOU", sizeof("KOUKOU"), 0);
                                        }
                                        if (ret_val == -1)
                                        {
                                                std::cerr << "recv() failed for fd-> " << *it_fd << " : " << strerror(errno) << std::endl;
                                                break;
                                        }
                                        memset(_buf, '\0', DATA_BUFFER);
                                }
                                ret_val--;
                                if (!ret_val) continue;
                        } /* for-loop */
                } /* (ret_val >= 0) */
        } /* while(1) */
}

int Server::launch(void)
{

        Message mess_test;
        struct sockaddr_in new_addr;
        int server_fd, new_fd, ret_val, i;
        socklen_t addrlen;

        std::string response = "";

        /* Get the socket server fd */
        server_fd = create_tcp_server_socket(this->_portnum);
        if (server_fd == -1)
        {
                std::cerr << "Failed to create a server" << std::endl;
                return -1;
        }
        _socket_fd.push_back(server_fd);

        loop();

        /* Last step: Close all the sockets */
        for (std::vector<int>::iterator it_fd = _socket_fd.begin(); it_fd != _socket_fd.end(); it_fd++)
                close(*it_fd);
        _socket_fd.clear();

        return 0;
}


int Server::create_tcp_server_socket(int port)
{
        int fd, ret_val;

        /* Step1: create a TCP socket */
        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        std::cout << "server fd is " << fd <<  ":D" << std::endl;
        if (fd == -1) {
                fprintf(stderr, "socket failed [%s]\n", strerror(errno));
                return -1;
        }
        printf("Created a socket with fd: %d\n", fd);

        /* Initialize the socket address structure */
        _saddr.sin_family = AF_INET;
        _saddr.sin_port = htons(port);
        _saddr.sin_addr.s_addr = INADDR_ANY;

        /* Step2: bind the socket to port 7000 on the local host */
        ret_val = bind(fd, (struct sockaddr *)&_saddr, sizeof(struct sockaddr_in));
        if (ret_val != 0)
        {
                fprintf(stderr, "bind failed [%s]\n", strerror(errno));
                close(fd);
                return -1;
        }

        /* Step3: listen for incoming connections */
        ret_val = listen(fd, 5);
        if (ret_val != 0)
        {
                fprintf(stderr, "listen failed [%s]\n", strerror(errno));
                close(fd);
                return -1;
        }
        return fd;
}

std::string Server::executionner(char buf[5000], Message &message, int fd)
{
        message.parsing_cmd(buf);
        message.display();
        do_cmd(message, fd);
        return("");
}

std::string   Server::ft_itoa(int nbr){
        std::string s;
        std::stringstream out;
        out << nbr;
        s = out.str();
        return(s);
}
