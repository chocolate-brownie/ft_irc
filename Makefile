NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./includes/

SRCS_DIR = srcs
OBJ_DIR = obj

SRCS = $(wildcard $(SRCS_DIR)/*.cpp)
OBJS = $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp, %.o, $(notdir $(SRCS))))

all: $(NAME)
$(NAME): $(OBJS)
	@echo "Linking executable... $(NAME)"
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "Done! Executable is ready: $(NAME)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling... $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up object directory... $(OBJ_DIR)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Removing executable... $(NAME)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re $(OBJ_DIR)