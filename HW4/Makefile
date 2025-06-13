RM       = rm -rf
MAKEFLAGS = --no-print-directory

BUILD_DIR = $(CURDIR)/build
TEST_DIR = $(CURDIR)/test
PATCHDEMO = $(BUILD_DIR)/tools/patchdemo/patchdemo
MAIN_EXE = $(BUILD_DIR)/tools/main/main
TEST_EXE = $(BUILD_DIR)/tools/test/test

.PHONY: build clean veryclean rebuild test handin

build:
	@cmake -G Ninja -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release; \
	cd $(BUILD_DIR) && ninja

clean:
	@find $(TEST_DIR) -maxdepth 1 -type f \( \
		-name "*.irp"\
		\) -exec $(RM) {} \;

veryclean: clean 
	@$(RM) $(BUILD_DIR)

rebuild: veryclean build

test: clean
	@cd $(TEST_DIR); \
	for file in $$(ls .); do \
		if [ "$${file##*.}" = "fmj" ]; then \
			echo "[$${file%%.*}]"; \
			$(TEST_EXE) "$${file%%.*}"; \
		fi \
	done

handin:
	@if [ ! -f docs/report.pdf ]; then \
		echo "请先在docs文件夹下攥写报告, 并转换为'report.pdf'"; \
		exit 1; \
	fi; \
	echo "请输入'学号-姓名' (例如: 12345678910-某个人)"; \
	read filename; \
	zip -q -r "docs/$$filename-hw4.zip" \
	  docs/report.pdf include lib

.PHONY: patchdemo run run-one

patchdemo: $(PATCHDEMO)
	$(PATCHDEMO) "test/patchdemo" 

run: $(MAIN_EXE)
	cd $(CURDIR)/test && \
	for file in $$(ls .); do \
        if [ "$${file##*.}" = "ast" ]; then \
            echo "Reading $${file%%.*}"; \
			$(MAIN_EXE) "$${file%%.*}"; \
        fi \
	done; \
	cd .. > /dev/null 2>&1 

FILE ?= hw3test06

run-one: $(MAIN_EXE)
	cd $(CURDIR)/test && \
	echo "Reading ${FILE}.2.ast"; \
	$(MAIN_EXE) "${FILE}"; \
	cd .. > /dev/null 2>&1 
