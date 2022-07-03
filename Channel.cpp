#include "Channel.hpp"

using std::string;
using std::set;
using std::map;

Channel::Channel() {}

Channel::Channel(const Channel& c)
{
	*this = c;
}

Channel::Channel(string name) : _name(name) {}

Channel& Channel::operator=(const Channel& c)
{
	_name = c._name;
	_topic = c._topic;
	_member = c._member;
	return *this;
}

Channel::~Channel() {}

bool Channel::isMember(int i)
{
	return _member.find(i) != _member.end();
}

void Channel::sendChannelMsg(std::vector<Client> clients, int i, string msg, int self)
{
	for (set<int>::iterator itr = _member.begin(); itr != _member.end(); ++itr)
		if (self || i != *itr)
			clients[*itr].sendMsg(msg);
}

void Channel::memberJoin(int client_idx)
{
	_member.insert(client_idx);
}

void Channel::memberLeave(std::map<string, Channel>& channel, int client_idx)
{
	// not member
	if (!isMember(client_idx))
		return ;
	
	// remove from member list
	_member.erase(client_idx);

	// if channel empty, remove channal
	if (_member.size() == 0)
		channel.erase(_name);
}

set<int>& Channel::member(void)
{
	return _member;
}
std::string Channel::name(void)
{
	return _name;
}