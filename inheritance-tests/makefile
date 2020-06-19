.PHONY: runtests
runtests: test1
	@./test1

test1: test1.cpp simple.h virtual.h
	@clang++ -Wall -g -O0 test1.cpp -o test1

assembly: test1
	@clang++ -S -masm=intel -g -O0 test1.cpp

memory_layout: test1
	# awk scripts derived from https://stackoverflow.com/a/22222219/2378475
	@clang++ -E test1.cpp > test1_preprocessed.cpp
	@clang++ -cc1 -fdump-record-layouts -emit-llvm-only -cl-opt-disable -O0 test1_preprocessed.cpp \
		| awk '/\*\*\* Dumping AST/{f=1} /^\s*$$/{f=0;print} f' \
		| awk '/\*\*\* Dumping AST/{f=1} f; /^\s*$$/{f=0}'
	@rm test1_preprocessed.cpp test1_preprocessed.ll

pahole: test1
	@pahole --verbose --exclude=std --exclude=__ test1

.PHONY: clean
clean:
	@rm -rf *.out *.s test1
