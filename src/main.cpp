// A's part

#include "Server.hpp"
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

// 数字だけか、1024-65535 に収まるかを見る（atoi は不正な文字を弾けない）
static bool	readPort(const std::string& text, int& port)
{
	long	value;

	if (text.empty() || text.size() > 5)
		return (false);
	for (size_t i = 0; i < text.size(); ++i)
		if (!std::isdigit(static_cast<unsigned char>(text[i])))
			return (false);
	value = std::atol(text.c_str());
	if (value < 1024 || value > 65535)
		return (false);
	port = static_cast<int>(value);
	return (true);
}

int	main(int argc, char** argv)
{
	int	port;

	if (argc != 3 || !readPort(argv[1], port) || argv[2][0] == '\0')
	{
		std::cerr << "usage: ./ircserv <port> <password>" << std::endl;
		std::cerr << "       port 1024-65535, password must not be empty"
				  << std::endl;
		return (1);
	}
	std::signal(SIGINT, &Server::stop);
	std::signal(SIGTERM, &Server::stop);
	std::signal(SIGPIPE, SIG_IGN);	// 閉じた相手への send で落ちるのを防ぐ
	try
	{
		Server	server(port, argv[2]);
		server.run();
	}
	catch (const std::exception& error)
	{
		std::cerr << "error: " << error.what() << std::endl;
		return (1);
	}
	return (0);
}
