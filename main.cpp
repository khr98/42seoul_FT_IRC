#include "Server.hpp"

int main(int argc, char **argv)
{
	// check args
	if (argc < 3 || argc > 4)
	{
		std::cerr << "check args: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	// check port is number
	for (int i = 0; argv[1][i]; ++i)
		if (!isdigit(argv[1][i]))
		{
			std::cout << "wrong port number" << std::endl;
			return 1;
		}

	// setup server
	Server irc_server(atoi(argv[argc - 2]), argv[argc - 1]);
	if (irc_server.setup())
		return 1;

	// start server
	irc_server.run();
}