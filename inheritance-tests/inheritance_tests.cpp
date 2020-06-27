#include "simple.h"
#include "virtual.h"
#include "multiple.h"
#include "diamond_virtual.h"
#include "diamond_nonvirtual.h"

void stack_spiller() {
    printf("Stack spiller test:\n");
    SimpleBase* sb1 = new SimpleBase();
    SimpleBase* sb2 = new SimpleBase();
    SimpleBase* sb3 = new SimpleBase();
    SimpleBase* sb4 = new SimpleBase();
    SimpleBase* sb5 = new SimpleBase();
    SimpleBase* sb6 = new SimpleBase();
    SimpleBase* sb7 = new SimpleBase();
    SimpleBase* sb8 = new SimpleBase();
    SimpleBase* sb9 = new SimpleBase();
    SimpleBase* sb10 = new SimpleBase();
    SimpleBase* sb11 = new SimpleBase();
    SimpleBase* sb12 = new SimpleBase();
    SimpleBase* sb13 = new SimpleBase();
    SimpleBase* sb14 = new SimpleBase();
    SimpleBase* sb15 = new SimpleBase();
    SimpleBase* sb16 = new SimpleBase();
    SimpleBase* sb17 = new SimpleBase();
    SimpleBase* sb18 = new SimpleBase();
    SimpleBase* sb19 = new SimpleBase();
    SimpleBase* sb20 = new SimpleBase();
    SimpleBase* sb21 = new SimpleBase();
    SimpleBase* sb22 = new SimpleBase();
    SimpleBase* sb23 = new SimpleBase();
    SimpleBase* sb24 = new SimpleBase();
    SimpleDerived1* sd1 = static_cast<SimpleDerived1*>(sb1);
    SimpleDerived1* sd2 = static_cast<SimpleDerived1*>(sb2);
    SimpleDerived1* sd3 = static_cast<SimpleDerived1*>(sb3);
    SimpleDerived1* sd4 = static_cast<SimpleDerived1*>(sb4);
    SimpleDerived1* sd5 = static_cast<SimpleDerived1*>(sb5);
    SimpleDerived1* sd6 = static_cast<SimpleDerived1*>(sb6);
    SimpleDerived1* sd7 = static_cast<SimpleDerived1*>(sb7);
    SimpleDerived1* sd8 = static_cast<SimpleDerived1*>(sb8);
    SimpleDerived1* sd9 = static_cast<SimpleDerived1*>(sb9);
    SimpleDerived1* sd10 = static_cast<SimpleDerived1*>(sb10);
    SimpleDerived1* sd11 = static_cast<SimpleDerived1*>(sb11);
    SimpleDerived1* sd12 = static_cast<SimpleDerived1*>(sb12);
    SimpleDerived1* sd13 = static_cast<SimpleDerived1*>(sb13);
    SimpleDerived1* sd14 = static_cast<SimpleDerived1*>(sb14);
    SimpleDerived1* sd15 = static_cast<SimpleDerived1*>(sb15);
    SimpleDerived1* sd16 = static_cast<SimpleDerived1*>(sb16);
    SimpleDerived1* sd17 = static_cast<SimpleDerived1*>(sb17);
    SimpleDerived1* sd18 = static_cast<SimpleDerived1*>(sb18);
    SimpleDerived1* sd19 = static_cast<SimpleDerived1*>(sb19);
    SimpleDerived1* sd20 = static_cast<SimpleDerived1*>(sb20);
    SimpleDerived1* sd21 = static_cast<SimpleDerived1*>(sb21);
    SimpleDerived1* sd22 = static_cast<SimpleDerived1*>(sb22);
    SimpleDerived1* sd23 = static_cast<SimpleDerived1*>(sb23);
    SimpleDerived1* sd24 = static_cast<SimpleDerived1*>(sb24);
    printf("%c\n", sd1->sd1_member1);
    printf("%c\n", sd2->sd1_member1);
    printf("%c\n", sd3->sd1_member1);
    printf("%c\n", sd4->sd1_member1);
    printf("%c\n", sd5->sd1_member1);
    printf("%c\n", sd6->sd1_member1);
    printf("%c\n", sd7->sd1_member1);
    printf("%c\n", sd8->sd1_member1);
    printf("%c\n", sd9->sd1_member1);
    printf("%c\n", sd10->sd1_member1);
    printf("%c\n", sd11->sd1_member1);
    printf("%c\n", sd12->sd1_member1);
    printf("%c\n", sd13->sd1_member1);
    printf("%c\n", sd14->sd1_member1);
    printf("%c\n", sd15->sd1_member1);
    printf("%c\n", sd16->sd1_member1);
    printf("%c\n", sd17->sd1_member1);
    printf("%c\n", sd18->sd1_member1);
    printf("%c\n", sd19->sd1_member1);
    printf("%c\n", sd20->sd1_member1);
    printf("%c\n", sd21->sd1_member1);
    printf("%c\n", sd22->sd1_member1);
    printf("%c\n", sd23->sd1_member1);
    printf("%c\n", sd24->sd1_member1);
    printf("End stack spiller test.\n\n");
}

void pathological_tests() {
    printf("--------------- \"PATHOLOGICAL\" TESTS ---------------\n\n");

    printf("Invalid cast in if-statement condition:\n");
    SimpleDerived1* sd1 = new SimpleDerived1();
    SimpleBase* sb1 = new SimpleBase();

    // INVALID CASTS in both condition and body, but both gets compiled out
    if (((void*)static_cast<SimpleDerived1*>(sb1)) == ((void*)sb1)) {
        SimpleDerived1* sd2 = static_cast<SimpleDerived1*>(sb1);
        printf("%d\n", sd2->sd2_member2[13]);
    }
    printf("\n");

    stack_spiller();

    printf("--------------- END \"PATHOLOGICAL\" TESTS ---------------\n\n");
}

int main() {
    simple_tests();
    virtual_tests();
    multiple_tests();
    diamond_virtual_tests();
    diamond_nonvirtual_tests();
    pathological_tests();
}
