#include "Utils.hpp"

std::vector<std::string> tokenizeCmd(std::string input) {
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
	// 	std::cout <<"\""<< tokens[i]<< "\""<< std::endl;
    return tokens;
}

std::vector<std::string> splitNewLine(Client cli, std::string input) {

	for(std::string::size_type i = 0; i < input.size() ; i++){
		if(input[i] == '\r')
			input[i] = '\n';
	}

	std::vector<std::string> tokens;
	Server::s_size prev = 0;
	std::string separater = "\n";
	Server::s_size current = input.find(separater, prev);

	while (current != std::string::npos){
		if(current - prev > 0)
			tokens.push_back(input.substr(prev, current - prev));
		prev = current + 1;
		while(input[prev] == '\n')
			prev +=  1;
		current = input.find(separater, prev);
	}
	if(prev < input.length())
		tokens.push_back(input.substr(prev, input.length()));
	
	// test print to check input data
	for(Server::s_size i = 0; i < tokens.size(); i ++)
		std::cout << "* input from " << cli.nickname()<< " (USER " << cli.idx() << ") <= " <<"\"" << tokens[i]<<"\"" <<std::endl;
    return tokens;
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

bool checkChannelName(std::string str)
{
	if(!isInclude(str[0], CHANNEL_PREFIX))
		return false;
	for(std::string::size_type i = 1; i < str.length() ; i++){
		if (!(std::isalpha(str[i]) || std::isdigit(str[i])) && !isInclude(str[i], ";[]\\`_^{|}"))
			return false;
		}
	return true;
}
