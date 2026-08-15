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
const std::set<Client*>&		Channel::getMembers() const		{ return (_members); }

void	Channel::setTopic(const std::string& topic)	{ _topic = topic; }
void	Channel::setKey(const std::string& key)		{ _key = key; }
void	Channel::setLimit(size_t limit)				{ _limit = limit; }
void	Channel::setInviteOnly(bool on)				{ _inviteOnly = on; }
void	Channel::setTopicLocked(bool on)			{ _topicLocked = on; }

// returns itky (invite, topic lock, password, limit)
std::string	Channel::getModes() const
{
	std::string modes;

	if (_inviteOnly)
		modes += "i";
	if (_topicLocked)
		modes += "t";
	if (!_key.empty())
		modes += "k";
	if (_limit != 0)
		modes += "l";
	return (modes);
}

// returns std::string of nicknames
std::string	Channel::getNames() const
{
	std::string	names;
	std::set<Client*>::iterator	it;

	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if (it != _members.begin())
			names += " ";
		if (isOperator(**it))
			names += "@";
		names += (*it)->getNick();
	}
	return (names);
}


// finds if client is a member of the channel
bool	Channel::has(Client& client) const
{
	if (_members.find(&client) == _members.end())
		return (false);
	return (true);
}

// finds if client is an operator of the channel
bool	Channel::isOperator(Client& client) const
{
	if (_operators.find(&client) == _operators.end())
		return (false);
	return (true);
}

// finds if client is invited to the channel
bool	Channel::isInvited(Client& client) const
{
	if (_invited.find(&client) == _invited.end())
		return (false);
	return (true);
}

Client*	Channel::find(const std::string& nick) const
{
	std::set<Client*>::iterator	it;

	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if (toLower((*it)->getNick()) == toLower(nick))
			return (*it);
	}
	return (NULL);
}

void	Channel::join(Client* client, bool asOperator)
{
	_members.insert(client);
	_invited.erase(client);
	if (asOperator)
		_operators.insert(client);
}

void	Channel::leave(Client& client)
{
	_members.erase(&client);
	_operators.erase(&client);
	_invited.erase(&client);
}

void	Channel::invite(Client& client)
{
	_invited.insert(&client);
}

void	Channel::grant(Client& client, bool on)
{
	if (on)
		_operators.insert(&client);
	else
		_operators.erase(&client);
}

void	Channel::broadcast(const std::string& message, Client* except)
{
	std::set<Client*>::iterator	it;

	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it != except)
			(*it)->push(message);
	}
}
