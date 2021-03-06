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

__attribute__((noinline))
int simple_function_block() {
    volatile SimpleBase* sb1 = new SimpleBase();
    return sb1->sb_member1;
}

void pathological_tests() {
    printf("--------------- \"PATHOLOGICAL\" TESTS ---------------\n\n");

    {
        printf("Invalid cast in if-statement condition:\n");
        SimpleDerived1* sd1 = new SimpleDerived1();
        SimpleBase* sb1 = new SimpleBase();

        // INVALID CASTS in both condition and body, but both get compiled out
        if (((void*)static_cast<SimpleDerived1*>(sb1)) == ((void*)sb1)) {
            SimpleDerived1* sd2 = static_cast<SimpleDerived1*>(sb1);
            printf("Looks like the condition check got compiled out! %d\n", sd2->sd2_member2[13]);
        }

        {
            DiamondVirtualIntermediate1* dvi1 = new DiamondVirtualIntermediate1();
            dvi1->dvi1_mem1 = 32322;
            // INVALID CAST in condition, still gets compiled out
            if(static_cast<DiamondVirtualIntermediate2*>(static_cast<void*>(dvi1))->dvi2_method1() == dvi1->dvi1_method()) {
                printf("INVALID cast in this condition got compiled out!\n");
            }
        }

        printf("\n");
    }

    stack_spiller();

    {
        printf("Pointers to pointers:\n");
        VirtualDerived1* vd1 = new VirtualDerived1();
        VirtualBase* vb1 = new VirtualDerived2();
        VirtualBase** pvb1 = &vb1;
        printf("VirtualBase** pvb1 (%p) points to VirtualBase* (%p)\n", pvb1, vb1);
        VirtualDerived1* vd2 = static_cast<VirtualDerived1*>(*pvb1);
        printf("INVALID: VirtualBase* *pvb1 (%p) casted to VirtualDerived1* vd2 (%p)\n", *pvb1, vd2);
        *pvb1 = reinterpret_cast<VirtualDerived2*>(&vd1);
        // printf("INVALID: *pvb1 (%p) reinterpret_casted to VirtualDerived2*
        printf("\n");
    }

    {
        printf("Fun with constructors and assignment operators:\n");
        SimpleDerived1 sd1;
        SimpleDerived1 sd1_2 = sd1; /* Copy constructor */
        printf("SimpleDerived1 sd1_2 copy-constructed from SimpleDerived1 sd1\n");
        sd1.sd1_member1 = 30;
        printf("sd1.sd1_mem1: %d\n", sd1.sd1_member1);
        printf("sd1_2.sd1_mem1: %d\n", sd1_2.sd1_member1);
        sd1 = sd1_2; /* Copy-assignment operator */
        printf("Copy-assignment operator called: sd1 = sd1_2\n");
        printf("sd1.sd1_mem1: %d\n", sd1.sd1_member1);
        printf("\n");
    }

    {
        printf("Going through a void*:\n");
        DiamondVirtualIntermediate1* dvi1 = new DiamondVirtualIntermediate1();
        DiamondVirtualIntermediate2* dvi2 = static_cast<DiamondVirtualIntermediate2*>(static_cast<void*>(dvi1));
        printf("INVALID: DiamondVirtualIntermediate1* vd1 (%p) static_casted through a void* to DiamondVirtualIntermediate2* vd2 (%p)\n", dvi1, dvi2);
        printf("dvi1->dvb_shared_method().c_str(): %s\n", dvi1->dvb_shared_method().c_str());
        printf("dvi2->dvb_shared_method().c_str(): %s\n", dvi2->dvb_shared_method().c_str());
        printf("dvi1->dvi1_method(): %d\n", dvi1->dvi1_method());
        printf("dvi2->dvi2_method(): %d\n", dvi2->dvi2_method());
        printf("dvi2->dvi2_method6(): %d\n", dvi2->dvi2_method6());
        printf("\n");
    }

    {
        printf("Calling destructor on an object allocated with new:\n");
        SimpleDerived1* sd1 = new SimpleDerived1();
        SimpleDerived2 sd2;
        printf("Created SimpleDerived1* (%p) and SimpleDerived2 (addr: %p)\n", sd1, &sd2);
        sd1->~SimpleDerived1();
        printf("Just called: sd1->~SimpleDerived1()\n");
        sd2.~SimpleDerived2();
        printf("Just called: sd2.~SimpleDerived2()\n");
        operator delete(sd1);
        printf("Just called: operator delete(sd1)\n");
        printf("Note that we cannot call: operator delete(&sd2), since it was not created with \"new\"\n");
        printf("\n");
    }

    {
        printf("Getting the address of operator new:\n");
        printf("address of operator_new(std::size_t): %p\n", static_cast<void* (*)(std::size_t)>(&(operator new)));
        printf("TODO: placement new, etc.?????????????\n");
        printf("\n");
    }

    {
        printf("Simple function block\n");
        printf("simple_function_block(): %d\n", simple_function_block());
        printf("\n");
    }

    {
        printf("Reading the value of %%rip:\n");
        // https://stackoverflow.com/a/19908182/2378475
        void* rip;
        asm volatile("leaq (%%rip), %[Var]" : [Var] "=r" (rip));
        printf("%%rip: %p\n", rip);
        asm volatile("leaq (%%rip), %[Var]" : [Var] "=r" (rip));
        printf("%%rip: %p\n", rip);
        asm volatile("leaq (%%rip), %[Var]" : [Var] "=r" (rip));
        printf("%%rip: %p\n", rip);
        asm volatile("leaq (%%rip), %[Var]" : [Var] "=r" (rip));
        printf("%%rip: %p\n", rip);
        asm volatile("leaq (%%rip), %[Var]" : [Var] "=r" (rip));
        printf("%%rip: %p\n", rip);
        asm volatile("leaq (%%rip), %[Var]" : [Var] "=r" (rip));
        printf("%%rip: %p\n", rip);
        asm volatile("leaq (%%rip), %[Var]" : [Var] "=r" (rip));
        printf("%%rip: %p\n", rip);
        printf("\n");
    }

    {
        printf("Different allocations within the same loop:\n");
        printf("Allocated 4 pointers, alternating between VirtualDerived2* and VirtualDerived1*. Casting them all to VirtualDerived2*:\n");
        void* pointers[4];
        for (int i = 0; i < 4; ++i) {
            if (i % 2 == 1) {
                pointers[i] = new VirtualDerived1();
            } else {
                pointers[i] = new VirtualDerived2();
            }
        }
        for(int i = 0; i < 4; ++i) {
            printf("%s: pointers[%d]->vd2_extra_method2(): %" PRIu64 "\n", i %2 == 0 ? "VALID" : "INVALID", i, static_cast<VirtualDerived2*>(pointers[i])->vd2_extra_method2());
        }
        printf("\n");
    }

    printf("TODO: write a test for when there's no vtable pointer and we invalidly cast to the first member of a class.\n");

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
