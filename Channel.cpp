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

void Channel::sendChannelMsg(std::map<int, Client> & clients, string msg)
{

	set<int>::iterator itr = _member.begin();

	for (; itr != _member.end(); itr++){
			clients[*itr].sendMsg(msg);
		}
}

void Channel::memberJoin(Client &cli)
{
	// std::set<int>::iterator itr = _member.begin();
	// for(;itr!= _member.end(); itr++)

	_member.insert(cli.idx());
	cli.joinChannel(_name);


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