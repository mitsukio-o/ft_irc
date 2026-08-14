#ifndef NUMERICS_HPP
# define NUMERICS_HPP

# define RPL_TOPIC(client, channel, topic) (":ircserv 332 " + client + " " + channel + " :" + topic)
# define RPL_NAMREPLY(client, symbol, channel, names) (":ircserv 353 " + client + " " + symbol + " " + channel + " :" + names)
# define RPL_ENDOFNAMES(client, channel) (":ircserv 366 " + client + " " + channel + " :End of /NAMES list")

# define ERR_NOORIGIN(client) (":ircserv 409 " + client + " :No origin specified")

# define ERR_NONICKNAMEGIVEN(client) (":ircserv 431 " + client + " :No nickname given")
# define ERR_ERRONEUSNICKNAME(client, nick) (":ircserv 432 " + client + " " + nick + " :Erroneus nickname")
# define ERR_NICKNAMEINUSE(client, nick) (":ircserv 433 " + client + " " + nick + " :Nickname is already in use")
// # define ERR_NICKCOLLISION(client, nick, user, host) (":ircserv 436 " + client + " " + nick + " :Nickname collision KILL from " + user + "@" + host)

# define ERR_NEEDMOREPARAMS(client, command) (":ircserv 461 " + client + " " + command " :Not enough parameters")
# define ERR_ALREADYREGISTERED(client) (":ircserv 462 " + client + " :You may not register")
# define ERR_PASSWDMISMATCH(client) (":ircserv 464 " + client + " :Password incorrect")

# define ERR_INVITEONLYCHAN(client, channel) (":ircserv 473 " + client + " " + channel + " :Cannot join channel (+i)")
# define ERR_BADCHANNELKEY(client, channel) (":ircserv 475 " + client + " " + channel + " :Cannot join channel (+k)")
# define ERR_BADCHANMASK(client, channel) (":ircserv 476 " + client + " " + channel + " :Bad Channel Mask")

#endif