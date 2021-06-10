#pragma once

# include <iostream>
# include "Client.hpp"

class Client;

class Server
{

	public:

		Server();
		Server(Server const & src);
		virtual ~Server();

		Server &		operator=( Server const & rhs );


	private:

    Client only_cli;

};

// std::ostream &			operator<<( std::ostream & o, Server const & i );
