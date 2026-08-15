// A's part

#include "Server.hpp"
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <set>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

static volatile sig_atomic_t	g_stop = 0;

// socket → setsockopt → fcntl → bind → listenの順は固定
Server::Server(int port, const std::string& password)
	: _socket(-1), _password(password), _clients(), _channels(), _poll()
{
	sockaddr_in	address;
	const int	yes = 1;

	_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		throw std::runtime_error("socket() failed");
	// SO_REUSEADDR は bind より前でないと効かない（TIME_WAIT対策用）
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

// reap → build → poll → イベント処理 の輪を回す。poll はこの 1 箇所だけ
void	Server::run()
{
	while (!g_stop)
	{
		reap();
		build();
		if (::poll(&_poll[0], _poll.size(), -1) < 0)
			break ;
		if (_poll[0].revents & POLLIN)
			greet();
		// _poll[0] はリッスンソケット。1 番目以降がクライアント
		for (size_t i = 1; i < _poll.size(); ++i)
		{
			ClientMap::iterator	it = _clients.find(_poll[i].fd);

			if (it == _clients.end())
				continue ;
			if (_poll[i].revents & (POLLERR | POLLHUP | POLLNVAL))
				quit(*it->second, "Connection error", false);
			else
			{
				if (_poll[i].revents & POLLOUT)
					flush(*it->second);
				if (_poll[i].revents & POLLIN)
					receive(*it->second);
			}
		}
	}
}

// リッスンソケット＋全クライアント。送るものがある時だけPOLLOUTを足す
void	Server::build()
{
	pollfd	entry;

	_poll.clear();
	entry.fd = _socket;
	entry.events = POLLIN;
	entry.revents = 0;
	_poll.push_back(entry);
	for (ClientMap::const_iterator it = _clients.begin();
		it != _clients.end(); ++it)
	{
		entry.fd = it->first;
		entry.events = 0;
		entry.revents = 0;
		// 切断待ちの相手からはもう読まない（送り切るのだけ待つ）
		if (!it->second->isQuitting())
			entry.events |= POLLIN;
		if (!it->second->pending().empty())
			entry.events |= POLLOUT;
		_poll.push_back(entry);
	}
}

// acceptして非ブロッキング、_clientsに登録するところまでこの関数でやる
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

// closeとdeleteはここでまとめて、ループ中に消すとセグフォになる
void	Server::reap()
{
	ClientMap::iterator	it = _clients.begin();

	while (it != _clients.end())
	{
		if (it->second->isQuitting() && it->second->pending().empty())
		{
			::close(it->first);
			delete it->second;
			_clients.erase(it++);
		}
		else
			++it;
	}
}

// recv → バッファに連結 → 行に切って 1 行ずつ execute へ渡す
void	Server::receive(Client& client)
{
	char			buffer[4096];
	const ssize_t	size = ::recv(client.getFd(), buffer, sizeof(buffer), 0);
	std::string		line;

	// 0以下なら理由を問わず切断として扱う（エラー要因は問わない）
	if (size <= 0)
		return (quit(client, "Connection closed", false));
	client.store(buffer, static_cast<size_t>(size));
	if (client.isFlooded())
		return (quit(client, "Input line too long", false));
	while (!client.isQuitting() && client.nextLine(line))
		execute(client, line);
}

// 送れた分だけキューから削る。残りは次に POLLOUT が立ったとき
void	Server::flush(Client& client)
{
	std::string&	pending = client.pending();
	const ssize_t	size = ::send(client.getFd(), pending.c_str(),
			pending.size(), 0);

	if (size <= 0)
		return (quit(client, "Broken connection", false));
	pending.erase(0, static_cast<size_t>(size));
}

// 印を付けるだけ。実際に閉じるのは reap（送り残しを届けてから消すため）
void	Server::quit(Client& client, const std::string& reason, bool graceful)
{
	ChannelMap::iterator	it = _channels.begin();

	if (client.isQuitting())
		return ;
	// 名簿から消す前に通知する。消してからでは配る相手が分からなくなる
	if (client.isRegistered())
		announce(client, ":" + client.getPrefix() + " QUIT :" + reason, false);
	while (it != _channels.end())
	{
		it->second->leave(client);
		if (it->second->isEmpty())
		{
			delete it->second;
			_channels.erase(it++);
		}
		else
			++it;
	}
	client.setQuitting();
	// 相手が既にいない切断なら、送り残しは捨てる
	if (!graceful)
		client.pending().clear();
}

void	Server::reply(Client& client, const std::string& message)
{
	client.push(message);
}

// 同席者を set に集めてから配る。2 つの部屋で同席していても 1 回だけ届く
void	Server::announce(Client& client, const std::string& message, bool toSelf)
{
	std::set<Client*>	targets;

	for (ChannelMap::const_iterator it = _channels.begin();
		it != _channels.end(); ++it)
	{
		if (!it->second->has(client))
			continue ;
		const std::set<Client*>&	members = it->second->getMembers();
		targets.insert(members.begin(), members.end());
	}
	targets.erase(&client);
	if (toSelf)
		targets.insert(&client);
	for (std::set<Client*>::iterator it = targets.begin();
		it != targets.end(); ++it)
		(*it)->push(message);
}

// ニックの比較は大小を無視する。切断待ちの相手は見つけない
Client*	Server::findClient(const std::string& nick) const
{
	const std::string	key = toLower(nick);

	for (ClientMap::const_iterator it = _clients.begin();
		it != _clients.end(); ++it)
	{
		if (!it->second->isQuitting() && toLower(it->second->getNick()) == key)
			return (it->second);
	}
	return (0);
}

// キーは小文字。#Chan と #chan を同じ部屋にするため
Channel*	Server::findChannel(const std::string& name) const
{
	const ChannelMap::const_iterator	it = _channels.find(toLower(name));

	if (it == _channels.end())
		return (0);
	return (it->second);
}

// 無ければその場で作る。部屋は最初の 1 人が入った瞬間に生まれる
Channel*	Server::openChannel(const std::string& name)
{
	Channel*	channel = findChannel(name);

	if (channel == 0)
	{
		channel = new Channel(name);
		_channels[toLower(name)] = channel;
	}
	return (channel);
}

// 最後の 1 人が去ったら部屋を消す
void	Server::closeIfEmpty(Channel& channel)
{
	if (!channel.isEmpty())
		return ;
	_channels.erase(toLower(channel.getName()));
	delete &channel;
}
