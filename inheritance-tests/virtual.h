#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <inttypes.h>
#include <string>

class VirtualBase {
public:
    int vb_mem1;
    std::string vb_mem2;
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
    std::string vd1_extra_method() { return "vd1_extra_method"; }
    uint64_t vd1_extra_method2() { return vd1_mem1; }
    VirtualDerived1(){};
    ~VirtualDerived1(){};
};

class VirtualDerived2 : public VirtualBase {
public:
    uint64_t vd2_mem1 = 13;
    int virt_method(int a, int b) override { return a - b; }
    int pure_virt_method(int a, int b) override { return a / b; }
    std::string vd2_extra_method() { return "vd2_extra_method"; }
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
    std::string ad1d1_extra_method() { return "ad1d1_extra_method"; }
    AbstractDerived1Derived1(){};
    ~AbstractDerived1Derived1(){};
};

void virtual_tests() {
    printf("--------------- VIRTUAL TESTS ---------------\n");

    VirtualDerived1* vd1 = new VirtualDerived1();
    printf("%d\n", vd1->virt_method(6, 9));
    VirtualBase* vb1 = static_cast<VirtualBase*>(vd1);
    printf("%d\n", vb1->virt_method(6, 9));
    VirtualDerived2* vd2 = static_cast<VirtualDerived2*>(vb1);
    printf("%d\n", vd2->virt_method(6, 9));

    printf("%s\n", vd1->vd1_extra_method().c_str());
    printf("%" PRIu64 "\n", vd1->vd1_extra_method2());
    printf("%s\n", vd2->vd2_extra_method().c_str());
    printf("%" PRIu64 "\n", vd2->vd2_extra_method2());

    printf("%p %p %p\n", vd1, vb1, vd2);

    printf("--------------- END VIRTUAL TESTS ---------------\n");
}

#endif /* VIRTUAL_H */
