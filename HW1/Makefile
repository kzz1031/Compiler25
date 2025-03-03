RM       = rm -rf
MAKEFLAGS = --no-print-directory

.PHONY: build clean rebuild handin

BUILD_DIR = $(CURDIR)/build

build:
	@cmake -G Ninja -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release; \
	cd $(BUILD_DIR) && ninja
# cd $(BUILD_DIR) && ninja -v

clean: 
	@$(RM) build ; \
	$(RM) test/*.ast ; \

rebuild: clean build

handin:
	@if [ ! -f docs/report.pdf ]; then \
		echo "请先在docs文件夹下攥写报告, 并转换为'report.pdf'"; \
		exit 1; \
	fi; \
	echo "请输入'学号-姓名' (例如: 12345678910-某个人)"; \
	read filename; \
	zip -q -r "docs/$$filename-hw1.zip" \
	  docs/report.pdf include lib

.PHONY: run run-one run-random

MAIN = $(BUILD_DIR)/tools/main/main
RANDOM_TEST = $(BUILD_DIR)/tools/main/randomTest

run: $(MAIN)
	cd $(CURDIR)/test && \
	for file in $$(ls .); do \
        if [ "$${file##*.}" = "fmj" ]; then \
            echo "Parsing $${file%%.*}"; \
			$(MAIN) "$${file%%.*}"; \
			echo "Comparing parsed write-out with load+then-write-out"; \
			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug.ast" ; \
			echo "Comparing parsed write-out with load+then-clone-then-write-out"; \
			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug3.ast" ; \
			echo "Comparing parsed write-out with load+then-clone-then-minusConst-converted-write-out"; \
			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug4.ast" ; \
			echo "Comparing parsed write-out with load+then-clone-then-constant-propagation-write-out"; \
			diff "$${file%%.*}.2.ast" "$${file%%.*}.2-debug5.ast" ; \
        fi \
	done; \
	cd .. > /dev/null 2>&1 

FILE = my_test4

run-one: $(MAIN)
	cd $(CURDIR)/test && \
	echo "Parsing ${FILE}.fmj"; \
	$(MAIN) "${FILE}"; \
	echo "Comparing parsed write-out with load+then-write-out"; \
	diff "${FILE}.2.ast" "${FILE}.2-debug.ast" ; \
	echo "Comparing parsed write-out with load+then-clone-then-write-out"; \
	diff "${FILE}.2.ast" "${FILE}.2-debug3.ast" ; \
	echo "Comparing parsed write-out with load+then-clone-then-minusConst-converted-write-out"; \
	diff "${FILE}.2.ast" "${FILE}.2-debug4.ast" ; \
	echo "Comparing parsed write-out with load+then-clone-then-constant-propagation-write-out"; \
	diff "${FILE}.2.ast" "${FILE}.2-debug5.ast" ; \
	cd .. > /dev/null 2>&1

run-random: $(RANDOM_TEST)
	$(RANDOM_TEST)
