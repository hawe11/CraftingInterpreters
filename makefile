CXX      := g++
CXXFLAGS := -g -std=c++14

SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
TRG := main.out

all: compile link

compile: $(OBJ)

link: $(TRG)

clean:
	rm -f $(OBJ_DIR)/*.o

$(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $?

$(TRG) : $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ 

.PHONY: clean all compile link
