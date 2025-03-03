CURRENT_DIR = tools/2.5c
BUILD_DIR = $(CURRENT_DIR)/build
TEST_DIR = test/2.5c

.PHONY: build run test clean rebuild

build:
	@mkdir -p build; \
	yacc -d -v -o build/parser.c parser.yacc; \
	lex -o build/lexer.c lexer.lex; \
	cc -o build/main build/lexer.c build/parser.c

run:
	@build/main

test:
	@cd ../../$(TEST_DIR); \
	echo "---Running tests for 2.5c---"; \
	for file in $$(ls .); do \
		if [ "$${file##*.}" = "txt" ]; then \
			echo "Input file: $${file%%.*}".txt; \
			cat "$${file%%.*}".txt; \
			echo ""; \
			../../$(BUILD_DIR)/main < "$${file%%.*}".txt; \
		fi \
	done; \
	cd ../../$(CURRENT_DIR)

clean:
	@rm -rf build

rebuild: clean build