CLANG		=	$(shell cc -v 2>&1 | grep --quiet clang && echo 1)
ifeq ($(CLANG),1)
	CC		=	c++
else
	CC		=	clang++
endif

CFLAGS		=	-std=c++98
CFLAGS		+=	-Wall -Wextra -Werror
DEBUGFLAGS	+=	-g -D_GLIBCXX_DEBUG


NAME		=	conversion

all: $(NAME)

SRC_FILES	=	$(shell find . -name "*.cpp")

OBJ_DIR		=	.build
OBJ_FILES	=	$(SRC_FILES:./%.cpp=$(OBJ_DIR)/%.o)

#--------------------------DEPENDENCIES-----------------------#
DEPS							:=	$(OBJ_FILES:.o=.d)
-include $(DEPS)
CPPFLAGS						:=	-MMD -MP

CPPFLAGS						+= $(addprefix -I, $(shell find Classes -type d))


$(OBJ_DIR)/%.o: ./%.cpp
	@mkdir -p $(@D)
	$(CC) $(DEBUGFLAGS) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ_FILES)
	$(CC) $(DEBUGFLAGS) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR)

re: clean all

lsp: clean
	@rm -f misc/compile_commands.json
	@bear --output misc/compile_commands.json -- make

leak:
	valgrind --show-leak-kinds=all --track-fds=yes ./$(NAME)
