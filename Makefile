NAME = nemu
INC_DIR += ./include
BUILD_DIR ?= ./build
OBJ_DIR ?= $(BUILD_DIR)/obj
BINARY ?= $(BUILD_DIR)/$(NAME)

# include Makefile.git

.DEFAULT_GOAL = app

# Compilation flags
CC = gcc
CXX = g++
LD = g++
INCLUDES  = $(addprefix -I, $(INC_DIR))
CXXFLAGS += -std=c++11 -O2 -MMD -Wall -Werror -ggdb $(INCLUDES)

# Files to be compiled
SRCS = $(shell find src/ -name "*.cpp")
OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)

# Compilation patterns
$(OBJ_DIR)/%.o: src/%.cpp
	@echo + CXX $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

# Depencies
-include $(OBJS:.o=.d)

# Some convinient rules

.PHONY: app run submit clean
app: $(BINARY)

ARGS ?= -l $(BUILD_DIR)/nemu-log.txt

# Command to execute NEMU
NEMU_EXEC := $(BINARY) $(ARGS)

$(BINARY): $(OBJS)
#	$(call git_commit, "compile")
	@echo + LD $@
	@$(LD) -O2 -o $@ $^ -lSDL2 -lpthread #-lreadline

run: $(BINARY)
#	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: $(BINARY)
#	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC) 

clean: 
	rm -rf $(BUILD_DIR)

count:
		find . | grep '\.cpp$$\|\.h$$\|\.hpp$$' | xargs wc -l
		find . | grep '\.cpp$$\|\.h$$\|\.hpp$$' | xargs cat | grep -v ^$$ | wc -l
