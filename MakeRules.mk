###############################################################################
# Generic variables
###############################################################################
CC=clang
CXX=clang++
LD=clang++

C_FLAGS=-Wall -O3
CPP_FLAGS=-std=c++11 -Wall -O3

###############################################################################
# Generic rules
###############################################################################
%.o : %.c
	@echo "[   CC] $@"
	@$(CC) $(C_FLAGS) $(INC_DIR) -o $@ -c $^
	
%.o : %.cpp
	@echo "[  CXX] $@"
	@$(CXX) $(CPP_FLAGS) $(INC_DIR) -o $@ -c $^

%.c : %.re2c.c
	@echo "[ RE2C] $@"
	@re2c -i $^ > $@
	
%.cpp : %.re2c.cpp
	@echo "[ RE2C] $@"
	@re2c -i $^ > $@