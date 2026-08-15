// B's part

#include "Server.hpp"
#include <sstream>
#include <iostream>

const Server::Command	Server::_commands[] = {
	{ 0, 0, false, 0 }
};

void	Server::execute(Client& client, const std::string& line)
{
	client.push("echo: " + line);	// A が受信を作る間の仮実装。B-4 で消す
}

void	Server::welcome(Client& client)	{ (void)client; }

// no capability supported
void	Server::cmdCap(Client& c, const Args& a)
{
	if (a.empty())
		return ;
	if (a[0] == "LS" || a[0] == "LIST")
		return (reply(c, ":" SERVER_NAME " CAP * " + a[0] + " :"));
	if (a[0] == "REQ")
	{
		std::string	wanted;

		if (a.size() > 1)
			wanted = a[1];
		return (reply(c, ":" SERVER_NAME " CAP * NAK :" + wanted));
	}
}

// authenticates to connect to the server
void	Server::cmdPass(Client& c, const Args& a)
{
	if (c.isRegistered())
		return (reply(c, ERR_ALREADYREGISTERED(c.getNick())));
	if (a.empty())
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "PASS")));
	if (a[0] != _password)
		return (reply(c, ERR_PASSWDMISMATCH(c.getNick())));

	c.setPass();
	if (c.gotPass() && c.getNick() != "*" && !c.getUser().empty())
	{
		c.setRegistered();
		welcome(c);
	}
}

static bool	ft_contain(const std::string& hay, const std::string& needles)
{
	std::string::const_iterator	it;

	for (it = needles.begin(); it != needles.end(); ++it)
	{
		if (hay.find(*it) != std::string::npos)
			return (true);
	}
	return (false);
}

static bool	ft_dupnick(const Server::ClientMap& clients, const std::string& nick)
{
	Server::ClientMap::const_iterator	it;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNick() == nick)
			return (true);
	}
	return (false);
}

// sets nickname
void	Server::cmdNick(Client& c, const Args& a)
{
	if (a.empty())
		return (reply(c, ERR_NONICKNAMEGIVEN(c.getNick())));

	std::string	nick = a[0];
	if (nick.empty() || CHANTYPES.find(nick[0]) != std::string::npos
		|| MEMPREFIX.find(nick[0]) != std::string::npos
		|| NICKBANPRE.find(nick[0]) != std::string::npos
		|| ft_contain(nick, NICKBAN))
		return (reply(c, ERR_ERRONEUSNICKNAME(c.getNick(), nick)));

	if (ft_dupnick(_clients, nick))
		return (reply(c, ERR_NICKNAMEINUSE(c.getNick(), nick)));
	

	if (c.isRegistered())
	{
		announce(c, ":" + c.getNick() + "!" + c.getUser() + "@tmphost NICK :" + nick);
		c.setNick(nick);
		return ;
	}

	c.setNick(nick);
	if (c.gotPass() && c.getNick() != "*" && !c.getUser().empty())
	{
		c.setRegistered();
		welcome(c);
	}
}

// Specify username and realname
void	Server::cmdUser(Client& c, const Args& a)
{
	if (c.isRegistered())
		return (reply(c, ERR_ALREADYREGISTERED(c.getNick())));
	if (a.size() < 4)
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "USER")));
	
	std::string	username = a[0];
	std::string	realname = a[3];
	if (username.size() < 2)
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "USER")));

	// undefined error for when invalid character is used for username
	if (ft_contain(username, USERBAN) || ft_contain(realname, REALBAN))
		return ;

	c.setUser(username);
	c.setReal(realname);

	if (c.gotPass() && c.getNick() != "*" && !c.getUser().empty())
	{
		c.setRegistered();
		welcome(c);
	}
}

void	Server::cmdPing(Client& c, const Args& a)
{
	if (a.empty())
		return (reply(c, ERR_NOORIGIN(c.getNick())));
	reply(c, ":" SERVER_NAME " PONG " SERVER_NAME " :" + a[0]);
}

// nothing to reply
void	Server::cmdPong(Client& c, const Args& a)
{
	(void)c;
	(void)a;
}

// Quit used from the client side. WARNING: does not handle error quits.
void	Server::cmdQuit(Client& c, const Args& a)
{
	std::string	quitMessage;

	if (!a.empty())
		quitMessage = a[0];

	// graceful??
	quit(c, quitMessage, true);
}

// joins a server or makes one
void	Server::cmdJoin(Client& c, const Args& a)
{
	if (a.empty())
		return (reply(c, ERR_NONICKNAMEGIVEN(c.getNick())));
	
	if (a[0] == "0")
	{
		// TODO Part all channels
		return ;
	}

	std::vector<std::string>	tryChannels;
	std::vector<std::string>	tryKeys;
	std::string					token;

	std::stringstream	ssChannels(a[0]);
	while (std::getline(ssChannels, token, ','))
		tryChannels.push_back(token);

	if (a.size() == 2)
	{
		std::stringstream	ssKeys(a[1]);
		while (std::getline(ssKeys, token, ','))
			tryKeys.push_back(token);
	}

	for (size_t i = 0; i < tryChannels.size(); ++i)
	{
		std::string&	channelName = tryChannels[i];
		if (CHANTYPES.find(channelName[0]) == std::string::npos || ft_contain(channelName, " \a,"))
			return (reply(c, ERR_BADCHANMASK(c.getNick(), channelName)));

		Channel*	channel = findChannel(channelName);
		if (!channel)
		{
			channel = openChannel(channelName);
			channel->join(&c, true);
		}
		else
		{
			std::string	key;
			if (i < tryKeys.size())
				key = tryKeys[i];
			if (!channel->getKey().empty() && channel->getKey() != key)
				return (reply(c, ERR_BADCHANNELKEY(c.getNick(), channelName)));
			if (channel->isInviteOnly() && !channel->isInvited(c))
				return (reply(c, ERR_INVITEONLYCHAN(c.getNick(), channelName)));
			channel->join(&c, false);
		}
		channel->broadcast(":" + c.getNick() + "!" + c.getUser() + "@tmphost JOIN :" + channelName);
		if (!channel->getTopic().empty())
			reply(c, RPL_TOPIC(c.getNick(), channelName, channel->getTopic()));
		reply(c, RPL_NAMREPLY(c.getNick(), "=", channelName, channel->getNames()));
		reply(c, RPL_ENDOFNAMES(c.getNick(), channelName));
	}
}


void	Server::cmdPart(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdTopic(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdInvite(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdKick(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdMode(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdPrivmsg(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdNotice(Client& c, const Args& a)	{ (void)c; (void)a; }
