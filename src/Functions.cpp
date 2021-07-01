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
                if ((it->second.getNick() == nick)/* && (it->getReg() == true)*/) {
                        return false;
                }
                it++;
        }
        return true;
}

int Server::nickcmd(Message msg, int fd){
        std::string s(ft_itoa(fd));
        if (nick_check(msg.getParams().front(), fd) == false) {
                std::cout << "Already one" << '\n';
        }
        else {
                std::cout << "it's ok" << '\n';
                _m_prefixclient[s].setNick(msg.getParams().front());
        }
        return(0);
}

int Server::passcmd(Message & msg, int fd) {
        std::string s(ft_itoa(fd));
        _m_prefixclient.insert(std::pair<std::string, Client>(s, _v_clients.back()));
        if (msg.getParams().size() >= 2)
        {
                //send_reply(fd, "mauvais params fdp");
                send_privmsg(fd, "False params\n");
                std::cout << "False params" << '\n';
        }
        else if (msg.getParams().front() == _password) {
                send_privmsg(fd, "True pass\n");
                _m_prefixclient[s].setCorr(true);
        }
        else{
                send_privmsg(fd, "bad pass\n");
                std::cout << "bad pass" << '\n';
        }
        return(0);
}

int Server::usercmd(Message &msg, int fd) {
        std::string s(ft_itoa(fd));

        //if () user deja register -- >ERR_ALREADYREGISTERED // wrong parameters // pass
        if (_m_prefixclient[s].getReg()){
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
        _m_prefixclient[prefix].setReg(true);
        return(0);
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
                        if (msg.getCmd() == "BIGLOL"){
                          std::cout << _m_prefixclient[_m_fdprefix[fd]].getNick() << '\n';
                          std::cout << _m_prefixclient[_m_fdprefix[fd]].getUser() << '\n';
                          std::cout << _m_prefixclient[_m_fdprefix[fd]].getReal() << '\n';
                          std::cout << _m_prefixclient[_m_fdprefix[fd]].getPass() << '\n';
                        }

                }
        }
        else {
                send_privmsg(fd, "Bad cmd\n");
                std::cout << "bad cmd" << '\n';
                //join privmsg(#chan || nick) OPER-> KILL LUSER(info serv) HELP
        }
        return (0);
}

// verif if username already exists
// otherwise send error
//
