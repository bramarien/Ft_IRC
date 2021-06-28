#include "../inc/Server.hpp"
// #include "../inc/main.hpp"

bool Server::nick_check(std::string &nick){
        std::vector<Client>::iterator it = this->_v_clients.begin();
        while(it != this->_v_clients.end()) {
                if (it->getNick() == nick) {
                        return false;
                }
                it++;
        }
        return true;
}

int Server::nick_fct(Message msg, int fd){
        std::string s(itoa(fd));
        if (_m_prefixclient[s].corr_pass) {
                if (nick_check(msg.getParams().front()) == false) {
                        std::cout << "Already one" << '\n';
                }
                else {
                        _m_prefixclient[s].nickname = msg.getParams().at(0);
                }
        }

        return(0);
}

int Server::passcmd(Message & msg, int fd) {
        std::string s(itoa(fd));
        if (msg->params.count() >= 2) {
                //send_reply(fd, "tg c le mauvais");
        }
        else if (msg->params[0] == _password) {
                _m_prefixclient[s].corr_pass = true;
        }
}

int Server::usercmd(Message &msg, int fd) {
        std::string s(itoa(fd));

        //if () user deja register -- >ERR_ALREADYREGISTERED // wrong parameters // pass
        _m_prefixclient[s].is_registered = true;
        std::string prefix;

        prefix = _m_prefixclient[s].username;
        prefix += "!";
        //std::string prefix(_m_prefixclient[s].username + "!" + _)
        // tous les champs user ! real @ host
        _m_fdprefix[fd] = prefix;
        _m_prefixclient[prefix] = _m_prefixclient[_m_fdprefix[fd]];

}

int Server::do_cmd(Message msg, int fd){
        if (msg->command == "PASS") {
                passcmd(fd);
        }
        else if ("NICK" ==  msg.getCmd()) {
                nick_fct();
        }
        else if (msg->command == "USER") {
                usercmd(fd);
        }
        else if (_m_prefixclient[_m_fdprefix[fd]].is_registered == true)
        {
                // join n
        }
}

// verif if username already exists
// otherwise send error
//
