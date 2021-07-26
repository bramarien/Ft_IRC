#include "../inc/Server.hpp"
// #include "../inc/main.hpp"

int Server::squitcmd(Message &msg, int fd) {
        exit(0);
}

int Server::namecmd(Message &msg, int fd) {
        if (msg.getParams().size() == 1) {
                std::list<std::string> receiver_list = split_every_char(msg.getParams().front(), ',');
                std::list<std::string>::iterator it_recv = receiver_list.begin();
                std::map<std::string, std::vector<Client*> >::iterator it_chan = chan.begin();
                std::string list_tosend = "";
                for (; it_recv != receiver_list.end(); it_recv++) {
                        it_chan = chan.begin();
                        for (; it_chan != chan.end(); it_chan++) {
                                if (it_chan->first == *it_recv) {
                                        list_tosend.append( "<" + it_chan->first + "> :");
                                        std::vector<Client*>::iterator it_cli = it_chan->second.begin();
                                        for (; it_cli != it_chan->second.end(); it_cli++) {
                                                if ((*it_cli)->getNick() == chan_oper[it_chan->first]) {
                                                        list_tosend.append("@<" + (*it_cli)->getNick() + "> ");
                                                }
                                                else {
                                                        list_tosend.append("<" + (*it_cli)->getNick() + "> ");
                                                }
                                        }
                                        list_tosend.append("\n");
                                }
                        }
                }
                send_privmsg(fd, list_tosend);
        }
        return (0);
}

int Server::listcmd(Message &msg, int fd) {
        if (msg.getParams().size() == 0) {
                std::list<std::string> receiver_list = split_every_char(msg.getParams().front(), ',');
                std::list<std::string>::iterator it_recv = receiver_list.begin();
                std::map<std::string, std::vector<Client*> >::iterator it_chan = chan.begin();
                std::string list_tosend = "";
                if (chan.size() >= 1) {
                        for (; it_chan != chan.end(); it_chan++) {
                                list_tosend.append(it_chan->first + ",");
                        }
                }
                send_privmsg(fd, "Channel :\n" + list_tosend + "\n" + ":End of /LIST\n");
        }
        return (0);
}

int Server::kickcmd(Message &msg, int fd) {
        if (msg.getParams().size() == 3) {
                std::list<std::string> args = msg.getParams();
                std::list<std::string>::iterator it_args;
                it_args = args.begin();
                std::map<std::string, std::vector<Client*> >::iterator it_chan = chan.begin();
                for (; it_chan != chan.end(); it_chan++) {
                        if (it_chan->first == msg.getParams().front())
                                break;
                }
                if (it_chan != chan.end()) {
                        if (_m_prefixclient[_m_fdprefix[fd]].getNick() == chan_oper[it_chan->first]) {
                                it_args++;
                                std::vector<Client*>::iterator it_cli = it_chan->second.begin();
                                for (; it_cli != it_chan->second.end(); it_cli++) {
                                        if ((**it_cli).getNick() == *it_args) {
                                                std::cout << "USER FOUND Let's kick him -->" << (**it_cli).getNick() << std::endl;
                                                break;
                                        }
                                }
                                if (it_cli != it_chan->second.end()) {
                                        it_chan->second.erase(it_cli);
                                }
                                else {
                                        send_err(fd, ERR_NOSUCHNICK, " <" + *it_args + "> :No such nick in the channel " + it_chan->first + "\n");
                                }
                        }
                        else {
                                send_err(fd, ERR_NOPRIVILEGES," :Permission Denied- You're not an operator of " + it_chan->first + " channel \n");
                        }
                }
                else {
                        send_err(fd, ERR_NOSUCHCHANNEL, "<" + msg.getParams().front() + "> :No such channel\n");
                }
        }
        else {
                send_err(fd, ERR_NEEDMOREPARAMS, " <" + msg.getCmd() + "> :Need 3 paramaters\n");
        }
        return (0);
}

int Server::killcmd(Message &msg, int fd) {
        if (_m_prefixclient[_m_fdprefix[fd]].getOp() == true) {
                if (msg.getParams().size() != 2) {
                        if (msg.getParams().size() > 2)
                                send_err(fd, ERR_NEEDMOREPARAMS, " <" + msg.getCmd() + "> :Need 2 paramaters\n");
                        else
                                send_err(fd, ERR_NEEDMOREPARAMS, " <" + msg.getCmd() + "> :Not enough parameters, need 2 paramaters\n");
                }
                else if (nick_check(msg.getParams().front(), fd) == true) {
                        send_err(fd, ERR_NOSUCHNICK, " <" + msg.getParams().front() + "> :No such nick\n");
                }
                else {
                        std::cout << "searching for clients" << '\n';
                        std::map<std::string, Client>::iterator it_clients = _m_prefixclient.begin();
                        for (; it_clients != _m_prefixclient.end(); it_clients++) {
                                int check = it_clients->second.getFd();
                                if (it_clients->second.getNick() == msg.getParams().front() && it_clients->second.getFd() != 0) {
                                        std::cout << "Closing connection for " << it_clients->second.getNick() << std::endl;
                                        send_privmsg(it_clients->second.getFd(), "you got killed : " + msg.getParams().back() + "\n");
                                        remove_Cinchans(it_clients->second.getFd());
                                        _m_fdprefix.erase(it_clients->second.getFd());
                                        clearClient(it_clients->second.getFd());
                                        close(it_clients->second.getFd());
                                        FD_CLR(it_clients->second.getFd(), &read_fd_set);
                                        for (std::vector<int>::iterator it = _socket_fd.begin(); it != _socket_fd.end(); it++) {
                                                if (*it == it_clients->second.getFd()) {
                                                        _socket_fd.erase(it);
                                                        break;
                                                }
                                        }
                                        std::cout << "User : " << it_clients->second.getNick() << " has been KILL" << std::endl;
                                        _m_prefixclient.erase(it_clients);
                                        break;
                                }
                        }
                }
        }
        else {
                send_err(fd, ERR_NOPRIVILEGES," :Permission Denied- You're not an IRC operator\n");
        }
        return (0);
}

int Server::opercmd(Message &msg, int fd) {
        if (msg.getParams().size() != 2) {
                if (msg.getParams().size() > 2)
                        send_err(fd, ERR_NEEDMOREPARAMS, " <" + msg.getCmd() + "> :Need 2 paramaters\n");
                else
                        send_err(fd, ERR_NEEDMOREPARAMS, " <" + msg.getCmd() + "> :Not enough parameters, need 2 paramaters\n");
        }
        else if (nick_check(msg.getParams().front(), fd) == true) {
                send_err(fd, ERR_NOSUCHNICK, " <" + msg.getParams().front() + "> :No such nick\n");
        }
        else if (msg.getParams().back() == OPERATOR_PW) {
                std::cout << "searching for clients" << '\n';
                std::map<std::string, Client>::iterator it_clients = _m_prefixclient.begin();
                for (; it_clients != _m_prefixclient.end(); it_clients++) {
                        if (it_clients->second.getNick() == msg.getParams().front() && it_clients->second.getFd() != 0) {
                                it_clients->second.setOp(true);
                                std::cout << "User : " << it_clients->second.getNick() << " has been OP" << std::endl;
                        }
                }
        }
        else {
                send_err(fd, ERR_PASSWDMISMATCH, " :Password incorrect\n");
        }
        return (0);

}

int Server::send_privmsg(int fd, std::string str) {
        return (send(fd, str.c_str(), str.size(), 0));
}

bool Server::nick_check(std::string &nick, int fd){
        std::string s(ft_itoa(fd));
        std::map<std::string, Client>::iterator it = this->_m_prefixclient.begin();
        while(it != this->_m_prefixclient.end()) {
                std::cout << (it->second.getNick()) << '\n';
                if (it->second.getNick() == nick && it->second.getFd() != 0) {
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
                _m_prefixclient[s].setNick(msg.getParams().front());
                _m_prefixclient[s].setNickstatus(true);
        }
        if (_m_prefixclient[s].getNickstatus() && _m_prefixclient[_m_fdprefix[fd]].getUserstatus()) {
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
                }
                else {
                        send_privmsg(fd, "ERROR :Access denied: Bad password?\n"); // erreur
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
        if (_m_prefixclient[s].getNickstatus() == true) {
                if (msg.getParams().size() == 4) {
                        if (_m_prefixclient[s].getReg() == false) {
                                if (_m_prefixclient[s].getReg()) {
                                        std::cout << "erreur" << '\n';
                                }
                                // nb params
                                _m_prefixclient[s].setUser(msg.getParams().front());
                                _m_prefixclient[s].setReal(msg.getParams().back());
                                std::string prefix;


                                prefix = _m_prefixclient[s].getNick();
                                prefix += "!";
                                prefix += _m_prefixclient[s].getReal();
                                std::cout << _m_prefixclient[s].getReal() << '\n';
                                prefix += "@";
                                prefix += inet_ntoa(_v_clients.back().getInfo().sin_addr);
                                std::cout << prefix << '\n';
                                _m_fdprefix[fd] = prefix;
                                _m_prefixclient[prefix] = _m_prefixclient[s];
                                _m_prefixclient[prefix].setUserstatus(true);
                                _m_prefixclient[prefix].setFd(fd);
                                if (_m_prefixclient[prefix].getNickstatus() && _m_prefixclient[prefix].getUserstatus()) {
                                        _m_prefixclient[prefix].setReg(true);
                                }
                                send_privmsg(fd, ":irc.example.net 001 " + _m_prefixclient[prefix].getNick() + " :Welcome to the Internet Relay Network " + _m_fdprefix[fd] + "\n");
                        }
                        else{
                                send_err(fd, ERR_ALREADYREGISTERED, " :You may not reregister\n");
                        }
                }
                else {
                        send_err(fd, ERR_NEEDMOREPARAMS, " <USER> :Not enough parameters\n");
                }
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

void Server::remove_Cinchans(int fd)
{
        std::map<std::string, std::vector<Client*> >::iterator it_chan = chan.begin();
        for (; it_chan != chan.end(); it_chan++) {
                std::vector<Client*>::iterator it_cli = it_chan->second.begin();
                for (; it_cli != it_chan->second.end(); it_cli++) {
                        if ((**it_cli).getFd() == _m_prefixclient[_m_fdprefix[fd]].getFd()) {
                                sendtoAll(fd, (it_chan)->second, ":" + _m_fdprefix[fd] + " QUIT :Client closed connection\n");
                                std::cout << "USER FOUND Let's kick him -->" << (**it_cli).getNick() << std::endl;
                                break;
                        }
                }
                if (it_cli != it_chan->second.end()) {
                        if (_m_prefixclient[_m_fdprefix[fd]].getNick() == chan_oper[it_chan->first]) {
                                chan_oper.erase(it_chan->first);
                        }
                        it_chan->second.erase(it_cli);
                }
        }
}

void Server::sendtoAllbutme(int fd, std::vector<Client*> at, std::string msg)
{
        std::vector<Client*>::iterator client = at.begin();
        for (; client != at.end(); client++) {
                // Client to_send = *(*client);
                int fd_tmp = (*(*client)).getFd();
                if (fd_tmp > 0 && fd != fd_tmp)
                        send_privmsg(fd_tmp, msg + "\n");
        }

}

void Server::sendtoAll(int fd, std::vector<Client*> at, std::string msg)
{
        std::vector<Client*>::iterator client = at.begin();
        for (; client != at.end(); client++) {
                // Client to_send = *(*client);
                int fd_tmp = (*(*client)).getFd();
                if (fd_tmp > 0)
                        send_privmsg(fd_tmp, msg + "\n");
        }

}

std::string Server::getmsg(Message &msg, int fd, std::string name)
{
        std::list<std::string> params = msg.getParams();
        std::string msg_tosend;
        msg_tosend = ":" + _m_fdprefix[fd] + " PRIVMSG " + name + " : ";
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
                std::list<std::string> receiver_list = split_every_char(msg.getParams().front(), ',');
                std::list<std::string>::iterator it_recv = receiver_list.begin();
                while(it_recv != receiver_list.end())
                {
                        std::string msg_tosend = getmsg(msg, fd, *it_recv);
                        std::cout << "it_recv -> " << *it_recv << std::endl;
                        if ((*it_recv)[0] == '#') {
                                if (chan.find(*it_recv) != chan.end()) {
                                        sendtoAllbutme(fd, chan[*it_recv], msg_tosend);
                                }
                                else {
                                        send_err(fd, ERR_CANNOTSENDTOCHAN, "<" + *it_recv + "> :Cannot send to channel\n");
                                }
                        }
                        else {
                                std::cout << "searching for clients" << '\n';
                                std::map<std::string, Client>::iterator it_clients = _m_prefixclient.begin();
                                for (; it_clients != _m_prefixclient.end(); it_clients++) {
                                        if (it_clients->second.getNick() == *it_recv && it_clients->second.getFd() != 0) {
                                                std::cout << "let's send some shit boy -> " << msg_tosend << std::endl;
                                                std::cout << "fd found -> " << it_clients->second.getFd() << std::endl;
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

void Server::dispMemberName(int fd, std::string chan_name) {
        std::string nick = _m_prefixclient[_m_fdprefix[fd]].getNick();
        send_privmsg(fd, ( ":" + static_cast<std::string>(SERV_NAME) + " " + static_cast<std::string>(RPL_NAMREPLY) + " " + nick + " = " + chan_name + " :")); // :irc.example.net 353 koko = #chan :@koko
        std::vector<Client *>::iterator it = chan[chan_name].begin();
        for(; it != chan[chan_name].end(); it++) {
                if ((*it)->getOp() || (chan_oper[chan_name] == (*it)->getNick())) {
                        send_privmsg(fd, "@" + (*it)->getNick() + " ");
                }
                else {
                        send_privmsg(fd, (*it)->getNick() + " ");
                }
        }
        send_privmsg(fd, "\n");
        send_privmsg(fd, ":" + static_cast<std::string>(SERV_NAME) + " " + static_cast<std::string>(RPL_ENDOFNAMES) + " " + nick + " " + chan_name + " :End of NAMES list\n"); //:irc.example.net 366 koko #jiji :End of NAMES list
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
                                                        sendtoAll(fd, chan[chan_name], ":" + _m_fdprefix[fd] + " JOIN :" + chan_name);
                                                        dispMemberName(fd, chan_name);

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
                                        chan_oper[chan_name] = temp->getNick();
                                        chan[chan_name] = _v_cli_tmp;
                                        if ((msg.getParams().size() == 2) && (*it_pass != "")) { //creation d'un chan a mdp
                                                chan_flag[chan_name] = "+m";
                                                chan_pass[chan_name] = msg.getParams().back();
                                        }
                                        sendtoAll(fd, chan[chan_name], ":" +  _m_fdprefix[fd] + " JOIN :" + chan_name);
                                        dispMemberName(fd, chan_name);
                                        // sendtoAll(chan[chan_name], (static_cast<std::string>(SERV_NAME) + " " + static_cast<std::string>(RPL_NAMREPLY) +  " lol")); //send to all
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
        send_privmsg(fd, ":" + static_cast<std::string>(SERV_NAME) + " " + err + msg);
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
                        else if (msg.getCmd() == "OPER") {
                                opercmd(msg, fd);
                        }
                        else if (msg.getCmd() == "LIST") {
                                listcmd(msg, fd);
                        }
                        else if (msg.getCmd() == "KICK") {
                                kickcmd(msg, fd);
                        }
                        else if (msg.getCmd() == "NAME") {
                                namecmd(msg, fd);
                        }
                        if (_m_prefixclient[_m_fdprefix[fd]].getOp() == true) {
                                if (msg.getCmd() == "KILL")
                                        killcmd(msg, fd);
                                else if (msg.getCmd() == "SQUIT")
                                        squitcmd(msg, fd);
                        }
                }
                else
                        send_err(fd, ERR_UNKNOWNCOMMAND, " :Unknown command\n");
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
