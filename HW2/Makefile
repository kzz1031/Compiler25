MAKEFLAGS = --no-print-directory

.PHONY: build clean rebuild test handin

build:
	@cd tools/demo; make build; cd ../..
# @cd tools/2.4b; make build; cd ../..
# @cd tools/2.5c; make build; cd ../..
# @cd tools/3.3d; make build; cd ../..
# @cd tools/3.12a; make build; cd ../..

clean:
	@cd tools/demo; make clean; cd ../..
# @cd tools/2.4b; make clean; cd ../..
# @cd tools/2.5c; make clean; cd ../..
# @cd tools/3.3d; make clean; cd ../..
# @cd tools/3.12a; make clean; cd ../..

rebuild: clean build

test:
	@cd tools/demo; make test; cd ../..
# @cd tools/2.4b; make test; cd ../..
# @cd tools/2.5c; make test; cd ../..
# @cd tools/3.3d; make test; cd ../..
# @cd tools/3.12a; make test; cd ../..

handin:
	@if [ ! -f docs/report.pdf ]; then \
		echo "请先在docs文件夹下攥写报告, 并转换为'report.pdf'"; \
		exit 1; \
	fi; \
	echo "请输入'学号-姓名' (例如: 12345678910-某个人)"; \
	read filename; \
	zip -q -r "docs/$$filename-hw2.zip" \
	  Makefile test tools docs/report.pdf
