#ifndef MULTIPLE_H
#define MULTIPLE_H

#include <string>
#include <vector>

class MultipleNVBase1 {
public:
    int mb1_mem1 = 37;
    std::string mb1_mem2;
    std::vector<uint64_t> mb1_mem3;
    MultipleNVBase1(){};
    virtual ~MultipleNVBase1(){};
    int mb1_base_method() { return 17; }
    virtual int mb1_virt_method() { return 18; }
    virtual int mb1_pure_virt_method() = 0;
    virtual int mb_shared_method() { return 4444; }
};

class MultipleNVBase2 {
public:
    char mb2_mem1 = '@';
    std::vector<int> mb2_mem2;
    MultipleNVBase2(){};
    virtual ~MultipleNVBase2(){};
    int mb2_base_method() { return 19; }
    virtual int mb2_virt_method() { return 20; }
    virtual int mb2_pure_virt_method() = 0;
    virtual int mb_shared_method() { return 55555; }
};

class MultipleNVBase3 {
public:
    double mb3_mem1 = 420.69;
    std::vector<double> mb3_mem2;
    MultipleNVBase3(){};
    virtual ~MultipleNVBase3(){};
    int mb3_base_method() { return 21; }
    virtual int mb3_virt_method() { return 22; }
    virtual int mb3_pure_virt_method() = 0;
    virtual int mb_shared_method() { return 666666; }
};

class MultipleNVDerived12 : public MultipleNVBase1, public MultipleNVBase2 {
public:
    int mnvd12_mem1 = 1212;
    MultipleNVDerived12(){};
    ~MultipleNVDerived12(){};
    int mb1_pure_virt_method() override { return 31; }
    int mb2_pure_virt_method() override { return 32; }
    int mb_shared_method() override { return 7777777; }
};

class MultipleNVDerived23 : public MultipleNVBase2, public MultipleNVBase3 {
public:
    int mnvd23_mem1 = 2323;
    MultipleNVDerived23(){};
    ~MultipleNVDerived23(){};
    int mb2_pure_virt_method() override { return 33; }
    int mb3_pure_virt_method() override { return 34; }
    int mb_shared_method() override { return 8888; }
};

void multiple_nonvirtual_tests() {
    printf("--------------- MULTIPLE (NONVIRTUAL) INHERITANCE TESTS ---------------\n\n");

    {
        printf("Correctly casting from derived to bases:\n");
        MultipleNVDerived12* mnvd12 = new MultipleNVDerived12();
        MultipleNVBase1* mnvb1_1 = static_cast<MultipleNVBase1*>(mnvd12);
        MultipleNVBase2* mnvb2_1 = static_cast<MultipleNVBase2*>(mnvd12);
        printf(
            "VALID: casted MultipleNVDerived12* mnvd12 (%p) up to MultipleNVBase1* mnvb1_1 (%p)\n",
            mnvd12, mnvb1_1);
        printf(
            "VALID: casted MultipleNVDerived12* mnvd12 (%p) up to MultipleNVBase2* mnvb2_1 (%p)\n",
            mnvd12, mnvb2_1);
        printf("mnvd12->mb_shared_method(): %d\n", mnvd12->mb_shared_method());
        printf("mnvb1_1->mb_shared_method(): %d\n", mnvb1_1->mb_shared_method());
        printf("mnvb2_1->mb_shared_method(): %d\n", mnvb2_1->mb_shared_method());
        printf("mnvd12->mb1_base_method(): %d\n", mnvd12->mb1_base_method());
        printf("mnvb1_1->mb1_base_method(): %d\n", mnvb1_1->mb1_base_method());
        printf("mnvb2_1->mb1_base_method(): <doesn't compile>\n");
        printf("\n");
    }

    {
        printf("Indirectly casting between parents:\n");
        MultipleNVDerived12* mnvd12 = new MultipleNVDerived12();
        MultipleNVBase2* mnvb2 = static_cast<MultipleNVBase2*>(mnvd12);
        printf("VALID: Casted MultipleNVDerived12* mnvd12 (%p) up to MultipleNVBase2* mnvb2 (%p)\n", mnvd12, mnvb2);
        MultipleNVDerived23* mnvd23 = static_cast<MultipleNVDerived23*>(mnvb2);
        printf("INVALID: Casted MultipleNVBase2* mnvb2 (%p) down to MultipleNVDerived23* mnvd23 (%p)\n", mnvb2, mnvd23);
        printf("mnvd12->mb_shared_method(): %d\n", mnvd12->mb_shared_method());
        printf("mnvd23->mb_shared_method(): %d\n", mnvd23->mb_shared_method());
        printf("mnvd12->mb2_pure_virt_method(): %d\n", mnvd12->mb2_pure_virt_method());
        printf("mnvd23->mb2_pure_virt_method(): %d\n", mnvd23->mb2_pure_virt_method());
        printf("mnvd12->mb1_mem1: %d\n", mnvd12->mb1_mem1);
        printf("mnvd12->mb2_mem1: %c\n", mnvd12->mb2_mem1);
        printf("mnvd23->mb2_mem1: %c\n", mnvd23->mb2_mem1);
        printf("mnvd23->mb3_mem1: %f\n", mnvd23->mb3_mem1);
        printf("mnvd12->mnvd12_mem1: %d\n", mnvd12->mnvd12_mem1);
        printf("mnvd23->mnvd23_mem1: %d\n", mnvd23->mnvd23_mem1);
        printf("addr of mnvd12->mnvd12_mem1: %p\n", &(mnvd12->mnvd12_mem1));
        printf("addr of mnvd23->mnvd23_mem1: %p\n", &(mnvd23->mnvd23_mem1));
        printf("\n");
    }

    printf("--------------- END MULTIPLE (NONVIRTUAL) INHERITANCE TESTS ---------------\n\n");
}

#endif /* MULTIPLE_H */
