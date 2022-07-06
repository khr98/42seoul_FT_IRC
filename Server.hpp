#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include <set>
# include <vector>
# include <queue>
# include <cstring>
# include <cstdlib>

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
private:
	// server setting
	const int port;
	const std::string passwd;

	// initial value
	const std::string oper_name;
	const std::string oper_pw;
    
	// client info
	std::map<std::string, int> client_map;
	std::map<std::string, Channel> channels;
	std::vector<Client> clients;

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
	void cmd(Client cli, std::string::size_type nl_index);

	// TODO: following fts

	// void pass();
	// void nick();
	// void user();
	// void join();


	// void privmsg();
	// void oper();
	// void squit();
	// void connect();
	// void kill();

	// 안해도 될 듯
	// void mode();
	// void part();
	// void names();
	// void kick();
	// void quit();
	
};

#endif