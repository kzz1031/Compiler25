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
	$(RM) test/*5.* \

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

.PHONY: run run-one gentests gentest-one patchdemo 

GENAST = $(CURDIR)/../HW5/build/tools/generatetests/gentests
GENIR = $(CURDIR)/../HW5/build/tools/main/main
GENQUAD = $(CURDIR)/../HW6/build/tools/main/main
GENSSA = $(CURDIR)/../HW7/build/tools/main/main
MAIN = $(BUILD_DIR)/tools/main/main
GENTESTS = $(BUILD_DIR)/tools/generatetests/gentests
PATCHDEMO = $(BUILD_DIR)/tools/patchdemo/patchdemo

compile: $(MAIN)
	cd $(CURDIR)/test && \
	for file in $$(ls .); do \
		if [ "$${file#*.}" = "fmj" ]; then \
			echo "Reading ${FILE}.4-prepared-xml.quad and ${FILE}.4-xml.clr"; \
			$(MAIN) "$${file%%.*}"; \
		fi; \
	done; \
	cd .. > /dev/null 2>&1 

compile-fmj: $(MAIN)
	cd $(CURDIR)/fmj_normal && \
	for file in $$(ls .); do \
		if [ "$${file#*.}" = "fmj" ]; then \
			echo "Reading ${FILE}.4-prepared-xml.quad and ${FILE}.4-xml.clr"; \
			$(MAIN) "$${file%%.*}"; \
		fi; \
	done; \
	cd .. > /dev/null 2>&1 

run: $(MAIN)
	cd $(CURDIR)/test && \
	echo "List of files:" && ls . && \
	for file in *; do \
		#echo "Checking file: $$file"; \
		if [ "$${file#*.}" = "s" ]; then \
			echo "[Running $$file]"; \
			arm-linux-gnueabihf-gcc -mcpu=cortex-a72 -Wall -Wextra --static -o "$${file%%.*}" "$${file%%.*}.s" ../vendor/libsysy/libsysy32.s -lm; \
			qemu-arm -B 0x1000 $${file%%.*}; \
			echo "Finished running $$file"; \
			echo "Return value: $$?"; \
		fi; \
	done

run-assem-one: $(MAIN)
	cd $(CURDIR)/test/assem && \
	echo "List of files:" && ls . && \
	for file in *; do \
		#echo "Checking file: $$file"; \
		if [ "$${file#*.}" = "4-prepared.quad" ]; then \
			echo "Reading $$file"; \
			arm-linux-gnueabihf-gcc -mcpu=cortex-a72 -Wall -Wextra --static -o "$${file%%.*}" "$${file%%.*}.s" ../../vendor/libsysy/libsysy32.s -lm; \
			echo "Running the final assembly program........." ; \
			qemu-arm -B 0x1000 $${file%%.*}; \
			echo "Finished running $$file"; \
		fi; \
	done

FILE=hw5test0

run-one: $(MAIN)
	cd $(CURDIR)/test && \
	echo "Reading ${FILE}.4-ssa-xml.quad and ${FILE}.4-xml.clr"; \
	$(MAIN) "${FILE}"; \
	cd .. > /dev/null 2>&1 

run-one-fmj: $(MAIN)
	cd $(CURDIR)/fmj_normal && \
	echo "Reading ${FILE}.4-ssa-xml.quad and ${FILE}.4-xml.clr"; \
	$(MAIN) "${FILE}"; \
	cd .. > /dev/null 2>&1 

run-one-assem: $(MAIN)
	cd $(CURDIR)/fmj_normal && \
    arm-linux-gnueabihf-gcc -mcpu=cortex-a72 -Wall -Wextra --static -o "${FILE}" "${FILE}.s" ../vendor/libsysy/libsysy32.s -lm; \
	echo "Running the final assembly program: ${FILE}........." ; \
	qemu-arm -B 0x1000 ${FILE}; \
    RET=$$?; \
    echo "Program return value: $$RET"; \
	cd .. > /dev/null 2>&1 
