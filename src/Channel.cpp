// B's part

#include "Channel.hpp"

Channel::Channel(const std::string& name)
	: _name(name), _topic(), _key(), _limit(0), _inviteOnly(false),
	  _topicLocked(false), _members(), _operators(), _invited()
{
}

Channel::~Channel()	{}

const std::string&				Channel::getName() const		{ return (_name); }
const std::string&				Channel::getTopic() const		{ return (_topic); }
const std::string&				Channel::getKey() const			{ return (_key); }
size_t							Channel::getLimit() const		{ return (_limit); }
bool							Channel::isInviteOnly() const	{ return (_inviteOnly); }
bool							Channel::isTopicLocked() const	{ return (_topicLocked); }
bool							Channel::isEmpty() const		{ return (_members.empty()); }
const std::set<const Client*>&	Channel::getMembers() const		{ return (_members); }

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


// finds if client is a member of the channel
bool	Channel::has(const Client& client) const
{
	if (_members.find(&client) == _members.end())
		return (false);
	return (true);
}

// finds if client is an operator of the channel
bool	Channel::isOperator(const Client& client) const
{
	if (_operators.find(&client) == _members.end())
		return (false);
	return (true);
}

// finds if client is invited to the channel
bool	Channel::isInvited(const Client& client) const
{
	if (_invited.find(&client) == _members.end())
		return (false);
	return (true);
}

const Client*	Channel::find(const std::string& nick) const
{
	std::set<const Client*>::iterator	it;

	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if ((*it)->getNick() == nick)
			return (*it);
	}
	return (NULL);
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
