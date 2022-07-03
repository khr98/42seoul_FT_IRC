#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <map>
# include <set>
# include <unistd.h>
# include <iostream>
# include <sys/socket.h>
# define MODE_i 8
# define MODE_o 1

class Client;
# include "Channel.hpp"

class Client
{
private:
	std::set<std::string> _joinedChannel;
	bool _isAuth;
	bool _isReg;
	std::string _nickname;
	std::string _username;
	std::string _msg;
	int _idx;
	int _fd;
	int _mode;
	

public:
	Client();
	Client(int idx, int fd);
	Client(const Client& a);
	~Client();
	Client& operator=(const Client& a);

	// set
	void setAuth(std::string passwd, std::string serverPw);
	void setNick(std::map<std::string, int>& client_map, std::string nic);
	void setUsername(std::vector<std::string> args);
	void setMsg(char *buf);

	// get
	bool isReg(void);
	bool isAuth(void);
	std::string msg(void);
	std::set<std::string>& joinedChannel(void);
	std::string nickname(void);
	int idx(void);
	int fd(void);
	int mode(void);

	// ft
	void sendMsg(std::string msg, int flag=0);
	void joinChannel(Channel channel);
	void leaveChannel(std::map<std::string, Channel> channels, Channel channel);
	void leaveAllChannel(std::map<std::string, Channel> channels);
	void closeFd();

};

#endif