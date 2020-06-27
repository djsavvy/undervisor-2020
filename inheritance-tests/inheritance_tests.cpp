#include "simple.h"
#include "virtual.h"
#include "multiple.h"
#include "diamond_virtual.h"
#include "diamond_nonvirtual.h"

void pathological_tests() {
    SimpleDerived1* sd1 = new SimpleDerived1();
    SimpleBase* sb1 = new SimpleBase();

    // INVALID CASTS in both condition and body, but both gets compiled out
    if (((void*)static_cast<SimpleDerived1*>(sb1)) == ((void*)sb1)) {
        SimpleDerived1* sd2 = static_cast<SimpleDerived1*>(sb1);
        printf("%d\n", sd2->sd2_member2[13]);
    }
}

int main() {
    simple_tests();
    virtual_tests();
    multiple_tests();
    diamond_virtual_tests();
    diamond_nonvirtual_tests();
    pathological_tests();
}
