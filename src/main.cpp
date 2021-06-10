#include "../inc/main.hpp"
#include "../inc/Server.hpp"
#include "../inc/Client.hpp"
#include "../inc/Message.hpp"


#define DATA_BUFFER 5000

 #define MAX_CONNECTIONS 1024

void disp_list(std::list<std::string> v) {
        std::list<std::string>::iterator it = v.begin();
        for(; it != v.end(); it++)
        {
                std::cout << *it << '\n';
        }

}

std::string split_first_char(std::string &str, char separator) {
        size_t pos = 0;
        std::list<std::string> list_temp;
        std::string temp;

        if ((pos = str.find(separator)) != std::string::npos)
        {
                temp = str.substr(pos, str.length());
                str = str.erase(pos, str.length());
        }
        else {
                return("");
        }

        return(temp);
}

std::list<std::string> split_char(std::string str, char separator) {
        size_t pos = 0;
        std::list<std::string> list_temp;

        while ((pos = str.find(separator)) != std::string::npos)
        {
                std::string temp = str.substr(0, pos);
                if (temp.length() != 0) {
                        list_temp.push_back(temp);
                }
                str = str.erase(0, pos + 1);
        }
        if (pos == std::string::npos) {
                std::string temp = str.substr(0, pos);
                if (temp.length() != 0) {
                        list_temp.push_back(temp);
                }
        }
        return(list_temp);
}

void parsing_cmd(char buf[5000], Message &message) {
        std::string buf_str(buf);
        std::list<std::string> list_temp;

        std::string rhs = split_first_char(buf_str, ':');
        list_temp = split_char(buf_str, ' ');

        if (rhs.length() != 0)
                list_temp.push_back(rhs);

        std::list<std::string>::iterator it = list_temp.begin();
        message.setCmd(*it);
        list_temp.erase(it);
        message.setParams(list_temp);

}

std::string executionner(char buf[5000], Message &message)
{
        parsing_cmd(buf, message);
        disp_list(message.getParams());
        return("");
}

int create_tcp_server_socket(int port) {
        struct sockaddr_in saddr;
        int fd, ret_val;

        /* Step1: create a TCP socket */
        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
        if (ret_val != 0) {
                fprintf(stderr, "bind failed [%s]\n", strerror(errno));
                close(fd);
                return -1;
        }

        /* Step3: listen for incoming connections */
        ret_val = listen(fd, 5);
        if (ret_val != 0) {
                fprintf(stderr, "listen failed [%s]\n", strerror(errno));
                close(fd);
                return -1;
        }
        return fd;
}

int main (int ac, char *av[]) {

        Server serv_test;
        Message mess_test;
        fd_set read_fd_set;
        char buf[DATA_BUFFER];
        struct sockaddr_in new_addr;
        int server_fd, new_fd, ret_val, i;
        socklen_t addrlen;
        int all_connections[MAX_CONNECTIONS];

        if (ac != 3)
        {
          std::cout << "Please provide port and password as follows :\n\t$ ./ircserv <port> <password>" << '\n';
          exit(1);
        }

        std::string response = "";

        /* Get the socket server fd */
        server_fd = create_tcp_server_socket(atoi(av[1]));
        if (server_fd == -1) {
                fprintf(stderr, "Failed to create a server\n");
                return -1;
        }

        /* Initialize all_connections and set the first entry to server fd */
        for (i=0; i < MAX_CONNECTIONS; i++) {
                all_connections[i] = -1;
        }
        all_connections[0] = server_fd;

        while (1) {
                FD_ZERO(&read_fd_set);  /* Set the fd_set before passing it to the select call */
                for (i=0; i < MAX_CONNECTIONS; i++) {
                        if (all_connections[i] >= 0) {
                                FD_SET(all_connections[i], &read_fd_set);
                        }
                }

                /* Invoke select() and then wait! */
                printf("\nUsing select() to listen for incoming events\n");
                ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

                /* select() woke up. Identify the fd that has events */
                if (ret_val >= 0 ) {
                        printf("Select returned with %d\n", ret_val);
                        /* Check if the fd with event is the server fd */
                        if (FD_ISSET(server_fd, &read_fd_set)) {
                                /* accept the new connection */
                                printf("Returned fd is %d (server's fd)\n", server_fd);
                                new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                                if (new_fd >= 0) {
                                        printf("Accepted a new connection with fd: %d\n", new_fd);
                                        for (i=0; i < MAX_CONNECTIONS; i++) {
                                                if (all_connections[i] < 0) {
                                                        all_connections[i] = new_fd;
                                                        break;
                                                }
                                        }
                                } else {
                                        fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                                }
                                ret_val--;
                                if (!ret_val) continue;
                        }

                        /* Check if the fd with event is a non-server fd */
                        for (i=1; i < MAX_CONNECTIONS; i++) {
                                if ((all_connections[i] > 0) &&
                                    (FD_ISSET(all_connections[i], &read_fd_set))) {
                                        /* read incoming data */
                                        printf("Returned fd is %d [index, i: %d]\n", all_connections[i], i);
                                        ret_val = recv(all_connections[i], buf, DATA_BUFFER, 0);
                                        response = executionner(buf, mess_test);
                                        // ret_val = send(all_connections[i],"KOUKOU", sizeof("KOUKOU"), 0);
                                        if (ret_val == 0) {
                                                printf("Closing connection for fd:%d\n", all_connections[i]);
                                                close(all_connections[i]);
                                                all_connections[i] = -1; /* Connection is now closed */
                                        }
                                        if (ret_val > 0) {
                                                printf("%s\n", buf);
                                        }
                                        if (ret_val == -1) {
                                                printf("recv() failed for fd: %d [%s]\n",
                                                       all_connections[i], strerror(errno));
                                                break;
                                        }
                                        memset(buf, '\0', DATA_BUFFER);
                                }
                                ret_val--;
                                if (!ret_val) continue;
                        } /* for-loop */
                } /* (ret_val >= 0) */
        } /* while(1) */

        /* Last step: Close all the sockets */
        for (i=0; i < MAX_CONNECTIONS; i++) {
                if (all_connections[i] > 0) {
                        close(all_connections[i]);
                }
        }
        return 0;
}
