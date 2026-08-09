// shared - 2 人で育てる。宣言の追加のみ、整形しない

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"
# include <map>
# include <poll.h>

class Server
{
public:
	typedef std::vector<std::string>		Args;
	typedef std::map<int, Client*>			ClientMap;
	typedef std::map<std::string, Channel*>	ChannelMap;
	typedef void (Server::*Handler)(Client&, const Args&);

	struct Command
	{
		const char*	name;
		Handler		handler;
		bool		registered;
		size_t		params;
	};

	Server(int port, const std::string& password);
	~Server();

	void		run();
	static void	stop(int signal);

private:
	// A
	void	build();
	void	reap();
	void	greet();
	void	receive(Client& client);
	void	flush(Client& client);

	// A（B から呼ばれる境界）
	void	execute(Client& client, const std::string& line);	// 中身は B
	void	quit(Client& client, const std::string& reason, bool graceful);
	void	reply(Client& client, const std::string& message);
	void	announce(Client& client, const std::string& message,
				bool toSelf = true);

	// A（B から呼ばれる）
	Client*		findClient(const std::string& nick) const;
	Channel*	findChannel(const std::string& name) const;
	Channel*	openChannel(const std::string& name);
	void		closeIfEmpty(Channel& channel);

	// B
	void	welcome(Client& client);
	void	cmdCap(Client& client, const Args& args);
	void	cmdPass(Client& client, const Args& args);
	void	cmdNick(Client& client, const Args& args);
	void	cmdUser(Client& client, const Args& args);
	void	cmdPing(Client& client, const Args& args);
	void	cmdPong(Client& client, const Args& args);
	void	cmdQuit(Client& client, const Args& args);
	void	cmdJoin(Client& client, const Args& args);
	void	cmdPart(Client& client, const Args& args);
	void	cmdTopic(Client& client, const Args& args);
	void	cmdInvite(Client& client, const Args& args);
	void	cmdKick(Client& client, const Args& args);
	void	cmdMode(Client& client, const Args& args);
	void	cmdPrivmsg(Client& client, const Args& args);
	void	cmdNotice(Client& client, const Args& args);

	static const Command	_commands[];

	int						_socket;
	std::string				_password;
	ClientMap				_clients;
	ChannelMap				_channels;
	std::vector<pollfd>		_poll;

	Server(const Server& other);
	Server&	operator=(const Server& other);
};

#endif
