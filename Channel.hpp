#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>

class Channel;
#include "Client.hpp"

class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::set<int> _member;

public:
	Channel();
	Channel(const Channel &c);
	Channel(std::string name);
	Channel &operator=(const Channel &c);
	~Channel();

	bool isMember(int i);
	void sendChannelMsg(std::map<int, Client> &clients, std::string msg);
	void memberJoin(Client &cli);
	void memberLeave(std::map<std::string, Channel> &channel, int client_idx);

	std::set<int> &member(void);
	int memberCnt(void);
	std::string name();
};

#endif