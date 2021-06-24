
#include "../inc/Server.hpp"

Server::Server() : _portnum(6667), _password("qwe"){
}

Server::Server(int port, std::string pass) : _portnum(port), _password(pass)
{
  socket_fd.clear();
}
Server::Server( Server const & src ) : _clients(src.getClients()), _portnum(src.getPort()), _password(src.getPass()){
}

Server::~Server(){
}

Server &    Server::operator=(Server const & rhs ){
        this->_clients = rhs._clients;
        return(*this);
}

void  Server::loop(void)
{
  char buf[DATA_BUFFER];
  fd_set read_fd_set;
  int ret_val;
  int new_fd;

  struct sockaddr_in new_addr;
  socklen_t addrlen;

  while (1)
  {
    FD_ZERO(&read_fd_set); /* Set the fd_set before passing it to the select call */
    for (std::vector<int>::iterator it = socket_fd.begin(); it != socket_fd.end(); it++)
        FD_SET(*it, &read_fd_set);

    /* Invoke select() and then wait! */
    std::cout << "Poll with select to listen for entry" << std::endl;
    ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

    /* select() woke up. Identify the fd that has events */
    if (ret_val >= 0)
    {
      std::cout << "Return of select : " << ret_val << std::endl;
      /* Check if the fd with event is the server fd */
      if (FD_ISSET(socket_fd[0], &read_fd_set))
      {
        std::cout << "The server find a new connection" << std::endl;
        new_fd = accept(socket_fd[0], (struct sockaddr*)&new_addr, &addrlen);
        if (new_fd >= 0 && socket_fd.size() <= 1024)
        {
          Client * new_user = new Client(new_addr, addrlen, new_fd);
          std::cout << "Accepted a new connection with fd " << new_fd << std::endl;
          socket_fd.push_back(new_fd);
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
      for (std::vector<int>::iterator it_fd = socket_fd.begin() + 1; it_fd != socket_fd.end(); it_fd++)
      {
        if (FD_ISSET(*it_fd, &read_fd_set))
        {
          /* read incoming data */
          std::cout << "Listening from fd-> " << *it_fd << std::endl;
          ret_val = recv(*it_fd, buf, DATA_BUFFER, 0);
          // response = executionner(buf, mess_test);
          // ret_val = send(all_connections[i],"KOUKOU", sizeof("KOUKOU"), 0);
          if (ret_val == 0)
          {
            std::cout << "Closing connection for fd-> " << *it_fd << std::endl;
            close(*it_fd);
            socket_fd.erase(it_fd); /* Connection is now closed */
            break;
          }
          if (ret_val > 0)
          {
            std::cout << buf << std::endl;
          }
          if (ret_val == -1)
          {
            std::cerr << "recv() failed for fd-> " << *it_fd << " : " << strerror(errno) << std::endl;
            break;
          }
          memset(buf, '\0', DATA_BUFFER);
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
  socket_fd.push_back(server_fd);

  loop();

  /* Last step: Close all the sockets */
  for (std::vector<int>::iterator it_fd = socket_fd.begin(); it_fd != socket_fd.end(); it_fd++)
      close(*it_fd);
  socket_fd.clear();

  return 0;
}


int Server::create_tcp_server_socket(int port)
{
        struct sockaddr_in saddr;
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
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = INADDR_ANY;

        /* Step2: bind the socket to port 7000 on the local host */
        ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
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

std::string Server::executionner(char buf[5000], Message &message)
{
  message.parsing_cmd(buf);
  message.disp_mess(message);
  return("");
}
