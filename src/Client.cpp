// A's part

#include "Client.hpp"

static const size_t	kMaxLine = 65536;

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
std::string&		Client::pending()				{ return (_pending); }

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
	_buffer.append(data, size);
}

// 見るのは受信側だけ
bool	Client::isFlooded() const
{
	return (_buffer.size() > kMaxLine);
}

// '\n' までを 1 行として取り出し、余りは _buffer に残す
bool	Client::nextLine(std::string& line)
{
	const std::string::size_type	end = _buffer.find('\n');

	if (end == std::string::npos)
		return (false);
	line = _buffer.substr(0, end);
	_buffer.erase(0, end + 1);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	return (true);
}

// キューに積む。POLLOUTが立ったら実際送る
void	Client::push(const std::string& message)
{
	_pending += message + "\r\n";
}
