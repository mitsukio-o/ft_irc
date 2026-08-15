#ifndef NUMERICS_HPP
# define NUMERICS_HPP

# define RPL_WELCOME(client, network, nick, user, host) (":ircserv 001 " + client + " :Welcome to the " + network + " Network, " + nick + "[!" + user + "@" + host + "]")
# define RPL_YOURHOST(client, servername, version) (":ircserv 002 " + client + " :Your host is " + servername " , running version " + version)
# define RPL_CREATED(client) (":ircserv 003 " + client + " :This server was created at some time (no_timestamp)")
# define RPL_MYINFO(client, servername, version) (":ircserv 004 " + client + " " + servername + " " + version + "   " + "itkl [kl]")
# define RPL_ISUPPORT(client) (":ircserv 005 " + client + "CHANTYPES=#& PREFIX=(o)@ :are supported by this server")

# define RPL_UMODEIS(client, user_modes) (":ircserv 221 " + client + " " + user_modes)
# define RPL_LUSERCLIENT(client, u) (":ircserv 251 " + client + " :There are " + u + " users and 0 invisible on 1 servers")
# define RPL_LUSERME(client, c) (":ircserv 255 " + client + " :I have " + c + " clients and 1 servers")

# define RPL_CHANNELMODEIS(client, channel, modestr, modeargs) (":ircserv 324 " + client + " " + channel + " " + modestr + " " + modeargs)
# define RPL_NOTOPIC(client, channel) (":ircserv 331 " + client + " " + channel + " :No topic is set")
# define RPL_TOPIC(client, channel, topic) (":ircserv 332 " + client + " " + channel + " :" + topic)
# define RPL_INVITING(client, nick, channel) (":ircserv 341 " + client + " " + nick + " " + channel)
# define RPL_NAMREPLY(client, symbol, channel, names) (":ircserv 353 " + client + " " + symbol + " " + channel + " :" + names)
# define RPL_ENDOFNAMES(client, channel) (":ircserv 366 " + client + " " + channel + " :End of /NAMES list")

# define ERR_NOSUCHNICK(client, nick) (":ircserv 401 " + client + " " + nick + " :No such nick/channel")
# define ERR_NOSUCHCHANNEL(client, channel) (":ircserv 403 " + client + " " + channel + " :No such channel")
# define ERR_CANNOTSENDTOCHAN(client, channel) (":ircserv 404 " + client + " " + channel + " :Cannot send to channel")

# define ERR_NOORIGIN(client) (":ircserv 409 " + client + " :No origin specified")
# define ERR_NORECIPIENT(client, command) (":ircserv 411 " + client + " :No recipient given (" + command + ")")
# define ERR_NOTEXTTOSEND(client) (":ircserv 412 " + client + " :No text to send")

# define ERR_UNKNOWNCOMMAND(client, command) (":ircserv 421 " + client + " " + command + " :Unknown command")
# define ERR_NOMOTD(client) (":ircserv 422 " + client + " :MOTD File is missing")

# define ERR_NONICKNAMEGIVEN(client) (":ircserv 431 " + client + " :No nickname given")
# define ERR_ERRONEUSNICKNAME(client, nick) (":ircserv 432 " + client + " " + nick + " :Erroneus nickname")
# define ERR_NICKNAMEINUSE(client, nick) (":ircserv 433 " + client + " " + nick + " :Nickname is already in use")
// # define ERR_NICKCOLLISION(client, nick, user, host) (":ircserv 436 " + client + " " + nick + " :Nickname collision KILL from " + user + "@" + host)

# define ERR_USERNOTINCHANNEL(client, nick, channel) (":ircserv 441 " + client + " " + nick + " " + channel + " :They aren't on that channel")
# define ERR_NOTONCHANNEL(client, channel) (":ircserv 442 " + client + " " + channel + " :You're not on that channel")
# define ERR_USERONCHANNEL(client, nick, channel) (":ircserv 443 " + client + " " + nick + " " + channel + " :is already on channel")

# define ERR_NOTREGISTERED(client) (":ircserv 451 " + client + " :You have not registered")
# define ERR_NEEDMOREPARAMS(client, command) (":ircserv 461 " + client + " " + command + " :Not enough parameters")
# define ERR_ALREADYREGISTERED(client) (":ircserv 462 " + client + " :You may not register")
# define ERR_PASSWDMISMATCH(client) (":ircserv 464 " + client + " :Password incorrect")

# define ERR_CHANNELISFULL(client, channel) (":ircserv 471 " + client + " " + channel + " :Cannot join channel (+l)")
# define ERR_UNKNOWNMODE(client, modechar) (":ircserv 472 " + client + " " + modechar + " :is unknown mode char to me")
# define ERR_INVITEONLYCHAN(client, channel) (":ircserv 473 " + client + " " + channel + " :Cannot join channel (+i)")
# define ERR_BADCHANNELKEY(client, channel) (":ircserv 475 " + client + " " + channel + " :Cannot join channel (+k)")
# define ERR_BADCHANMASK(client, channel) (":ircserv 476 " + client + " " + channel + " :Bad Channel Mask")

# define ERR_CHANOPRIVSNEEDED(client, channel) (":ircserv 482 " + client + " " + channel + "  :You're not channel operator")

# define ERR_UMODEUNKNOWNFLAG(client) (":ircserv 501 " + client + " :Unknown MODE flag")
# define ERR_USERSDONTMATCH(client) (":ircserv 502 " + client + " :Cant change mode for other users")

#endif