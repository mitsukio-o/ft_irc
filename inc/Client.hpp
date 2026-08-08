// A's part

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Irc.hpp"

class Client
{
public:
	Client(int fd, const std::string& host);
	~Client();

	int					getFd() const;
	const std::string&	getNick() const;
	const std::string&	getUser() const;
	std::string			getPrefix() const;
	bool				gotPass() const;
	bool				isRegistered() const;
	bool				isQuitting() const;

	void				setNick(const std::string& nick);
	void				setUser(const std::string& user);
	void				setPass();
	void				setRegistered();
	void				setQuitting();

	void				store(const char* data, size_t size);
	bool				isFlooded() const;
	bool				nextLine(std::string& line);

	void				push(const std::string& message);
	std::string&		pending();

private:
	int			_fd;
	std::string	_host;
	std::string	_nick;
	std::string	_user;
	std::string	_buffer;
	std::string	_pending;
	bool		_gotPass;
	bool		_registered;
	bool		_quitting;

	Client(const Client& other);
	Client&	operator=(const Client& other);
};

#endif
