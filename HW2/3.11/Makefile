CXX = g++
CXXFLAGS = -Wall -g
TARGET = main
SRCS = main.cc
OBJS = $(SRCS:.cc=.o)
TEST_DIR = test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	@for testfile in $(TEST_DIR)/*; do \
		echo "Running $$testfile:"; \
		./$(TARGET) $$testfile; \
	done

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all run clean