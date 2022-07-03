#include "Server.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::set;
using std::make_pair;
using std::vector;
using std::priority_queue;


Server::Server(int port, string pw): port(port), passwd(pw), oper_name("Anon"), oper_pw("1234"), clientaddr_len(sizeof(client_addr)) {}

Server::~Server()
{
	for (int i = 0; i < CLIENT_MAX; ++i)
		if (poll_fds[i].fd != -1)
			close(poll_fds[i].fd);
}

int Server::setup()
{
    // CREATE SOCKET
    server_socket = socket(PF_INET, SOCK_STREAM, 0);

    // CREATE SOCKET ERROR
	if (server_socket < 0)
	{
		cerr << "Cannot create a socket" << endl;
		return 1;
	}

    // SET SOCKET OPTION
	int op = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
	bzero(&server_addr, sizeof(server_addr));

    // SET SOCKET ADDRESS
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

    // BIND
    int resultBind = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (resultBind < 0)
	{
		cerr << "Cannot bind name to socket" << endl;
		return 1;
	}

    // LISTEN
    int resultListen = listen(server_socket, 5);
	if (resultListen < 0)
	{
		cerr << "Listen error" << endl;
		return 1;
	}

    // SET SERVER FD FOR CHECK POLL
	poll_fds[0].fd = server_socket;
	poll_fds[0].events = POLLIN;

	for (int i = 1; i < CLIENT_MAX; ++i)
	{
		poll_fds[i].fd = -1;
	}
	return 0;
}



int Server::run()
{
	max_index = 1;
	while (1)
	{
        // SET POLL
		int poll_ret = poll(poll_fds, max_index, -1);
        if (poll_ret < 0)
        {
            cerr << "Poll error" << endl;
            return 1;
        }
        if (poll_ret == 0)
			continue ;

        // CHECK POLL EVENT ON SERVER (EVENT TRIGGERD WHEN CLIENT JOIN)
		if (poll_fds[0].revents & POLLIN)
		{
			if(!processServerPoll())
				return 1;
			continue ;
		}

        // CHECK ALL CLIENT POLL FD 
		if(!processClientPoll(poll_ret))
			return 1;
	}
}

void Server::addNewClient(int client_socket){
    // FULL
    if(ret_idxs.empty() && max_index >= CLIENT_MAX){
        cout << "client Full..." <<  endl;
        return;
    }

    // find available idx
    int idx;
    if(ret_idxs.empty() == false){
        idx = ret_idxs.front();
        ret_idxs.pop();
    }
    else
        idx = max_index++;

    // create client_fd and client
    poll_fds[idx].fd = client_socket;
    poll_fds[idx].events = POLLIN;
	clients.push_back(Client(idx, client_socket));


	// user info
    cout << "client connection success " << "\U00002714"<< endl;
	cout << "- client [" << idx <<"] : "  << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
	cout << endl;
}

bool Server::processServerPoll()
{
	int client_socket;
	bzero(&client_addr, sizeof(client_addr));

	// SET CLIENT SOCKET
	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &clientaddr_len);
	if (client_socket < 0)
	{
		cerr << "accept error " << "\U0000274C" << endl;
		return false;
	}

	// CREATE CLIENT AND PUSH CLIENT LIST
	addNewClient(client_socket);
	return true;
}

bool Server::processClientPoll(int poll_ret)
{
        vector<Client>::iterator iter = clients.begin();
		for(; iter != clients.end() && poll_ret > 0; iter++)
		{
            // CHECK IS EVENT CLIENT
            Client cli = (*iter);

			// if poll fd did not get event
            int idx = cli.idx();
			if (!(poll_fds[idx].revents & POLLIN))
				continue;

            // GET DATA
			int recv_size = recv(poll_fds[idx].fd, buf,sizeof(buf), 0);

			if (recv_size < 0) // RECV ERR
			{
				cerr << "recv error" << endl;
				return false;
			}
			else if (recv_size > MAX_MESSAGE_LENGTH) // MSG ERR
			{
				cli.sendMsg("Message length NOT supported!");
			}
			else if (recv_size == 0 ) // LEAVE
			{
                // CLOSE CLIENT FD
				cli.closeFd();
                // LEAVE FROM ALL CHANNEL
				cli.leaveAllChannel(channels);

                // RESET POLLFD
				poll_fds[idx].events = 0;
				poll_fds[idx].fd = -1;
                ret_idxs.push(idx);

                // SEND LEAVE MSG

    			cout << "client connection lost " << "\U00002708"<< endl;
				cout << "- "<< cli.nickname() << ((cli.nickname().length() > 0) ? ": ": "") << "client [" << idx << "] left" << endl;
				cout << endl;

                // REMOVE NICKNAME FROM OCCUPIED NICKNAME LIST
				client_map.erase(cli.nickname());
			}
			else // CMD
			{
				buf[recv_size] = 0;
				cli.setMsg(buf);
				cmd(cli, cli.msg().find_first_of("\r\n"));
			}

			// decrese event poll count because proccess done
			poll_ret--;
		}
		return true;
}
void Server::cmd(Client cli, string::size_type nl_index)
{
	// rm newline
    cout << "CMD from [client "<< cli.idx() <<"]: " << cli.msg().substr(0, nl_index) <<" (" << nl_index << ")"<< endl;
}
