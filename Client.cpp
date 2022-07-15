#include "Client.hpp"

Client::Client() : _isAuth(false), _isReg(false), _nickname(""), _username(""), _msg(""), _idx(-1), _fd(-1), _mode(0) {}

Client::Client(int idx, int fd, std::string host) : _isAuth(false),
													_isReg(false), _nickname(""), _username(""), _msg(""), _idx(idx), _fd(fd), _mode(0)
{
	_host = host;
}

Client::Client(const Client &cli)
{
	*this = cli;
}

Client::~Client() {}

// set

Client &Client::operator=(const Client &cli)
{
	_isAuth = cli._isAuth;
	_isReg = cli._isReg;
	_nickname = cli._nickname;
	_username = cli._username;
	_msg = cli._msg;
	_idx = cli._idx;
	_fd = cli._fd;
	_mode = cli._mode;
	_host = cli._host;
	return *this;
}

void Client::setAuth(std::string passwd, std::string serverPw)
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
		sendMsg(":Welcome to the Internet Relay Network " + _nickname + "!" + _username + "@" + _host + "\n");
	}
}

void Client::setUsername(std::vector<std::string> &args)
{
	_username = args[1];

	if (_isAuth && _nickname != "" && _username != "")
	{
		_isReg = true;
		sendMsg("reg\n");
		sendMsg(":Welcome to the Internet Relay Network " + _nickname + "!" + _username + "@" + _host + "\n");
	}
}

void Client::setNick(std::map<std::string, int> &client_map, std::string arg)
{
	if (_nickname != "")
		client_map.erase(_nickname);
	_nickname = arg;
	client_map[arg] = _idx;
	if (_isAuth && _nickname != "" && _username != "")
	{
		_isReg = true;
		sendMsg("reg\n");
		sendMsg(":Welcome to the Internet Relay Network " + _nickname + "!" + _username + "@" + _host + "\n");
	}
}

// ft

void Client::sendMsg(std::string msg, int flag)
{
	std::cout << "# msg to " << _nickname << " (USER " << _idx << ") => " << msg;
	send(_fd, msg.c_str(), msg.size(), flag);
}

void Client::setMsg(std::string buf)
{
	_msg.clear();
	_msg += buf;
}

void Client::joinChannel(std::string channel_name)
{
	_joinedChannel.insert(channel_name);
}

void Client::leaveChannel(std::map<std::string, Channel> &channels, Channel &channel)
{
	_joinedChannel.erase(channel.name());
	channel.memberLeave(channels, _idx);
}

void Client::leaveAllChannel(std::map<std::string, Channel> &channels)
{
	while (!_joinedChannel.empty())
	{
		Channel channel = channels[*_joinedChannel.begin()];
		_joinedChannel.erase(channel.name());
		channel.memberLeave(channels, _idx);
	}
}
void Client::leaveServer(std::map<std::string, Channel> &channels)
{
	leaveAllChannel(channels);
	closeFd();
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

std::string Client::msg(void)
{
	return _msg;
}

std::set<std::string> &Client::joinedChannel(void)
{
	return _joinedChannel;
}

std::string Client::nickname(void)
{
	return _nickname;
}
std::string Client::username(void)
{
	return _username;
}

int Client::idx(void)
{
	return _idx;
}

int Client::fd(void)
{
	return _fd;
}

int Client::mode(void)
{
	return _mode;
}
int Client::channelCnt(void)
{
	return _joinedChannel.size();
}

std::string Client::host(void)
{

	return _host;
}