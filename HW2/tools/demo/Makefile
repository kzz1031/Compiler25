.PHONY: build test clean rebuild

build:
	@mkdir -p build; \
	cc -o build/main.o -c main.c; \
	cc -o build/main.out build/main.o

clean:
	@rm -rf build

rebuild: clean build

test:
	@for file in ../../test/demo/*.txt; do \
    build/main.out < "$$file"; \
	done
