#include "../inc/Server.hpp"
// #include "../inc/main.hpp"

bool Server::nick_check(std::string &nick){
        std::vector<Client>::iterator it = this->_v_clients.begin();
        while(it != this->_v_clients.end()) {
                if ((it->getNick() == nick) && (it->getReg() == true)) {
                        return false;
                }
                it++;
        }
        return true;
}

int Server::nickcmd(Message msg, int fd){
        std::string s(ft_itoa(fd));
        if (nick_check(msg.getParams().front()) == false) {
                std::cout << "Already one" << '\n';
        }
        else {
                _m_prefixclient[s].setNick(msg.getParams().front());
        }
        return(0);
}

int Server::passcmd(Message & msg, int fd) {
        std::string s(ft_itoa(fd));
        _m_prefixclient.insert(std::pair<std::string, Client>(s, _v_clients.back()));
        if (msg.getParams().size() >= 2) {
                //send_reply(fd, "mauvais params fdp");
                std::cout << "False params" << '\n';
        }
        else if (msg.getParams().front() == _password) {
                std::cout << "True pass" << '\n';
                _m_prefixclient[s].setCorr(true);
        }
        else{
                //send_reply ("t'as pas le bon mdp fdp")
                std::cout << "bad pass" << '\n';
        }
        return(0);
}

int Server::usercmd(Message &msg, int fd) {
        std::string s(ft_itoa(fd));

        //if () user deja register -- >ERR_ALREADYREGISTERED // wrong parameters // pass
        // if (_m_prefixclient[s].setReg(true);)
        _m_prefixclient[s].setReg(true);
        std::string prefix;

        prefix = _m_prefixclient[s].getUser();
        prefix += "!";
        //std::string prefix(_m_prefixclient[s].username + "!" + _)
        // tous les champs user ! real @ host
        _m_fdprefix[fd] = prefix;
        _m_prefixclient[prefix] = _m_prefixclient[_m_fdprefix[fd]];
        return(0);
}

int Server::do_cmd(Message msg, int fd){
        if (msg.getCmd() == "PASS") {
                passcmd(msg, fd);
        }
        else if (_m_prefixclient[_m_fdprefix[fd]].getCorr() == true) {
                std::cout << "good pass, access" << '\n';
                if (msg.getCmd() == "NICK") {
                        nickcmd(msg, fd);
                }
                else if (msg.getCmd() == "USER") {
                        usercmd(msg, fd);
                }
                else if (_m_prefixclient[_m_fdprefix[fd]].getReg() == true)
                {
                        // join etc...
                }
        }
        else {
                std::cout << "bad cmd" << '\n';
                //balancer erreur
        }
        return (0);
}

// verif if username already exists
// otherwise send error
//
