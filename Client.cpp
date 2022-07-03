#include "Client.hpp"

using std::string;
using std::vector;
using std::map;
using std::set;

Client::Client() : _isAuth(false), _isReg(false), _nickname(""), _username(""), _msg(""), _idx(-1), _fd(-1), _mode(0) {}

Client::Client(int idx, int fd) : _isAuth(false),
 _isReg(false), _nickname(""), _username(""), _msg(""), _idx(idx), _fd(fd), _mode(0)
  {}

Client::Client(const Client& cli)
{
	*this = cli;
}

Client::~Client() {}

// set

Client& Client::operator=(const Client& cli)
{
	_isAuth = cli._isAuth;
	_isReg = cli._isReg;
	_nickname = cli._nickname;
	_username = cli._username;
	_msg = cli._msg;
	_idx = cli._idx;
	_fd = cli._fd;
	_mode = cli._mode;
	return *this;
}

void Client::setAuth(string passwd, string serverPw)
{
	if (passwd == serverPw)
	{
		sendMsg("auth\n");
		_isAuth = true;
	}
	else
		sendMsg("wrong password\n");

	if (_isAuth && _nickname != "" && _username != "")
	{
		_isReg = true;
		sendMsg("reg\n");
	}
}

void Client::setNick(map<string, int>& client_map, string arg)
{
	if (_nickname != "")
		client_map.erase(_nickname);
	_nickname = arg;
	client_map[arg] = _idx;
	if (_isAuth && _nickname != "" && _username != "")
	{
		_isReg = true;
		sendMsg("reg\n");
	}
}

void Client::setUsername(vector<string> args)
{
	_username = args[0];
	if (_isAuth && _nickname != "" && _username != "")
	{
		_isReg = true;
		sendMsg("reg\n");
	}
}


// ft

void Client::sendMsg(string msg, int flag = 0)
{
	std::cout << "to " << _idx << ':' << msg;
	send(_fd, msg.c_str(), msg.size(), flag);
}

void Client::setMsg(char *buf)
{
	_msg[0] = '\0';
	_msg += buf;
}

void Client::joinChannel(Channel channel)
{
	_joinedChannel.insert(channel.name());
	channel.memberJoin(_idx);
}

void Client::leaveChannel(std::map<std::string, Channel> channels, Channel channel)
{
	_joinedChannel.erase(channel.name());
	channel.memberLeave(channels, _idx);

}

void Client::leaveAllChannel(std::map<std::string, Channel> channels)
{
	while (!_joinedChannel.empty())
	{
		Channel channel = channels[*_joinedChannel.begin()];
		_joinedChannel.erase(channel.name());
		channel.memberLeave(channels, _idx);

	}

}
void Client::closeFd()
{
	close(_fd);
}

// get

bool Client::isReg(void)
{
	return _isReg;
}

bool Client::isAuth(void)
{
	return _isAuth;
}

string Client::msg(void)
{
	return _msg;
}


set<string>& Client::joinedChannel(void)
{
	return _joinedChannel;
}

string Client::nickname(void)
{
	return _nickname;
}

int  Client::idx(void)
{
	return _idx;
}

int  Client::fd(void)
{
	return _fd;
}

int Client::mode(void)
{
	return _mode;
}
