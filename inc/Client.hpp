#pragma once

# include <iostream>

class Client
{

	public:

    Client(std::string nick, std::string user, std::string real, std::string pass);
		Client( Client const & src );
		virtual ~Client();

		Client &		operator=( Client const & rhs );


	private:

		Client();
    std::string nickname;
    std::string username;
    std::string realname;
    std::string password;

};

// std::ostream &			operator<<( std::ostream & o, Client const & i );
