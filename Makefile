# **************************************************************************** #
#                                  Makefile                                    #
# **************************************************************************** #

NAME = ft_ssl
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g3


PATH_HEADER = inc/
PATH_SRC = src/
PATH_OBJ = objs/

HEADER = ft_ssl.h \
		 utils.h \
		 digest_ops.h \
		 md5.h sha256.h

OPTIONS = -I $(PATH_HEADER)

HEADERS = $(addprefix $(PATH_HEADER), $(HEADER))

SRC = ft_ssl.c \
	  utils.c \
 	  command_hash.c \
	  md5.c sha256.c

SRCS = $(addprefix $(PATH_SRC), $(SRC))

OBJ = $(SRCS:.c=.o)
OBJS = $(addprefix $(PATH_OBJ), $(OBJ))

all: $(NAME)


$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(@) $(OBJS)


$(PATH_OBJ)$(PATH_SRC)%.o: $(PATH_SRC)%.c $(HEADERS)
	@mkdir -p $(PATH_OBJ)$(PATH_SRC)
	$(CC) $(CFLAGS) $(OPTIONS) -c $< -o $@

clean:
	rm -f $(OBJ)
	rm -rf $(PATH_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all