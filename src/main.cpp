// A's part

#include "Server.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int	main(int argc, char** argv)
{
	if (argc != 3 || argv[2][0] == '\0')
	{
		std::cerr << "usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	std::signal(SIGINT, &Server::stop);
	std::signal(SIGTERM, &Server::stop);
	std::signal(SIGPIPE, SIG_IGN);
	try
	{
		Server	server(std::atoi(argv[1]), argv[2]);
		server.run();
	}
	catch (const std::exception& error)
	{
		std::cerr << "error: " << error.what() << std::endl;
		return (1);
	}
	return (0);
}
