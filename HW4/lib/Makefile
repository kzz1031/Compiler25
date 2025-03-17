RM       = rm -rf
MAKEFLAGS = --no-print-directory

.PHONY: build clean rebuild

BUILD_DIR = $(CURDIR)/build

build:
	@cmake -G Ninja -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release; \
	cd $(BUILD_DIR) && ninja -v

clean: 
	@$(RM) build ; \
	$(RM) test/*.ast ; \

rebuild: clean build

.PHONY: main xml2astTesk

MAIN = $(BUILD_DIR)/tools/main/main
XML2ASTTest = $(BUILD_DIR)/tools/dev/xml2astTest
AST2XMLTest = $(BUILD_DIR)/tools/dev/ast2xmlTest

run: $(MAIN)
	@cd $(CURDIR)/test && \
	for file in $$(ls .); do \
        if [ "$${file##*.}" = "fmj" ]; then \
            echo "Parsing $${file%%.*}"; \
			$(MAIN) "$${file%%.*}"; \
#			echo "Comparing parsed write-out with load+then-write-out"; \
#			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug.ast" ; \
#			echo "Comparing parsed write-out with load+then-clone-then-write-out"; \
#			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug3.ast" ; \
#			echo "Comparing parsed write-out with load+then-clone-then-minusConst-converted-write-out"; \
#			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug4.ast" ; \
        fi \
	done; \
	cd .. > /dev/null 2>&1 

run-xml2astTest: $(XML2ASTTest)
	@cd $(CURDIR)/test && \
	for file in $$(ls .); do \
        if [ "$${file##*.}" = "fmj" ]; then \
            echo "Converting ast-xml to ast (and back to xml!)  $${file%%.*}"; \
			$(XML2ASTTest) "$${file%%.*}"; \
			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug.ast" ; \
        fi \
	done; \
	cd .. > /dev/null 2>&1 


run-ast2xmlTest: $(AST2XMLTest) 
	@cd $(CURDIR)/test && \
    echo "Converting ast to ast-xml: ast2xmlTestfile"; \
			$(AST2XMLTest) "ast2xmlTestfile"; \
	cd .. > /dev/null 2>&1 

run-one: $(MAIN)
	@cd $(CURDIR)/test && \
	echo "Parsing ${FILE}.fmj"; \
	$(MAIN) "${FILE}"; \
#	echo "Comparing parsed write-out with load+then-write-out"; \
#	diff "${FILE}.2.ast" "$${FILE}.2-debug.ast" ; \
#	echo "Comparing parsed write-out with load+then-clone-then-write-out"; \
#	diff "${FILE}.2.ast" "$${FILE}.2-debug3.ast" ; \
#	echo "Comparing parsed write-out with load+then-clone-then-minusConst-converted-write-out"; \
#	diff "$${FILE}.2.ast" "$${FILE}.2-debug4.ast" ; \
	cd .. > /dev/null 2>&1 

