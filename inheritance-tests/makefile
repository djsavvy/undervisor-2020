.PHONY: run_static
run_static: build_static
	@./inheritance_tests

.PHONY: run_dynamic
run_dynamic: build_dynamic
	@./inheritance_tests

.PHONY: build_dynamic
build_dynamic: inheritance_tests.cpp *.h
	@clang++ -Wall -g -O0 inheritance_tests.cpp -o inheritance_tests

assembly: build_dynamic
	@clang++ -S -masm=intel -g inheritance_tests.cpp -o assembly.s

.PHONY: build_static
build_static: inheritance_tests.cpp *.h
	@clang++ -stdlib=libstdc++ -Wall -g -O0 inheritance_tests.cpp -o inheritance_tests -static -lstdc++

.PHONY: objdump_dynamic
objdump_dynamic: build_dynamic
	@objdump -dSgrtx inheritance_tests > dynamic_objdump

.PHONY: objdump_static
objdump_static: build_static
	@objdump -dSgrtx inheritance_tests > static_objdump

.PHONY: memory_layout
memory_layout: build_dynamic
	# awk scripts derived from https://stackoverflow.com/a/22222219/2378475
	@clang++ -E inheritance_tests.cpp > inheritance_tests_preprocessed.cpp
	@clang++ -cc1 -fdump-record-layouts -emit-llvm-only -cl-opt-disable -O0 inheritance_tests_preprocessed.cpp \
		| awk '/\*\*\* Dumping AST/{f=1} /^\s*$$/{f=0;print} f' \
		| awk '/\*\*\* Dumping AST/{f=1} f; /^\s*$$/{f=0}'
	@rm inheritance_tests_preprocessed.cpp

# For use in godbolt
.PHONY: noheaders 
noheaders: 
	@awk '!/^#include "/' inheritance_tests.cpp > inheritance_tests_noheaders.cpp
	@cat *.h inheritance_tests_noheaders.cpp > noheaders.cpp
	@rm inheritance_tests_noheaders.cpp

pahole: build_dynamic
	pahole --verbose --exclude=std --exclude=__ inheritance_tests

.PHONY: clean
clean:
	@rm -rf *.out *.s inheritance_tests noheaders.cpp static_objdump dynamic_objdump
