#include "../inc/Server.hpp"
// #include "../inc/main.hpp"

int Server::send_privmsg(int fd, std::string str)
{
        return (send(fd, str.c_str(), str.size(), 0));
}

bool Server::nick_check(std::string &nick, int fd){
        std::string s(ft_itoa(fd));
        std::map<std::string, Client>::iterator it = this->_m_prefixclient.begin();
        while(it != this->_m_prefixclient.end()) {
                std::cout << (it->second.getNick()) << '\n';
                if ((it->second.getNick() == nick) /* && (it->getReg() == true)*/) {
                        return false;
                }
                it++;
        }
        return true;
}

int Server::nickcmd(Message msg, int fd){
        std::string s(ft_itoa(fd));
        if (msg.getParams().size() == 0) {
                send_err(fd, ERR_NONICKNAMEGIVEN, " :No nickname given\n");
        }
        else if (msg.getParams().front() == _m_prefixclient[s].getNick()) {
                send_err(fd, ERR_NICKNAMEINUSE, " <" + msg.getParams().front() +"> :Nickname is already in use\n");
        }
        else if (nick_check(msg.getParams().front(), fd) == false) {
                send_err(fd, ERR_NICKCOLLISION, " <" + msg.getParams().front() + "> :Nickname collision\n");
        }
        else {
                send_privmsg(fd, "Nick has been registered -- <" + msg.getParams().front() + ">\n");
                _m_prefixclient[s].setNick(msg.getParams().front());
        }
        return(0);
}

Client Server::find_CfromFd(int fd){
        std::vector<Client>::iterator it = _v_clients.begin();
        for (; it != _v_clients.end(); it++) {
                if (it->getFd() == fd) {
                        return(*it);
                }
        }
        return(_v_clients.front());
}

int Server::passcmd(Message & msg, int fd) {
        std::string s(ft_itoa(fd));

        if (_m_prefixclient[s].getCorr() == false) {
                if (msg.getParams().size() == 0) {
                        send_err(fd, ERR_NEEDMOREPARAMS, " PASS :Not enough parameters\n");
                }
                else if (msg.getParams().front() == _password) {
                        _m_prefixclient.insert(std::pair<std::string, Client>(s, find_CfromFd(fd)));
                        _m_prefixclient[s].setCorr(true);
                        send_privmsg(fd, "You can now register with NICK then USER.\n");
                }
                else {
                        send_privmsg(fd, "Wrong Password\n");
                        std::cout << "bad pass" << '\n';
                }
        }
        else {
                send_err(fd, ERR_ALREADYREGISTERED, " :You may not reregister\n");
        }
        return(0);
}

int Server::usercmd(Message &msg, int fd) {
        std::string s(ft_itoa(fd));

        //if () user deja register -- >ERR_ALREADYREGISTERED // wrong parameters // pass
        if (msg.getParams().size() == 4) {
                if (_m_prefixclient[s].getReg() == false) {
                        if (_m_prefixclient[s].getReg()) {
                                std::cout << "erreur" << '\n';
                        }
                        // nb params
                        _m_prefixclient[s].setUser(msg.getParams().front());
                        _m_prefixclient[s].setReal(msg.getParams().back());
                        std::string prefix;


                        prefix = _m_prefixclient[s].getUser();
                        prefix += "!";
                        prefix += _m_prefixclient[s].getReal();
                        std::cout << _m_prefixclient[s].getReal() << '\n';
                        prefix += "@";
                        prefix += inet_ntoa(_v_clients.back().getInfo().sin_addr);
                        //std::string prefix(_m_prefixclient[s].username + "!" + _)
                        // tous les champs user ! real @ host
                        std::cout << prefix << '\n';
                        _m_fdprefix[fd] = prefix;
                        _m_prefixclient[prefix] = _m_prefixclient[s];
                        send_privmsg(fd, ":irc.example.net 001 " + _m_prefixclient[prefix].getNick() + " :Welcome to the Internet Relay Network " + _m_fdprefix[fd] + "\n");
                        _m_prefixclient[prefix].setReg(true);
                }
                else{
                        send_err(fd, ERR_ALREADYREGISTERED, " :You may not reregister\n");
                }
        }
        else {
                send_err(fd, ERR_NEEDMOREPARAMS, " <USER> :Not enough parameters\n");
        }
        return(0);
}

bool Server::find_Cinchan(int fd, std::vector<Client*> vect)
{
        std::vector<Client*>::iterator it = vect.begin();
        for(; it != vect.end(); it++) {
                if((**it).getNick() == _m_prefixclient[_m_fdprefix[fd]].getNick())
                        return(true);
        }
        return(false);
}

void Server::sendtoAll(std::vector<Client*> at, std::string &msg)
{
  std::cout << "sendto all\n";
}

void Server::privmsg(Message &msg, int fd)
{
        if (msg.getParams().size() == 2) {
          if (msg.getParams().front().find(',') != std::string::npos) {
                if (msg.getParams().front().find('#') != std::string::npos) {
                  if (chan.find(msg.getParams().front()) != chan.end()) {
                    sendtoAll(chan[msg.getParams().front()], msg.getParams().back());
                  }
                }
                else {
                  std::vector<Client>::iterator clients = _v_clients.begin();
                  for (; clients != _v_clients.end(); clients++) {
                    if (clients->getFd() == fd)
                      continue ;
                    else if (clients->getNick() == msg.getParams().front()) {
                      send_privmsg(clients->getFd(), msg.getParams().back() + "\n");
                    }
                  }
                }
          }
          else {

          }
        }
}

int Server::joincmd(Message &msg, int fd)
{
        //s'occuper de l'acces a plusieurs canal en meme temps
        std::list<std::string> chan_list = split_char(msg.getParams().front(), ",");
        std::list<std::string> pass_list = split_char(msg.getParams().back(), ",");
        std::list<std::string>::iterator it_chan = chan_list.begin();
        std::list<std::string>::iterator it_pass = pass_list.begin();

        while((it_chan != chan_list.end()) && (it_pass != pass_list.end()))
        {
                std::string chan_name = *it_chan;
                size_t      size = msg.getParams().size();
                if(size <= 2 && size >= 1) {
                        if (chan_name[0] == '#')
                        {
                                if (chan.find(chan_name) != chan.end())
                                {
                                        if (find_Cinchan(fd, chan.find(chan_name)->second) != true)
                                        {
                                                if ((chan_pass.find(msg.getParams().front()) != chan_pass.end()) || (chan_pass.find(msg.getParams().front())->second == msg.getParams().back()))
                                                {
                                                        Client *temp = &(_m_prefixclient[_m_fdprefix[fd]]);
                                                        chan.find(chan_name)->second.push_back(temp);
                                                        send_privmsg(fd, "added to chan " + chan_name + "\n");
                                                }
                                                else {
                                                        send_privmsg(fd, "please give correct chanels password\n");
                                                }
                                        }
                                        else{
                                                send_privmsg(fd, "already in said chan\n");
                                        }
                                        //sur invite ?
                                        //est ce que le gars est ban/kick ?
                                }
                                else{
                                        Client *temp = &(_m_prefixclient[_m_fdprefix[fd]]);
                                        std::vector<Client *> _v_cli_tmp;
                                        _v_cli_tmp.push_back(temp);
                                        chan[chan_name] = _v_cli_tmp;
                                        if (size == 2) { //creation d'un chan a mdp
                                                chan_flag[chan_name] = "+m";
                                                chan_pass[chan_name] = msg.getParams().back();
                                        }
                                        else
                                                chan_flag[chan_name] = "";
                                        send_privmsg(fd, "Created and added to chan " + chan_name + "\n");
                                }
                        }
                        else{
                                send_privmsg(fd, "chan must begin by #\n");
                        }
                }
                else{
                        send_privmsg(fd, "please give good amount of parameters\n");
                }
                it_chan++;
                it_pass++;
        }
        return(0);
}

void Server::send_err(int fd, int err, std::string msg) {
        std::string s(ft_itoa(err));
        send_privmsg(fd, s + msg);
}

int Server::do_cmd(Message msg, int fd){
        std::string s(ft_itoa(fd));
        if (msg.getCmd() == "PASS") {
                passcmd(msg, fd);
        }
        else if (_m_prefixclient[s].getCorr() == true) {
                std::cout << "good pass, access" << '\n';
                if (msg.getCmd() == "NICK") {
                        nickcmd(msg, fd);
                }
                else if (msg.getCmd() == "USER") {
                        usercmd(msg, fd);
                }
                else if (_m_prefixclient[_m_fdprefix[fd]].getReg() == true)
                {
                        std::cout << "entering matrix" << '\n';
                        if (msg.getCmd() == "BIGLOL") {
                                std::cout << _m_prefixclient[_m_fdprefix[fd]].getNick() << '\n';
                                std::cout << _m_prefixclient[_m_fdprefix[fd]].getUser() << '\n';
                                std::cout << _m_prefixclient[_m_fdprefix[fd]].getReal() << '\n';
                                std::cout << _m_prefixclient[_m_fdprefix[fd]].getPass() << '\n';
                        }
                        else if (msg.getCmd() == "JOIN") {
                                joincmd(msg, fd);
                        }

                }
                else
                        send_privmsg(fd, "Bad cmd\n");
        }
        else {
                send_privmsg(fd, "Bad cmd\n");
                std::cout << "bad cmd" << '\n';
                //join privmsg(#chan || nick) OPER-> KILL LUSER(info serv) HELP  - NOTICE
        }
        return (0);
}

// verif if username already exists
// otherwise send error
//
