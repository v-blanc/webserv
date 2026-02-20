CXX := c++
CXXFLAGS := -Wall -Werror -Wextra -std=c++98 -MMD -MP -g

NAME := webserv
RM := rm -rf


INCDIR := include
SRCDIR := src
OBJDIR := build

FILES := parsing_conf/LexerConfig.cpp parsing_conf/ParserConfig.cpp \
		parsing_conf/GlobalConfig.cpp parsing_conf/ServerConfig.cpp parsing_conf/LocationConfig.cpp \
		\
		http_request/HTTPRequest.cpp http_request/HTTPRequest_parseRequest.cpp \
		http_request/HTTPResponse.cpp http_request/SessionManager.cpp\
		server/GlobalServer.cpp server/Server.cpp \
		\
		utils/getTimeOfDay.cpp utils/getLocalFileContent.cpp utils/isInvalidPath.cpp utils/generateAutoindexHTML.cpp \
		utils/errorMessageConfig.cpp utils/errorMessageServer.cpp \
		cgi/Cgi.cpp \
		\
		main.cpp

INCLUDES := $(addprefix -I,$(shell find $(INCDIR) -type d))
SRCS := $(addprefix $(SRCDIR)/, $(FILES))
OBJS := $(addprefix $(OBJDIR)/, $(FILES:.cpp=.o))
DEPS := $(OBJS:.o=.d)

.PHONY: all libft clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJDIR)

fclean:
	$(RM) $(OBJDIR)
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)
