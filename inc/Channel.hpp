// B's part

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include <set>
# include <vector>

class Channel
{
public:
	explicit Channel(const std::string& name);
	~Channel();

	const std::string&				getName() const;
	const std::string&				getTopic() const;
	const std::string&				getKey() const;
	size_t							getLimit() const;
	bool							isInviteOnly() const;
	bool							isTopicLocked() const;
	std::string						getModes() const;
	std::string						getNames() const;
	const std::set<const Client*>&	getMembers() const;

	bool	isEmpty() const;
	bool	has(const Client& client) const;
	bool	isOperator(const Client& client) const;
	bool	isInvited(const Client& client) const;
	const Client*	find(const std::string& nick) const;

	void	setTopic(const std::string& topic);
	void	setKey(const std::string& key);
	void	setLimit(size_t limit);
	void	setInviteOnly(bool on);
	void	setTopicLocked(bool on);

	void	join(Client* client, bool asOperator);
	void	leave(const Client& client);
	void	invite(const Client& client);
	void	grant(const Client& client, bool on);
	void	broadcast(const std::string& message, const Client* except = 0) const;

private:
	std::string				_name;
	std::string				_topic;
	std::string				_key;
	size_t					_limit;
	bool					_inviteOnly;
	bool					_topicLocked;
	std::set<const Client*>	_members;
	std::set<const Client*>	_operators;
	std::set<const Client*>	_invited;

	Channel(const Channel& other);
	Channel&	operator=(const Channel& other);
};

#endif
