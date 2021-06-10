
#include "../inc/Message.hpp"

Message::Message(){
}

Message::Message( Message const & src ) : params(src.params){
        this->command = src.command;
}

Message::~Message(){
}

Message &   Message::operator=(Message const & rhs ){
        this->command = rhs.command;
        return(*this);
}

void Message::disp_mess(Message mess) {

        int i = 0;
        std::list<std::string> temp = mess.getParams();

        //printing the command
        std::cout << "CMD :\t" << mess.getCmd() << std::endl;

        //printing the parameters of the this
        std::list<std::string>::iterator it = temp.begin();
        for(; it != temp.end(); it++)
        {
                std::cout << std::endl << "PAR " << i << " :\t";
                std::cout << *it;
                i++;
        }

}

std::string Message::split_first_char(std::string &str, char separator) {
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

std::list<std::string> Message::split_char(std::string str, char separator) {
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

void Message::parsing_cmd(char buf[5000]) {
        std::string buf_str(buf);
        std::list<std::string> list_temp;

        std::string rhs = split_first_char(buf_str, ':');
        list_temp = split_char(buf_str, ' ');

        if (rhs.length() != 0)
                list_temp.push_back(rhs);

        std::list<std::string>::iterator it = list_temp.begin();
        this->setCmd(*it);
        list_temp.erase(it);
        this->setParams(list_temp);

}
