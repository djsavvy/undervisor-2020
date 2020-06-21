#include "simple.h"
#include "virtual.h"
#include "multiple.h"
#include "diamond_virtual.h"
#include "diamond_nonvirtual.h"

int main() {
    simple_tests();
    virtual_tests();
    multiple_tests();
    diamond_virtual_tests();
    diamond_nonvirtual_tests();
}
