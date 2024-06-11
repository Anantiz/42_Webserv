CXX:=c++
INCLUDES:= -I./includes
CPPFLAGS:= -std=c++98 -Wall -Wextra -g3 -Wshadow $(INCLUDES)
# -Werror //

RM:=rm -rf

NAME = webserv

##############################################################################
##############################################################################
##############################################################################

OBJDIR = .obj
SRCDIR = src

SRC_FILES = main.cpp

PARSING_PATH:=parsing
PARSING_FILES:=
SRC_FILES+= 	$(addprefix $(PARSING_PATH)/,$(PARSING_FILES))

INTERPRET_PATH:=interpret
INTERPRET_FILES:=
SRC_FILES+= 	$(addprefix $(INTERPRET_PATH)/,$(INTERPRET_FILES))

UTILS_PATH:=utils
UTILS_FILES:= debug.cpp
SRC_FILES+= 	$(addprefix $(UTILS_PATH)/,$(UTILS_FILES))

EXEC_PATH:=exec
EXEC_FILES:=
SRC_FILES+= 	$(addprefix $(EXEC_PATH)/,$(EXEC_FILES))

SETUP_PATH:=setup
SETUP_FILES:=
SRC_FILES+= 	$(addprefix $(SETUP_PATH)/,$(SETUP_FILES))

SRC_FILES:=		$(addprefix $(SRCDIR)/,$(SRC_FILES))
SRC_OBJECTS:=	$(patsubst $(SRCDIR)/%.cpp, ${OBJDIR}/%.o,$(SRC_FILES))

##############################################################################
##############################################################################
##############################################################################

all: $(NAME)

${OBJDIR}/%.o:$(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CPPFLAGS) -c $< -o $@

$(NAME): $(SRC_OBJECTS)
	$(CXX) $(CPPFLAGS) -o $@ $^

clean:
	@$(RM) $(SRC_OBJECTS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
