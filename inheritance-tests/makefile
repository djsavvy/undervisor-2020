.PHONY: runtests
runtests: inheritance_tests
	@./inheritance_tests

inheritance_tests: inheritance_tests.cpp *.h
	@clang++ -Wall -g -O0 inheritance_tests.cpp -o inheritance_tests

assembly: inheritance_tests
	@clang++ -S -masm=intel -g -O0 inheritance_tests.cpp

.PHONY: memory_layout
memory_layout: inheritance_tests
	# awk scripts derived from https://stackoverflow.com/a/22222219/2378475
	@clang++ -E inheritance_tests.cpp > inheritance_tests_preprocessed.cpp
	@clang++ -cc1 -fdump-record-layouts -emit-llvm-only -cl-opt-disable -O0 inheritance_tests_preprocessed.cpp \
		| awk '/\*\*\* Dumping AST/{f=1} /^\s*$$/{f=0;print} f' \
		| awk '/\*\*\* Dumping AST/{f=1} f; /^\s*$$/{f=0}'
	@rm inheritance_tests_preprocessed.cpp

# For use in godbolt
.PHONY: noheaders 
noheaders: 
	cat *.h inheritance_tests.cpp > noheaders.cpp

pahole: inheritance_tests
	@pahole --verbose --exclude=std --exclude=__ inheritance_tests

.PHONY: clean
clean:
	@rm -rf *.out *.s inheritance_tests noheaders.cpp
