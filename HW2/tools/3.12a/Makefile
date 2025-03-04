CURRENT_DIR = tools/3.12a
BUILD_DIR = build
TEST_DIR = test/3.12a

CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = $(BUILD_DIR)/parser
SRC = main.cc
OBJ = $(BUILD_DIR)/$(SRC:.cc=.o)

.PHONY: all clean test build

all: $(TARGET)

build: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJ) -o $(TARGET)

$(BUILD_DIR)/%.o: %.cc
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

test: $(TARGET)
	@cd ../../$(TEST_DIR); \
	echo "---Running tests for 3.12a---"; \
	for file in $$(ls .); do \
		if [ "$${file##*.}" = "txt" ]; then \
			echo "Input file: $${file%%.*}".txt; \
			# cat "$${file%%.*}".txt; \
			# echo ""; \
			../../$(CURRENT_DIR)/build/parser "$${file%%.*}".txt; \
		fi \
	done; \
	cd ../../$(CURRENT_DIR)
