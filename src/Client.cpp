// A's part

#include "Client.hpp"

Client::Client(int fd, const std::string& host)
	: _fd(fd), _host(host), _nick("*"), _user(), _buffer(), _pending(),
	  _gotPass(false), _registered(false), _quitting(false)
{
}

Client::~Client()	{}

int					Client::getFd() const			{ return (_fd); }
const std::string&	Client::getNick() const			{ return (_nick); }
const std::string&	Client::getUser() const			{ return (_user); }
const std::string&	Client::getReal() const			{ return (_real); }
bool				Client::gotPass() const			{ return (_gotPass); }
bool				Client::isRegistered() const	{ return (_registered); }
bool				Client::isQuitting() const		{ return (_quitting); }
bool				Client::pending() const			{ return (!_pending.empty()); }
std::string&		Client::buffer()				{ return (_buffer); }

void	Client::setNick(const std::string& nick)	{ _nick = nick; }
void	Client::setUser(const std::string& user)	{ _user = user; }
void	Client::setReal(const std::string& real)	{ _real = real; }
void	Client::setPass()						{ _gotPass = true; }
void	Client::setRegistered()					{ _registered = true; }
void	Client::setQuitting()					{ _quitting = true; }

std::string	Client::getPrefix() const
{
	return (_nick + "!" + _user + "@" + _host);
}

void	Client::store(const char* data, size_t size)
{
	(void)data;
	(void)size;
}

bool	Client::isFlooded() const
{
	return (false);
}

bool	Client::nextLine(std::string& line)
{
	(void)line;
	return (false);
}

void	Client::push(const std::string& message)
{
	(void)message;
}
