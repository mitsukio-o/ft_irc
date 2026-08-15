// B's part

#include "Server.hpp"
#include <sstream>
#include <iostream>

std::string	CHANTYPES = "#&";
std::string	MEMPREFIX = "~@%+";
std::string	NICKBANPRE = "$:";
std::string	NICKBAN = " ,*?!@.\0\r\n";
std::string	USERBAN = " \0\r\n";
std::string	REALBAN = "\0\r\n";

std::string SRVNAME = "ircserv";

// { name, handler, needs registration, minimum number of parameters }
const Server::Command	Server::_commands[] = {
	{ "CAP",     &Server::cmdCap,     false, 1 },
	{ "PASS",    &Server::cmdPass,    false, 1 },
	{ "NICK",    &Server::cmdNick,    false, 1 },
	{ "USER",    &Server::cmdUser,    false, 4 },
	{ "PING",    &Server::cmdPing,    false, 1 },
	{ "PONG",    &Server::cmdPong,    false, 0 },
	{ "QUIT",    &Server::cmdQuit,    false, 0 },
	{ "JOIN",    &Server::cmdJoin,    true,  1 },
	{ "PART",    &Server::cmdPart,    true,  1 },
	{ "TOPIC",   &Server::cmdTopic,   true,  1 },
	{ "INVITE",  &Server::cmdInvite,  true,  2 },
	{ "KICK",    &Server::cmdKick,    true,  2 },
	{ "MODE",    &Server::cmdMode,    true,  1 },
	{ "PRIVMSG", &Server::cmdPrivmsg, true,  1 },
	{ "NOTICE",  &Server::cmdNotice,  true,  1 },
	{ 0, 0, false, 0 }
};

void	Server::execute(Client& client, const std::string& line)
{
	client.push("echo: " + line);	// 受信チェック用の仮実装。B-4 で消す
}

void	Server::welcome(Client& c)
{
	// welcome sequence
	reply(c, RPL_WELCOME(c.getNick(), "ircserv", c.getNick(), c.getUser(), c.getHost()));
	reply(c, RPL_YOURHOST(c.getNick(), "ircserv", "1.0"));
	reply(c, RPL_CREATED(c.getNick()));
	reply(c, RPL_MYINFO(c.getNick(), "ircserv", "1.0"));
	reply(c, RPL_ISUPPORT(c.getNick()));

	// reply to LUSERS
	std::stringstream	ss;
	ss << _clients.size();

	reply(c, RPL_LUSERCLIENT(c.getNick(), ss.str()));
	reply(c, RPL_LUSERME(c.getNick(), ss.str()));

	// MOTD
	reply(c, ERR_NOMOTD(c.getNick()));
}

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
		announce(c, c.getSource() + " NICK :" + nick);
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
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "JOIN")));
	
	if (a[0] == "0")
	{
		std::vector<std::string>			chans;
		Server::ChannelMap::const_iterator	it;

		for (it = _channels.begin(); it != _channels.end(); ++it)
		{
			if (it->second->has(c))
				chans.push_back(it->second->getName());
		}
		if (!chans.empty())
			cmdPart(c, chans);
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
			{
				reply(c, ERR_BADCHANNELKEY(c.getNick(), channelName));
				continue;
			}
			if (channel->isInviteOnly() && !channel->isInvited(c))
			{
				reply(c, ERR_INVITEONLYCHAN(c.getNick(), channelName));
				continue;
			}
			if (channel->getLimit() != 0 && channel->getLimit() >= channel->getMembers().size())
			{
				reply(c, ERR_CHANNELISFULL(c.getNick(), channelName));
				continue;
			}
			channel->join(&c, false);
		}
		channel->broadcast(c.getSource() + " JOIN :" + channelName);
		if (channel->getTopic().empty())
			reply(c, RPL_NOTOPIC(c.getNick(), channelName));
		else
			reply(c, RPL_TOPIC(c.getNick(), channelName, channel->getTopic()));
		reply(c, RPL_NAMREPLY(c.getNick(), "=", channelName, channel->getNames()));
		reply(c, RPL_ENDOFNAMES(c.getNick(), channelName));
	}
}

// removes client from channels
void	Server::cmdPart(Client& c, const Args& a)
{
	if (a.empty())
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "PART")));

	std::vector<std::string>	channelNames;
	std::string					token;

	std::stringstream	ssChannels(a[0]);
	while (std::getline(ssChannels, token, ','))
		channelNames.push_back(token);
	
	std::string	reason;
	if (a.size() > 2)
		reason = a[1];
	
	Channel*	channel;
	std::vector<std::string>::const_iterator	it;
	for (it = channelNames.begin(); it != channelNames.end(); ++it)
	{
		channel = findChannel(*it);
		if (!channel)
		{
			reply(c, ERR_NOSUCHCHANNEL(c.getNick(), *it));
			continue;
		}
		if (!channel->has(c))
		{
			reply(c, ERR_NOTONCHANNEL(c.getNick(), *it));
			continue;
		}
		channel->broadcast(c.getSource() + " PART " + *it + " :" + reason);
		channel->leave(c);
	}
}

// change topic in channel
void	Server::cmdTopic(Client& c, const Args& a)
{
	if (a.empty())
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "TOPIC")));

	Channel*	channel = findChannel(a[0]);
	if (!channel)
		return (reply(c, ERR_NOSUCHCHANNEL(c.getNick(), a[0])));
	if (!channel->has(c))
		return (reply(c, ERR_NOTONCHANNEL(c.getNick(), a[0])));
	
	if (a.size() < 2)
	{
		if (channel->getTopic().empty())
			return (reply(c, RPL_NOTOPIC(c.getNick(), a[0])));
		else
			return (reply(c, RPL_TOPIC(c.getNick(), a[0], channel->getTopic())));
	}

	if (channel->isTopicLocked() && !channel->isOperator(c))
		return (reply(c, ERR_CHANOPRIVSNEEDED(c.getNick(), a[0])));
	channel->setTopic(a[1]);
	channel->broadcast(":ircserv TOPIC " + a[0] + " :" + channel->getTopic());
}

// invites a person to the channel
void	Server::cmdInvite(Client& c, const Args& a)
{
	if (a.size() < 2)
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "INVITE")));
	
	Channel*	channel = findChannel(a[1]);
	if (!channel)
		return (reply(c, ERR_NOSUCHCHANNEL(c.getNick(), a[1])));
	if (!channel->has(c))
		return (reply(c, ERR_NOTONCHANNEL(c.getNick(), a[1])));
	if (channel->isInviteOnly() && !channel->isOperator(c))
		return (reply(c, ERR_CHANOPRIVSNEEDED(c.getNick(), a[1])));
	if (channel->find(a[0]));
		return (reply(c, ERR_USERONCHANNEL(c.getNick(), a[0], a[1])));
	
	Client*	invited = findClient(a[0]);
	if (!invited)
		return ;
	
	channel->invite(*invited);
	reply(c, RPL_INVITING(c.getNick(), a[0], a[1]));
	reply(*invited, c.getSource() + " INVITE " + a[0] + a[1]);
}

// kicks a user from channel
void	Server::cmdKick(Client& c, const Args& a)
{
	if (a.size() < 2)
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "KCIK")));
	
	Channel*	channel = findChannel(a[0]);
	if (!channel)
		return (reply(c, ERR_NOSUCHCHANNEL(c.getNick(), a[0])));
	if (!channel->has(c))
		return (reply(c, ERR_NOTONCHANNEL(c.getNick(), a[0])));
	if (!channel->isOperator(c))
		return (reply(c, ERR_CHANOPRIVSNEEDED(c.getNick(), a[0])));

	std::vector<std::string>	userNames;
	std::string					token;

	std::stringstream	ssUsers(a[1]);
	while (std::getline(ssUsers, token, ','))
		userNames.push_back(token);
	
	std::string	message = " :Was Kicked";
	if (a.size() > 1)
		message = " :" + a[1];
	
	std::vector<std::string>::const_iterator	it;
	for (it = userNames.begin(); it != userNames.end(); ++it)
	{
		Client*	client = findClient(*it);
		if (!client)
			continue;
		if (!channel->has(*client))
		{
			reply(c, ERR_USERNOTINCHANNEL(c.getNick(), *it, a[0]));
			continue;
		}
		channel->broadcast(c.getSource() + " KICK " + a[0] + *it + message);
		channel->leave(*client);
	}
}

static std::string	getModestr(Channel& channel)
{
	std::string	modestr = "+";

	if (channel.isInviteOnly())
		modestr += "i";
	if (channel.isTopicLocked())
		modestr += "t";
	if (!channel.getKey().empty())
		modestr += "k";
	if (channel.getLimit() != 0)
		modestr += "l";
	return (modestr);
}

static std::string	getModeArgs(Channel& channel)
{
	if (channel.getLimit() == 0)
		return ("");

	std::stringstream	ss;
	ss << channel.getLimit();
	return (ss.str());
}

static size_t	strToLimit(std::string str)
{
	size_t				limit;
	std::stringstream	ss(str);
	char				extra;

	if (!(ss >> limit))
		return (0);
	if (ss >> extra)
		return (0);
	return (limit);
}

// set or remove options from a given target
void	Server::cmdMode(Client& c, const Args& a)
{
	if (a.empty())
		return (reply(c, ERR_NEEDMOREPARAMS(c.getNick(), "MODE")));

	// when target is a user
	if (CHANTYPES.find(a[0]) != std::string::npos)
	{
		std::string	nick = a[0];
		Client*	client = findClient(nick);
		if (!client)
			return (reply(c, ERR_NOSUCHNICK(c.getNick(), nick)));
		if (client != &c)
			return (reply(c, ERR_USERSDONTMATCH(c.getNick())));
		
		if (a.size() < 2)
			return (reply(c, RPL_UMODEIS(c.getNick(), "+")));

		return (reply(c, ERR_UMODEUNKNOWNFLAG(c.getNick())));
	}

	// when target is a channel
	else
	{
		Channel*	channel = findChannel(a[0]);
		if (!channel)
			return (reply(c, ERR_NOSUCHCHANNEL(c.getNick(), a[0])));
		
		if (a.size() < 2)
			return (reply(c, RPL_CHANNELMODEIS(c.getNick(), a[0], getModestr(*channel), getModeArgs(*channel))));

		if (!channel->isOperator(c))
			return (reply(c, ERR_CHANOPRIVSNEEDED(c.getNick(), a[0])));

		std::string	modestr = a[1];
		size_t		lenModestr = modestr.length();
		size_t		itModeArg = 2;
		bool		isAdd = true;
		std::string	actModestr;
		std::string	actModeArg;
		bool		actIsAdd = true;
		std::string	errChar;

		for (size_t i = 0; i < lenModestr; ++i)
		{
			bool		didSomething = false;
			std::string	hasModeArg;
			if (modestr[i] == '+')
				isAdd = true;
			else if (modestr[i] == '-')
				isAdd = false;

			else if (modestr[i] == 'i')
			{
				if (isAdd && !channel->isInviteOnly())
				{
					channel->setInviteOnly(true);
					didSomething = true;
				}
				else if (!isAdd && channel->isInviteOnly())
				{
					channel->setInviteOnly(false);
					didSomething = true;
				}
				else
					continue;
			}

			else if (modestr[i] == 't')
			{
				if (isAdd && !channel->isTopicLocked())
				{
					channel->setTopicLocked(true);
					didSomething = true;
				}
				else if (!isAdd && channel->isInviteOnly())
				{
					channel->setInviteOnly(false);
					didSomething = true;
				}
				else
					continue;
			}

			else if (modestr[i] == 'k')
			{
				if (!isAdd)
				{
					if (!channel->getKey().empty())
					{
						channel->setKey("");
						didSomething = true;
					}
					else
						continue;
				}
				else
				{
					if (itModeArg >= a.size())
						break;
					hasModeArg = a[itModeArg];
					++itModeArg;
					if (hasModeArg.empty() || hasModeArg == channel->getKey())
						continue;
					channel->setKey(hasModeArg);
					didSomething = true;			
				}
			}

			else if (modestr[i] == 'o')
			{
				if (itModeArg >= a.size())
					break;
				hasModeArg = a[itModeArg];
				++itModeArg;
				Client*	target = findClient(hasModeArg);
				if (!target || !channel->has(*target))
					continue;
				if (isAdd && !channel->isOperator(*target))
				{
					channel->grant(*target, true);
					didSomething = true;
				}
				else if (!isAdd && channel->isOperator(*target))
				{
					channel->grant(*target, false);
					didSomething = true;
				}
				else
					continue;
			}

			else if (modestr[i] == 'l')
			{
				if (!isAdd)
				{
					if (channel->getLimit() != 0)
					{
						channel->setLimit(0);
						didSomething = true;
					}
					else
						continue;
				}
				else
				{
					if (itModeArg >= a.size())
						break;
					hasModeArg = a[itModeArg];
					++itModeArg;
					size_t	limit = strToLimit(hasModeArg);
					if (limit == 0 || limit == channel->getLimit())
						continue;
					channel->setLimit(limit);
					didSomething = true;
				}
			}

			else
				errChar += modestr[i];

			if (didSomething)
			{
				if (actModestr.empty() || actIsAdd != isAdd)
				{
					if (isAdd)
						actModestr += "+";
					else
						actModestr += "-";
					actIsAdd = isAdd;
				}
				actModestr += modestr[i];

				if (!hasModeArg.empty())
					actModeArg += " " + hasModeArg;
			}
		}

		channel->broadcast(c.getSource() + " MODE " + channel->getName() + " " + actModestr + actModeArg);

		if (!errChar.empty())
		{
			size_t	errlen = errChar.length();
			for (size_t i = 0; i < errlen; ++i)
				reply(c, ERR_UNKNOWNMODE(c.getNick(), errChar[i]));
		}

	}
}

void	Server::cmdPrivmsg(Client& c, const Args& a)
{
	if (a.empty() || a[0].empty())
		return (reply(c, ERR_NORECIPIENT(c.getNick(), "PRIVMSG")));
	if (a.size() < 2 || a[1].empty())
		return (reply(c, ERR_NOTEXTTOSEND(c.getNick())));
	
	// make an array of targets
	std::vector<std::string>	targetNames;
	std::string					token;

	std::stringstream	ssTarget(a[1]);
	while (std::getline(ssTarget, token, ','))
		targetNames.push_back(token);

	std::vector<std::string>::const_iterator	it;
	for (it = targetNames.begin(); it != targetNames.end(); ++it)
	{
		// handle channels
		if (CHANTYPES.find((*it)[0]) != std::string::npos)
		{
			Channel*	channel = findChannel(*it);
			if (!channel)
			{
				reply(c, ERR_NOSUCHNICK(c.getNick(), *it));
				continue;
			}
			if (!channel->has(c))
			{
				reply(c, ERR_CANNOTSENDTOCHAN(c.getNick(), *it));
				continue;
			}

			channel->broadcast(c.getSource() + " PRIVMSG " + channel->getName() + " :" + a[1], &c);
		}
		else
		{
			Client*	client = findClient(*it);
			if (!client)
			{
				reply(c, ERR_NOSUCHNICK(c.getNick(), *it));
				continue;
			}
			
			client->push(":" + c.getNick() + " PRIVMSG " + *it + " :" + a[1]);
		}
	}
}

// same as PRIVMSG but never answers with an error
void	Server::cmdNotice(Client& c, const Args& a)
{
	if (a.empty() || a[0].empty())
		return ;
	if (a.size() < 2 || a[1].empty())
		return ;

	std::vector<std::string>	targetNames;
	std::string					token;

	std::stringstream	ssTarget(a[0]);
	while (std::getline(ssTarget, token, ','))
		targetNames.push_back(token);

	std::vector<std::string>::const_iterator	it;
	for (it = targetNames.begin(); it != targetNames.end(); ++it)
	{
		if (it->empty())
			continue;
		if (CHANTYPES.find((*it)[0]) != std::string::npos)
		{
			Channel*	channel = findChannel(*it);
			if (!channel || !channel->has(c))
				continue;
			channel->broadcast(c.getSource() + " NOTICE " + channel->getName()
				+ " :" + a[1], &c);
		}
		else
		{
			Client*	client = findClient(*it);
			if (!client)
				continue;
			client->push(c.getSource() + " NOTICE " + client->getNick()
				+ " :" + a[1]);
		}
	}
}
