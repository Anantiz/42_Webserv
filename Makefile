CXX:=c++
INCLUDES:= -I./src
CPPFLAGS:= -std=c++98 -Wall -Wextra -g3 -Wshadow $(INCLUDES)
# -Werror //

DEFINES:= -DDEBUG_PROD

RM:=rm -rf
NAME = webserv

OBJDIR = .obj
SRCDIR = src

SRC_FILES = main.cpp mock_data.cpp

### PARSER ########## ########## ########## ########## ##########
PARSING_PATH:=config_parser
PARSING_FILES:= parser.cpp
SRC_FILES+= 	$(addprefix $(PARSING_PATH)/,$(PARSING_FILES))



### CLIENT ########## ########## ########## ########## ##########
CLIENT_PATH:=client
CLIENT_FILES:= client.cpp
SRC_FILES+= 	$(addprefix $(CLIENT_PATH)/,$(CLIENT_FILES))



### UTILS ########## ########## ########## ########## ##########
UTILS_PATH:=utils
UTILS_FILES:= logs.cpp utils.cpp signals.cpp
SRC_FILES+= 	$(addprefix $(UTILS_PATH)/,$(UTILS_FILES))



### CLUSTER ########## ########## ########## ########## ##########
CLUSTER_PATH:=cluster
CLUSTER_FILES:= cluster.cpp start.cpp run.cpp methods.cpp
SRC_FILES+= 	$(addprefix $(CLUSTER_PATH)/,$(CLUSTER_FILES))



### SERV ########## ########## ########## ########## ##########
SERV_PATH:=server
SERV_FILES:= server.cpp
SRC_FILES+= 	$(addprefix $(SERV_PATH)/,$(SERV_FILES))
LOCATIONS_PATH:=$(SERV_PATH)/location
LOCATIONS_FILES:= location.cpp
SRC_FILES+= 	$(addprefix $(LOCATIONS_PATH)/,$(LOCATIONS_FILES))




SRC_FILES:=		$(addprefix $(SRCDIR)/,$(SRC_FILES))
SRC_OBJECTS:=	$(patsubst $(SRCDIR)/%.cpp, ${OBJDIR}/%.o,$(SRC_FILES))


all: $(NAME)

${OBJDIR}/%.o:$(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CPPFLAGS) -c $< -o $@

$(NAME): $(SRC_OBJECTS)
	$(CXX) $(CPPFLAGS) $(DEFINES) -o $@ $^

clean:
	@$(RM) $(SRC_OBJECTS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
