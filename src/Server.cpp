// A's part

#include "Server.hpp"
#include <csignal>

static volatile sig_atomic_t	g_stop = 0;

Server::Server(int port, const std::string& password)
	: _socket(-1), _password(password), _clients(), _channels(), _poll()
{
	(void)port;
}

Server::~Server()
{
}

void	Server::stop(int signal)
{
	(void)signal;
	g_stop = 1;
}

void	Server::run()
{
}

void	Server::build()	{}
void	Server::reap()	{}
void	Server::greet()	{}

void	Server::receive(Client& client)	{ (void)client; }
void	Server::flush(Client& client)	{ (void)client; }

void	Server::quit(Client& client, const std::string& reason, bool graceful)
{
	(void)client;
	(void)reason;
	(void)graceful;
}

void	Server::reply(Client& client, const std::string& message)
{
	(void)client;
	(void)message;
}

void	Server::announce(Client& client, const std::string& message)
{
	(void)client;
	(void)message;
}

Client*	Server::findClient(const std::string& nick) const
{
	(void)nick;
	return (0);
}

Channel*	Server::findChannel(const std::string& name) const
{
	(void)name;
	return (0);
}

Channel*	Server::openChannel(const std::string& name)
{
	(void)name;
	return (0);
}

void	Server::closeIfEmpty(Channel& channel)	{ (void)channel; }
