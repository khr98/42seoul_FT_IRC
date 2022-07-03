#include "Server.hpp"

using std::cout;
using std::cin;
using std::endl;

int main(int argc, char **argv)
{
	// check args
	if (argc < 3 || argc > 4)
	{
		std::cerr << "check args: ./ircserv <port> <password>" << endl;
		return 1;
	}

	// check port is number
	for (int i = 0; argv[1][i]; ++i)
		if (!isdigit(argv[1][i]))
		{
			cout << "wrong port number" << endl;
			return 1;
		}

	// setup server
	Server irc_server(atoi(argv[argc - 2]), argv[argc - 1]);
	if (irc_server.setup())
		return 1;

	// start server
	irc_server.run();
}