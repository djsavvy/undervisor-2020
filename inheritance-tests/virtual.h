#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <inttypes.h>
#include <string>

class VirtualBase {
public:
    int vb_mem1;
    std::string vb_mem2;
    std::string vb_mem3;
    VirtualBase() : vb_mem1(60), vb_mem2("vb_test_string"){};
    virtual ~VirtualBase(){};
    virtual int virt_method(int a, int b) { return -1; }
    virtual int pure_virt_method(int a, int b) = 0;
};

class VirtualDerived1 : public VirtualBase {
public:
    uint64_t vd1_mem1 = 12;
    int virt_method(int a, int b) override { return a + b; }
    int pure_virt_method(int a, int b) override { return a * b; }
    std::string vd1_extra_method() { return "\"vd1_extra_method\""; }
    uint64_t vd1_extra_method2() { return vd1_mem1; }
    VirtualDerived1(){};
    ~VirtualDerived1(){};
};

class VirtualDerived2 : public VirtualBase {
public:
    uint64_t vd2_mem1 = 13;
    int virt_method(int a, int b) override { return a - b; }
    int pure_virt_method(int a, int b) override { return a / b; }
    std::string vd2_extra_method() { return "\"vd2_extra_method\""; }
    uint64_t vd2_extra_method2() { return vd2_mem1; }
    VirtualDerived2(){};
    ~VirtualDerived2(){};
};

class AbstractDerived1 : public VirtualBase {
public:
    uint64_t ad1_mem1;
    int virt_method(int a, int b) { return 0xbeef; }
    std::string ad1_extra_method() { return "ad1_extra_method"; }
    AbstractDerived1(){};
    ~AbstractDerived1(){};
};

class AbstractDerived1Derived1 : public AbstractDerived1 {
public:
    uint64_t ad1d1_mem1;
    int pure_virt_method(int a, int b) override { return 0xfeed; }
    std::string ad1d1_extra_method() { return "\"ad1d1_extra_method\""; }
    AbstractDerived1Derived1(){};
    ~AbstractDerived1Derived1(){};
};

void virtual_stack_tests() {
    printf("Stack test using references:\n");
    AbstractDerived1Derived1 ad1d1;
    VirtualBase& vb = static_cast<VirtualBase&>(ad1d1);
    printf(
        "VALID: Casted stack AbstractDerived1Derived1 ad1d1 (addr: %p) to "
        "VirtualBase& vb (addr: %p)\n",
        &ad1d1, &vb);
    printf("ad1d1.pure_virt_method(6, 9): %d\n", ad1d1.pure_virt_method(6, 9));
    printf("vb.pure_virt_method(6, 9): %d\n", vb.pure_virt_method(6, 9));
    printf("ad1d1.virt_method(6, 9): %d\n", ad1d1.virt_method(6, 9));
    printf("vb.virt_method(6, 9): %d\n", vb.virt_method(6, 9));

    VirtualDerived1& vd1 = static_cast<VirtualDerived1&>(vb);
    printf(
        "INVALID: Casted stack VirtualBase vb (addr: %p) down to "
        "VirtualDerived1& vd1 (addr: %p)\n",
        &vb, &vd1);
    printf("vd1.vd1_extra_method().c_str(): %s\n", vd1.vd1_extra_method().c_str());
    printf("vd1.vd1_extra_method2(): %" PRIu64 "\n", vd1.vd1_extra_method2());

    printf("\n");
}

static AbstractDerived1Derived1 g_ad1d1;

void virtual_global_tests() {
    printf("Global test using references:\n");
    VirtualBase& vb = static_cast<VirtualBase&>(g_ad1d1);
    printf(
        "VALID: Casted global AbstractDerived1Derived1 g_ad1d1 (addr: %p) to "
        "VirtualBase& vb (addr: %p)\n",
        &g_ad1d1, &vb);
    printf("g_ad1d1.pure_virt_method(6, 9): %d\n", g_ad1d1.pure_virt_method(6, 9));
    printf("vb.pure_virt_method(6, 9): %d\n", vb.pure_virt_method(6, 9));
    printf("g_ad1d1.virt_method(6, 9): %d\n", g_ad1d1.virt_method(6, 9));
    printf("vb.virt_method(6, 9): %d\n", vb.virt_method(6, 9));

    VirtualDerived1& vd1 = static_cast<VirtualDerived1&>(vb);
    printf(
        "INVALID: Casted global VirtualBase vb (addr: %p) down to "
        "VirtualDerived1& vd1 (addr: %p)\n",
        &vb, &vd1);
    printf("vd1.vd1_extra_method().c_str(): %s\n", vd1.vd1_extra_method().c_str());
    printf("vd1.vd1_extra_method2(): %" PRIu64 "\n", vd1.vd1_extra_method2());

    printf("\n");
}

void virtual_tests() {
    printf("--------------- VIRTUAL TESTS ---------------\n\n");

    printf("Indirectly Casting between siblings:\n");
    VirtualDerived1* vd1 = new VirtualDerived1();
    VirtualBase* vb1 = static_cast<VirtualBase*>(vd1);
    VirtualDerived2* vd2 = static_cast<VirtualDerived2*>(vb1);
    printf(
        "INVALID: VirtualDerived1* vd1 (%p) casted up to VirtualBase* vb1 "
        "(%p), then casted down to VirtualDerived2* vd2 (%p)\n",
        vd1, vb1, vd2);
    printf("vd1->virt_method(6, 9): %d\n", vd1->virt_method(6, 9));
    printf("vb1->virt_method(6, 9): %d\n", vb1->virt_method(6, 9));
    printf("vd2->virt_method(6, 9): %d\n", vd2->virt_method(6, 9));
    printf("vd1->vd1_extra_method().c_str(): %s\n", vd1->vd1_extra_method().c_str());
    printf("vd1->vd1_extra_method2(): %" PRIu64 "\n", vd1->vd1_extra_method2());
    printf("vd2->vd2_extra_method().c_str()): %s\n", vd2->vd2_extra_method().c_str());
    printf("vd2->vd2_extra_method2(): %" PRIu64 "\n", vd2->vd2_extra_method2());
    printf("\n");

    virtual_stack_tests();
    virtual_global_tests();

    printf("--------------- END VIRTUAL TESTS ---------------\n\n");
}

#endif /* VIRTUAL_H */
