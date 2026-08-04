// B's part

#include "Channel.hpp"

Channel::Channel(const std::string& name)
	: _name(name), _topic(), _key(), _limit(0), _inviteOnly(false),
	  _topicLocked(false), _members(), _operators(), _invited()
{
}

Channel::~Channel()	{}

const std::string&			Channel::getName() const		{ return (_name); }
const std::string&			Channel::getTopic() const		{ return (_topic); }
const std::string&			Channel::getKey() const			{ return (_key); }
size_t						Channel::getLimit() const		{ return (_limit); }
bool						Channel::isInviteOnly() const	{ return (_inviteOnly); }
bool						Channel::isTopicLocked() const	{ return (_topicLocked); }
bool						Channel::isEmpty() const		{ return (_members.empty()); }
const std::vector<Client*>&	Channel::getMembers() const		{ return (_members); }

void	Channel::setTopic(const std::string& topic)	{ _topic = topic; }
void	Channel::setKey(const std::string& key)		{ _key = key; }
void	Channel::setLimit(size_t limit)				{ _limit = limit; }
void	Channel::setInviteOnly(bool on)				{ _inviteOnly = on; }
void	Channel::setTopicLocked(bool on)			{ _topicLocked = on; }

std::string	Channel::getModes() const
{
	return ("+");
}

std::string	Channel::getNames() const
{
	return ("");
}

bool	Channel::has(const Client& client) const
{
	(void)client;
	return (false);
}

bool	Channel::isOperator(const Client& client) const
{
	(void)client;
	return (false);
}

bool	Channel::isInvited(const Client& client) const
{
	(void)client;
	return (false);
}

Client*	Channel::find(const std::string& nick) const
{
	(void)nick;
	return (0);
}

void	Channel::join(Client* client, bool asOperator)
{
	(void)client;
	(void)asOperator;
}

void	Channel::leave(const Client& client)
{
	(void)client;
}

void	Channel::invite(const Client& client)			{ (void)client; }
void	Channel::grant(const Client& client, bool on)	{ (void)client; (void)on; }

void	Channel::broadcast(const std::string& message, const Client* except) const
{
	(void)message;
	(void)except;
}
