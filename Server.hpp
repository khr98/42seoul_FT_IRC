#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include <set>
# include <vector>
# include <queue>
# include <cstring>
# include <cstdlib>
# include <string>
# include <algorithm>

# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <unistd.h>
# include <queue>

#define CLIENT_MAX 1000
#define BUFFER_SIZE 600
#define MAX_MESSAGE_LENGTH 512
#define CHANNEL_PREFIX "#&+!"

# include "Client.hpp"
# include "Channel.hpp"

class Server
{
public:
	typedef std::string::size_type s_size;

private:

	// server info
	std::string _host;
	std::string _servername;
	std::string _date;
	std::string _version;
	std::string _available_user_modes;
	std::string _available_channel_modes;

	// server setting
	const int port;
	const std::string passwd;

	// initial value
	const std::string oper_name;
	const std::string oper_pw;
    
	// client info
	std::map<std::string, int> client_names;
	std::map<std::string, Channel> channels;
	std::map<int, Client> clients;

	// poll management
	struct pollfd poll_fds[CLIENT_MAX];
    std::queue<int> ret_idxs;
	int max_index;

	// socket info
	struct sockaddr_in server_addr, client_addr;
	socklen_t clientaddr_len;
	int server_socket;

	char buf[BUFFER_SIZE + 1];


public:
	Server(int port, std::string pw);
	~Server();

	int setup();
	int run();

	// add client 
	void addNewClient(int client_socket);

	// poll ft
	bool processServerPoll();
	bool processClientPoll(int poll_ret);

	// process cmd
	void cmd(Client &  cli);
	void serverResponse(Client &  cli, std::vector<std::string> & arg);

	// TODO: following fts

	void pass(Client &  cli, std::vector<std::string> & arg);
	void nick(Client &  cli, std::vector<std::string> & arg);
	void user(Client &  cli, std::vector<std::string> & arg);
	void join(Client &  cli, std::vector<std::string> & arg);
	void oper(Client &  cli, std::vector<std::string> & arg);
	void privmsg(Client &  cli, std::vector<std::string> & arg);
	
	// void quit();	
	// void privmsg();
	// void squit();
	// void connect();
	// void kill();

	// 안해도 될 듯
	// void part();
	// void names();
	// void kick();
	

	bool isChannel(Client & cli,char c, std::string pool);

	std::string serverReponse(Client& cli, int code);

	class IRC_ERR : public std::exception
	 {
	// private: 
	// 	int code;
	public:
		//IRC_ERR(int code): code(code){};
		virtual const char* what() const throw();
	};

	class ERR_IRC: public std::exception
	{
	private:
		int _code;
	public:
		ERR_IRC(int code): _code(code){};
		virtual const char* what() const throw();
	};
	

};

#endif
