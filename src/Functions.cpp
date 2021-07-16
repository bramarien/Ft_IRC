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
        else if (msg.getParams().size() > 1) {
                send_err(fd, ERR_NEEDMOREPARAMS, msg.getCmd() + " :Syntax error\n");
        }
        else if (msg.getParams().front() == _m_prefixclient[_m_fdprefix[fd]].getNick()) {
                send_err(fd, ERR_NICKNAMEINUSE, " <" + msg.getParams().front() +"> :Nickname is already in use\n");
        }
        else if (nick_check(msg.getParams().front(), fd) == false) {
                send_err(fd, ERR_NICKCOLLISION, " <" + msg.getParams().front() + "> :Nickname collision\n");
        }
        else {
                send_privmsg(fd, "Nick has been registered -- <" + msg.getParams().front() + ">\n");
                _m_prefixclient[_m_fdprefix[fd]].setNick(msg.getParams().front());
        }
        if (_m_prefixclient[_m_fdprefix[fd]].getNickstatus() && _m_prefixclient[_m_fdprefix[fd]].getUserstatus()) {
                _m_prefixclient[_m_fdprefix[fd]].setReg(true);
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
                        _m_prefixclient[prefix].setUserstatus(true);
                        _m_prefixclient[prefix].setFd(fd);
                        if (_m_prefixclient[prefix].getNickstatus() && _m_prefixclient[prefix].getUserstatus()) {
                                _m_prefixclient[prefix].setReg(true);
                        }
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

void Server::sendtoAll(std::vector<Client*> at, std::string msg)
{
        std::vector<Client*>::iterator client = at.begin();
        for (; client != at.end(); client++) {
                Client to_send = *(*client);
                send_privmsg(to_send.getFd(), msg + "\n");
        }

}

std::string Server::getmsg(Message &msg, int fd)
{
        std::list<std::string> params = msg.getParams();
        std::string msg_tosend = "From " + _m_prefixclient[_m_fdprefix[fd]].getNick() + " : ";
        std::list<std::string>::iterator it = params.begin();
        it++;
        if (params.size() == 2) {
                msg_tosend.append(*it);
        }
        else {
                send_err(fd, ERR_NEEDMOREPARAMS, "Syntax Error\n"); // changer !
        }
        return (msg_tosend);
}

void Server::privmsg(Message &msg, int fd)
{
        if (msg.getParams().size() >= 2) {
                std::cout << "privmsg" << std::endl;
                std::string msg_tosend = getmsg(msg, fd);
                std::list<std::string> receiver_list = split_every_char(msg.getParams().front(), ',');
                std::list<std::string>::iterator it_recv = receiver_list.begin();
                while(it_recv != receiver_list.end())
                {
                        std::cout << "it_recv -> " << *it_recv << std::endl;
                        if ((*it_recv)[0] == '#') {
                                if (chan.find(*it_recv) != chan.end()) {
                                        sendtoAll(chan[*it_recv], msg_tosend);
                                }
                                else {
                                        send_err(fd, ERR_CANNOTSENDTOCHAN, "<" + *it_recv + "> :Cannot send to channel\n");
                                }
                        }
                        else {
                                std::cout << "searching for clients" << '\n';
                                std::map<std::string, Client>::iterator it_clients = _m_prefixclient.begin();
                                for (; it_clients != _m_prefixclient.end(); it_clients++) {
                                        if (it_clients->second.getFd() == fd)
                                                continue;
                                        else if (it_clients->second.getNick() == *it_recv) {
                                                send_privmsg(it_clients->second.getFd(), msg_tosend + "\n");
                                        }

                                }
                        }
                        it_recv++;
                }
        }
        else {
                send_err(fd, ERR_NOTEXTTOSEND, " :No text to send     or     ");
                send_err(fd, ERR_NOSUCHNICK, " <" + msg.getParams().front() + "> :No such nick/channel\n");
        }
}

int Server::joincmd(Message &msg, int fd)
{
        //commencer par verifier nombre de parametres
        std::list<std::string> chan_list = split_every_char(msg.getParams().front(), ',');
        std::list<std::string>::iterator it_chan = chan_list.begin();
        std::list<std::string> pass_list = split_every_char(msg.getParams().back(), ',');
        std::list<std::string>::iterator it_pass = pass_list.begin();
        //send message to all users once someone is logged in channel
        while((it_chan != chan_list.end()) && (it_pass != pass_list.end()))
        {
                std::string chan_name = *it_chan;
                size_t size = msg.getParams().size();
                if(size <= 2 && size >= 1) {
                        if (chan_name[0] == '#')
                        {
                                if (chan.find(chan_name) != chan.end())
                                {
                                        if (find_Cinchan(fd, chan.find(chan_name)->second) != true)
                                        {
                                                if ((chan_pass.find(msg.getParams().front()) == chan_pass.end()) || (chan_pass.find(msg.getParams().front())->second == *it_pass))
                                                {
                                                        Client *temp = &(_m_prefixclient[_m_fdprefix[fd]]);
                                                        chan.find(chan_name)->second.push_back(temp);
                                                        send_privmsg(fd, "added to chan " + chan_name + "\n"); //send to all
                                                }
                                                else {
                                                        send_err(fd, ERR_BADCHANNELKEY, ":Cannot join channel (+k)\n");
                                                }
                                        }
                                        //sur invite ?
                                        //est ce que le gars est ban/kick ?
                                }
                                else{
                                        Client *temp = &(_m_prefixclient[_m_fdprefix[fd]]);
                                        std::vector<Client *> _v_cli_tmp;
                                        _v_cli_tmp.push_back(temp);
                                        chan[chan_name] = _v_cli_tmp;
                                        if ((msg.getParams().size() == 2) && (*it_pass != "")) { //creation d'un chan a mdp
                                                chan_flag[chan_name] = "+m";
                                                chan_pass[chan_name] = msg.getParams().back();
                                        }
                                        sendtoAll(chan[chan_name], (static_cast<std::string>(SERV_NAME) + " " + static_cast<std::string>(RPL_NAMREPLY) +  " lol")); //send to all
                                }
                        }
                        else{
                                send_err(fd, ERR_NOSUCHCHANNEL, ":No such channel\n");
                        }
                }
                else{
                        send_err(fd, ERR_NEEDMOREPARAMS, ":Not enough parameters\n");
                }
                it_chan++;
                it_pass++;
        }
        return(0);
}

void Server::send_err(int fd, std::string err, std::string msg) {
        send_privmsg(fd, ":" + static_cast<std::string>(SERV_NAME) + " " + err + _m_prefixclient[_m_fdprefix[fd]].getNick() + msg);
}

int Server::do_cmd(Message msg, int fd){
        std::string s(ft_itoa(fd));
        if (msg.getCmd() == "PASS") {
                passcmd(msg, fd);
        }
        else if (_m_prefixclient[s].getCorr() == true) {
                if (msg.getCmd() == "NICK") {
                        nickcmd(msg, fd);
                }
                else if (msg.getCmd() == "USER") {
                        usercmd(msg, fd);
                }
                else if (_m_prefixclient[_m_fdprefix[fd]].getReg() == true)
                {
                        if (msg.getCmd() == "JOIN") {
                                joincmd(msg, fd);
                        }
                        else if (msg.getCmd() == "PRIVMSG" || msg.getCmd() == "NOTICE") {
                                privmsg(msg, fd);
                        }
                }
                else
                        send_err(fd, ERR_UNKNOWNCOMMAND, msg.getCmd() + " :Unknown command");
        }
        else {
                send_err(fd, ERR_NOTREGISTERED, "* :Connection not registered\n");
                //join privmsg(#chan || nick) OPER-> KILL
        }
        return (0);
}

// verif if username already exists
// otherwise send error
//
