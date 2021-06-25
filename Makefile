NAME =	ircserv

SRCSPATH =	src/
#DEBUGPATH =	debug/
INCPATH =	inc/

#---------	INCLUDES --
INCLUDES =	$(INCPATH)main.hpp \
						$(INCPATH)Client.hpp \
						$(INCPATH)Server.hpp \
						$(INCPATH)Message.hpp \
						$(INCPATH)nick.hpp \
						$(INCPATH)Replies.hpp


#--------	SRCS --
SRCS = 		$(SRCSPATH)main.cpp \
					$(SRCSPATH)Client.cpp \
					$(SRCSPATH)Server.cpp \
					$(SRCSPATH)nick.cpp \
					$(SRCSPATH)Message.cpp



#--------	COMP --

CC =		clang++

CFLAGSPROD	= -Wall -Wextra -Werror -std=c++98
CFLAGS		= -Wall -Wextra -Werror
CFLAGSPADD	= -Wpadded
CFLAGSSAN	= -g -g3 -fsanitize=address
ACTIVES_FLAGS = $(CFLAGSSAN) $(CFLAGSPROD)

OBJS = ${SRCS:.cpp=.o}

$(NAME):	$(OBJS) $(INCLUDES)
					$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS):	$(INCLUDES)

all:		$(NAME)

launch:		$(NAME)
					./$(NAME)

clean:
			${RM} $(OBJS)

fclean:		clean
			${RM} $(NAME).a $(NAME)

re:			fclean all

.PHONY: clear
