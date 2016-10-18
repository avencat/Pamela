CC	= gcc

RM	= rm -rf

NAME	= pam_module.so

LDFLAGS	= -lpam

CFLAGS	= -fPIC -W -Wall -Wextra -Werror

SRCS	= pam_module.c

OBJS	= $(SRCS:.c=.o)


$(NAME):  $(OBJS)
	  $(CC) -shared -o $(NAME) $(OBJS)

all:	  $(NAME)

clean:
	  $(RM) $(OBJS)

fclean:	  clean
	  $(RM) $(NAME)

re:	  fclean all

.PHONY:	  all clean fclean re
