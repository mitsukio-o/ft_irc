# A's part

NAME	=	ircserv

CPP		=	c++ -Wall -Wextra -Werror -std=c++98 -pedantic -Wshadow -Iinc

SRCS	=	src/main.cpp src/Server.cpp src/Commands.cpp src/Client.cpp \
			src/Channel.cpp
OBJS	=	$(SRCS:.cpp=.o)
DEPS	=	$(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CPP) -MMD -MP -MF"$(@:%.o=%.d)" -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
