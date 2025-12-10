NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./includes/

SRCS_DIR = src
OBJ_DIR = obj

# List of subdirectories to include
DIRS = . 1_network 2_parser 3_commands utils

# Find all .cpp files in the specified directories
SRCS = $(foreach dir, $(DIRS), $(wildcard $(SRCS_DIR)/$(dir)/*.cpp))

# Map .cpp files to .o files in the object directory
OBJS = $(patsubst $(SRCS_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking executable... $(NAME)"
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@echo "Done! Executable is ready: $(NAME)"

# Create object directory structure properly
$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
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