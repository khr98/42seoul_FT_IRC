NAME	= ircserv

CC		= clang++ 
CFLAGS	= -Wall -Wextra -Werror -std=c++98

SRCS	= main.cpp Server.cpp Client.cpp Channel.cpp Utils.cpp
OBJS	= $(SRCS:.cpp=.o)
DEPS	= $(SRCS:.cpp=.d)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) -o $(NAME) $(CFLAGS) $(OBJS)

%.o : %.cpp
	$(CC) $(CFLAGS) -MD -c -o $@ $<

clean :
	rm -rf $(OBJS) $(DEPS)

fclean : clean
	rm -rf $(NAME)

re : fclean all

-include $(DEPS)

.PHONY : all clean fclean re