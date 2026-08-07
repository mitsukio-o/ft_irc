// A's part

#include "Server.hpp"
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

static volatile sig_atomic_t	g_stop = 0;

// socket → setsockopt → fcntl → bind → listen の順は固定
Server::Server(int port, const std::string& password)
	: _socket(-1), _password(password), _clients(), _channels(), _poll()
{
	sockaddr_in	address;
	const int	yes = 1;

	_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		throw std::runtime_error("socket() failed");
	// SO_REUSEADDR は bind より前でないと効かない（TIME_WAIT 対策）
	if (::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		throw std::runtime_error("setsockopt() failed");
	if (::fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl() failed");
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(static_cast<unsigned short>(port));	// バイト順を変換
	if (::bind(_socket, reinterpret_cast<sockaddr*>(&address),
			sizeof(address)) < 0)
		throw std::runtime_error("bind() failed: is the port already in use?");
	if (::listen(_socket, 128) < 0)
		throw std::runtime_error("listen() failed");
}

// 全 Client / Channel を解放してから、自分の fd を閉じる
Server::~Server()
{
	for (ClientMap::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		::close(it->first);
		delete it->second;
	}
	for (ChannelMap::iterator it = _channels.begin();
		it != _channels.end(); ++it)
		delete it->second;
	if (_socket >= 0)
		::close(_socket);
}

// シグナルハンドラではフラグを立てるだけ（非同期シグナル安全にするため）
void	Server::stop(int signal)
{
	(void)signal;
	g_stop = 1;
}

// build → poll → イベント処理 の輪を回す。poll はこの 1 箇所だけ
void	Server::run()
{
	while (!g_stop)
	{
		build();
		if (::poll(&_poll[0], _poll.size(), -1) < 0)
			break ;
		if (_poll[0].revents & POLLIN)
			greet();
	}
}

// 今はまだリッスンソケットだけ。クライアントの fd は受信を書く時に足す
void	Server::build()
{
	pollfd	entry;

	_poll.clear();
	entry.fd = _socket;
	entry.events = POLLIN;
	entry.revents = 0;
	_poll.push_back(entry);
}

// accept して非ブロッキングにし、_clients に登録するところまで
void	Server::greet()
{
	sockaddr_in	address;
	socklen_t	size;
	int			fd;

	size = sizeof(address);
	fd = ::accept(_socket, reinterpret_cast<sockaddr*>(&address), &size);
	if (fd < 0)
		return ;
	if (::fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		::close(fd);
		return ;
	}
	_clients[fd] = new Client(fd, ::inet_ntoa(address.sin_addr));
}

void	Server::reap()	{}

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
