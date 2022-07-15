#include "Server.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::set;
using std::make_pair;
using std::vector;
using std::priority_queue;

std::vector<std::string> split(std::string input) {
	std::vector<std::string>  tokens;
	Server::s_size prev = 0;
	char separater = ' ';
	Server::s_size current = input.find(separater, prev);
	std::string last;
	
	// find ":"
	Server::s_size temp = input.find(':', prev);
	if(temp != std::string::npos){
		last = input.substr(temp, input.length() - temp);
		input = input.substr(0, temp);
	}

	// split extra space
	while (current != std::string::npos){
		tokens.push_back(input.substr(prev, current - prev));
		prev = current + 1;
		current = input.find(separater, prev);
	}
	if(prev < input.length())
		tokens.push_back(input.substr(prev, input.length()));

	// make command to upper case
	for(Server::s_size i = 0; i < tokens[0].length(); i ++)
		tokens[0][i] = std::toupper(tokens[0][i]);

	if(!last.empty())
		tokens.push_back(last);
	// for(Server::s_size i = 0; i < tokens.size(); i ++)
	// 	std::cout << tokens[i]<< std::endl;
    return tokens;
}

std::vector<std::string> splitNewLine(Client cli, std::string input) {

	std::vector<std::string>  tokens;
	Server::s_size prev = 0;
	std::string separater = "\n";
	Server::s_size current = input.find(separater, prev);
	
	while (current != std::string::npos){
		if(current - prev > 0)
			tokens.push_back(input.substr(prev, current - prev));
		prev = current + 1;
		current = input.find(separater, prev);
	}
	if(prev < input.length())
		tokens.push_back(input.substr(prev, input.length()));
	
	// test print to check input data
	for(Server::s_size i = 0; i < tokens.size(); i ++)
		std::cout << "* input from " << cli.nickname()<< " (USER " << cli.idx() << ") <= " <<"\"" << tokens[i]<<"\"" <<std::endl;
    return tokens;
}


Server::Server(int port, std::string pw): port(port), passwd(pw), oper_name("Admin"), oper_pw("Admin1234"), clientaddr_len(sizeof(client_addr)) {}

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

	// server info
	_host = inet_ntoa(server_addr.sin_addr);
	_servername = "myIRC";
	_date = "unkwon";
	_version = "1.0";

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
	std::string str = std::string(inet_ntoa(client_addr.sin_addr));
	Client cli = Client(idx, client_socket, str);
	clients[idx] = cli;


	// user info
    cout << "client connection success " << "\U00002714"<< endl;
	cout << "- client [" << cli.idx() <<"] : "  << cli.host() << ":" << ntohs(client_addr.sin_port) << endl;
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
        std::map<int,Client>::iterator iter = clients.begin();
		for(; iter != clients.end() && poll_ret > 0; iter++)
		{
            // CHECK IS EVENT CLIENT
            Client& cli = (iter->second);

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
				client_names.erase(cli.nickname());
			}

			else // CMD
			{
				buf[recv_size] = 0;
				for(int i = 0; i < recv_size ; i++){
					if(buf[i] == '\r')
						buf[i] = '\n';
				}

				std::vector<std::string> cmds = splitNewLine(cli, buf);
				std::vector<std::string>::iterator itr = cmds.begin();
				for (;itr != cmds.end(); itr++ )
				{
					if((*itr).empty())
						continue;
					cli.setMsg(*itr);
					cmd(cli);
				}
			}

			// decrese event poll count because proccess done
			poll_ret--;
		}
		return true;
}


bool isInclude(char c, std::string str)
{
	 return str.find(c) != std::string::npos;
}

bool checkNick(std::string str)
{ //TODO : need more condition
	if(str.length() > 9)
		return false;
	for(std::string::size_type i = 0; i < str.length() ; i++)
		if (!(std::isalpha(str[i]) || std::isdigit(str[i])) && !isInclude(str[i], ";[]\\`_^{|}"))
			return false;

	return true;
}

void Server::cmd(Client & cli)
{	
	try{
	// rm newline 
	std::vector<std::string>  tokens = split(cli.msg());

	if(tokens[0] == "PASS")
		pass(cli, tokens);
	else if(tokens[0] == "NICK")
		nick(cli, tokens);
	else if(tokens[0] == "USER")
		user(cli, tokens);
	else if(tokens[0] == "JOIN")
		join(cli, tokens);
	else if(tokens[0] == "OPER")
		oper(cli, tokens);
	else if(tokens[0] == "PRIVMSG")
		privmsg(cli, tokens);
	} 
	catch (const std::exception& e) {
    cout << e.what()<< endl;
	}
}


void Server::pass(Client & cli, std::vector<std::string> & arg){
	if (arg.size() < 2) //ERR_NEEDMOREPARAMS
		throw ERR_IRC(461);
	if (cli.isAuth()) //ERR_ALREADYREGISTRED
		throw ERR_IRC(462);
	cli.setAuth(arg[1], passwd);
}

void Server::nick(Client & cli, std::vector<std::string> & arg){

	if (arg.size() < 2) //ERR_NONICKNAMEGIVEN
		throw ERR_IRC(431);
	if (!checkNick(arg[1])) //ERR_ERRONEUSNICKNAME
		throw ERR_IRC(432);
	if (client_names.find(arg[1]) != client_names.end()) //ERR_NICKNAMEINUSE
		throw ERR_IRC(433);
	// if () //ERR_NICKCOLLISION
	// 	throw ERR_IRC(436);
	// if () //ERR_UNAVAILRESOURCE
	// 	throw ERR_IRC(437);
	// if () //ERR_RESTRICTED
	// //Sent by the server to a user upon connection to indicate  the restricted nature of the connection (user mode "+r")
	// 	throw ERR_IRC(484);
	cli.setNick(client_names, arg[1]);
}

void Server::user(Client & cli, std::vector<std::string> & arg){
	if (arg.size() < 5) //ERR_NEEDMOREPARAMS
		throw ERR_IRC(461);
	if (cli.isReg()) //ERR_ALREADYREGISTRED
		throw ERR_IRC(462);
	cli.setUsername(arg);
	client_names[arg[1]] = cli.idx();
}

void Server::join(Client & cli, std::vector<std::string> & arg){
	std::vector<std::string>::iterator itr = ++(arg.begin());
	for (; itr != arg.end(); itr++)
	{
		if (channels.find(*itr) == channels.end())
			channels[*itr] = Channel(*itr);
		channels[*itr].memberJoin(cli);
		channels[*itr].sendChannelMsg(clients, ":"+ cli.nickname()+"!"+cli.username()+"@"+cli.host() + " JOIN " + *itr +"\n");
	
	}
}

void Server::privmsg(Client & cli, std::vector<std::string> & arg){
	//채널메세지일경우
	//있는 채널일경우
	//없는 채널일경우
	if (arg.size() < 2)
		throw ERR_IRC(461);
	std::vector<std::string>::iterator itr = ++(arg.begin());
	for (; itr != --arg.end(); itr++)
	{
		if (isChannel(cli,itr->at(0), CHANNEL_PREFIX))
		{
			cli.sendMsg("this is channel");
			if (channels.find(*itr) == channels.end())
				throw ERR_IRC(404);
			channels[*itr].sendChannelMsg(clients, ":" + cli.nickname()+"!"+cli.username()+"@"+cli.host() + "PRIVMSG" + *itr + "\n");
		}
		else
		{
			//nick 메세지 일경우
			//있는 유저일경우
			//없는 유저일경우
			cli.sendMsg("is not channel");
			// if (client_map.find(*itr) == client_map.end())
			// 	throw ERR_NOSUCHNICK();
			// client[client_map[*itr]].sendMsg(client[i].prefix() + client[i].message() + '\n');
		}
	}
}

void Server::oper(Client & cli, std::vector<std::string> & arg){
	if (arg.size() != 3)
		throw ERR_IRC(461); ////ERR_NEEDMOREPARAMS
	if (oper_name != arg[1])
		throw ERR_IRC(491);//ERR_NOOPERHOST
	if (oper_pw != arg[2])
		throw ERR_IRC(464);//ERR_PASSWDMISMATCH
	cli.setOper();
	if (cli.isOper())
		cli.sendMsg(":You are now an IRC operator\n");
}



# include<sstream>
std::string Server::serverReponse(Client& cli, int code){
	std::ostringstream ss;
	ss<< ":" << _host << " " << code  << " ";


	switch (code)
	{
	case 1: // RPL_WELCOME
		 ss <<":Welcome to the Internet Relay Network "<< cli.nickname()<<"!"<<cli.username()<<"@"<<cli.host();
		break;
	case 2: // RPL_YOURHOST
		ss << "Your host is "<<_servername<<", running version "<< _version;
		break;
	case 3: // RPL_CREATED
		ss << "This server was created " << _date;
		break;
	case 4: // RPL_MYINFO
		ss << _servername << " " << _version << " "  << _available_user_modes << " " << _available_channel_modes;
		break;
	default:
		return "";
	}
	return ss.str();
}


const char* Server::ERR_IRC::what() const throw()
{
	switch (_code)
	{
		case 403: //ERR_NOSUCHCHANNEL
			//arg : channel 
			return " :No such channel";
		
		case 404:
			return " :Cannot send to channel";
		
		case 405://  ERR_TOOMANYCHANNELS
			//arg : channel 
			return " :You have joined too many channels";
		case 407://  ERR_TOOMANYTARGETS
			//arg : target 
			return " :407 recipients.";
			
		case 431: //  ERR_NONICKNAMEGIVEN 
			return ":No nickname given";
		case 432://  ERR_ERRONEUSNICKNAME  
			//arg : nick 
			return " :Erroneous nickname";
		case 433://  ERR_NICKNAMEINUSE 
			//arg : nick 
			return " :Nickname is already in use";
		case 436://  ERR_NICKCOLLISION
			//arg : nick ,user@host
			return " :Nickname collision KILL from ";
		case 437://  ERR_UNAVAILRESOURCE 
			//arg : nick/channel
			return " :Nick/channel is temporarily unavailable";

		case 464: // ERR_PASSWDMISMATCH
			return " :Wrong host password";

		case 461://  ERR_NEEDMOREPARAMS
			//arg : CMD
			return " :Not enough parameters";
		case 462://  ERR_ALREADYREGISTRED
			return ":Unauthorized command (already registered)";

		case 471: // 471 ERR_CHANNELISFULL
			//arg : channel
			return " :Cannot join channel (+l)";
		case 473:// 473 ERR_INVITEONLYCHAN
			//arg : channel
			return " :Cannot join channel (+i)";
		case 474: // 474 ERR_BANNEDFROMCHAN
			//arg : channel
			return " :Cannot join channel (+b)";
		case 475: // 475 ERR_BADCHANNELKEY
			//arg : channel
			return " :Cannot join channel (+k)";
		case 476: // 476 ERR_BADCHANMASK
			//arg : channel
			return " :Bad Channel Mask";
		case 484: // 484 ERR_RESTRICTED
			return " :Your connection is restricted!";

		case 491: //491 ERR_NOOPERHOST
			return " :Wrong Host name!";
		default:
			return "error\n";
	}
}

bool Server::isChannel(Client & cli,char c, std::string pool)
{
	std::string s(1, c);
	cli.sendMsg(s);
	return (pool.find(c) != std::string::npos);
}