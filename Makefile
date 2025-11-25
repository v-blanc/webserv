CXX := c++
CXXFLAGS := -Wall -Werror -Wextra -std=c++98 -MMD -MP -g

NAME := webserv
RM := rm -rf

INCDIR := include
SRCDIR := src
OBJDIR := build

FILES := Config.cpp Server.cpp main.cpp

SRCS := $(addprefix $(SRCDIR)/, $(FILES))
OBJS := $(addprefix $(OBJDIR)/, $(FILES:.cpp=.o))
DEPS := $(OBJS:.o=.d)

.PHONY: all libft clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -I$(INCDIR) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJDIR)

fclean:
	$(RM) $(OBJDIR)
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)
