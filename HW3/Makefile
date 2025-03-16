RM       = rm -rf
MAKEFLAGS = --no-print-directory

BUILD_DIR = $(CURDIR)/build
TEST_DIR = $(CURDIR)/test
MAIN_EXE = $(BUILD_DIR)/tools/main/main

.PHONY: build clean veryclean rebuild handin

build:
	@cmake -G Ninja -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release; \
	cd $(BUILD_DIR) && ninja

clean:
	@find $(TEST_DIR) -type f \( \
		-name "*.ast" -o -name "*.out"\
		\) -exec $(RM) {} \;

veryclean: clean 
	@$(RM) $(BUILD_DIR)

rebuild: veryclean build

handin:
	@if [ ! -f docs/report.pdf ]; then \
		echo "请先在docs文件夹下攥写报告, 并转换为'report.pdf'"; \
		exit 1; \
	fi; \
	echo "请输入'学号-姓名' (例如: 12345678910-某个人)"; \
	read filename; \
	zip -q -r "docs/$$filename-hw3.zip" \
	  docs/report.pdf include lib

.PHONY: run run-one

run: $(MAIN_EXE)
	cd $(TEST_DIR) && \
	for file in $$(ls .); do \
        if [ "$${file##*.}" = "fmj" ]; then \
            echo "Parsing $${file%%.*}"; \
			$(MAIN_EXE) "$${file%%.*}"; \
        fi \
	done; \
	cd .. > /dev/null 2>&1 

FILE = test1

run-one: $(MAIN_EXE)
	cd $(TEST_DIR) && \
	echo "Parsing ${FILE}.fmj"; \
	$(MAIN_EXE) "${FILE}"; \
	cd .. > /dev/null 2>&1 
