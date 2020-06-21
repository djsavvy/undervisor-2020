#ifndef MULTIPLE_H
#define MULTIPLE_H

#include <string>
#include <vector>

class MultipleBase1 {
public:
    int mb1_mem1 = 37;
    std::string mb1_mem2;
    std::vector<uint64_t> mb1_mem3;
    MultipleBase1(){};
    virtual ~MultipleBase1(){};
    int mb1_base_method() { return 17; }
    virtual int mb1_virt_method() { return 18; }
    virtual int mb1_pure_virt_method() = 0;
    virtual int mb_shared_method() { return 4444; }
};

class MultipleBase2 {
public:
    char mb2_mem1 = '@';
    std::vector<int> mb2_mem2;
    MultipleBase2(){};
    virtual ~MultipleBase2(){};
    int mb2_base_method() { return 19; }
    virtual int mb2_virt_method() { return 20; }
    virtual int mb2_pure_virt_method() = 0;
    virtual int mb_shared_method() { return 55555; }
};

class MultipleBase3 {
public:
    double mb3_mem1 = 420.69;
    std::vector<double> mb3_mem2;
    MultipleBase3(){};
    virtual ~MultipleBase3(){};
    int mb3_base_method() { return 21; }
    virtual int mb3_virt_method() { return 22; }
    virtual int mb3_pure_virt_method() = 0;
    virtual int mb_shared_method() { return 666666; }
};

class MultipleDerived12 : public MultipleBase1, public MultipleBase2 {
public:
    int md12_mem1 = 1212;
    MultipleDerived12(){};
    ~MultipleDerived12(){};
    int mb1_pure_virt_method() override { return 31; }
    int mb2_pure_virt_method() override { return 32; }
    int mb_shared_method() override { return 7777777; }
};

class MultipleDerived23 : public MultipleBase2, public MultipleBase3 {
public:
    int md23_mem1 = 2323;
    MultipleDerived23(){};
    ~MultipleDerived23(){};
    int mb2_pure_virt_method() override { return 33; }
    int mb3_pure_virt_method() override { return 34; }
    int mb_shared_method() override { return 8888; }
};

void multiple_tests() {
    printf("--------------- MULTIPLE INHERITANCE TESTS ---------------\n\n");

    {
        printf("Correctly casting from derived to bases:\n");
        MultipleDerived12* md12 = new MultipleDerived12();
        MultipleBase1* mb1_1 = static_cast<MultipleBase1*>(md12);
        MultipleBase2* mb2_1 = static_cast<MultipleBase2*>(md12);
        printf("VALID: casted MultipleDerived12* md12 (%p) up to MultipleBase1* mb1_1 (%p)\n",
               md12, mb1_1);
        printf("VALID: casted MultipleDerived12* md12 (%p) up to MultipleBase2* mb2_1 (%p)\n",
               md12, mb2_1);
        printf("md12->mb_shared_method(): %d\n", md12->mb_shared_method());
        printf("mb1_1->mb_shared_method(): %d\n", mb1_1->mb_shared_method());
        printf("mb2_1->mb_shared_method(): %d\n", mb2_1->mb_shared_method());
        printf("md12->mb1_base_method(): %d\n", md12->mb1_base_method());
        printf("mb1_1->mb1_base_method(): %d\n", mb1_1->mb1_base_method());
        printf("mb2_1->mb1_base_method(): <doesn't compile>\n");
        printf("\n");
    }

    {
        printf("Indirectly casting between parents:\n");
        MultipleDerived12* md12 = new MultipleDerived12();
        MultipleBase2* mb2 = static_cast<MultipleBase2*>(md12);
        printf("VALID: Casted MultipleDerived12* md12 (%p) up to MultipleBase2* mb2 (%p)\n",
               md12, mb2);
        MultipleDerived23* md23 = static_cast<MultipleDerived23*>(mb2);
        printf("IALID: Casted MultipleBase2* mb2 (%p) down to MultipleDerived23* md23 (%p)\n",
               mb2, md23);
        printf("md12->mb_shared_method(): %d\n", md12->mb_shared_method());
        printf("md23->mb_shared_method(): %d\n", md23->mb_shared_method());
        printf("md12->mb2_pure_virt_method(): %d\n", md12->mb2_pure_virt_method());
        printf("md23->mb2_pure_virt_method(): %d\n", md23->mb2_pure_virt_method());
        printf("md12->mb1_mem1: %d\n", md12->mb1_mem1);
        printf("md12->mb2_mem1: %c\n", md12->mb2_mem1);
        printf("md23->mb2_mem1: %c\n", md23->mb2_mem1);
        printf("md23->mb3_mem1: %f\n", md23->mb3_mem1);
        printf("md12->md12_mem1: %d\n", md12->md12_mem1);
        printf("md23->md23_mem1: %d\n", md23->md23_mem1);
        printf("addr of md12->md12_mem1: %p\n", &(md12->md12_mem1));
        printf("addr of md23->md23_mem1: %p\n", &(md23->md23_mem1));
        printf("\n");
    }

    printf("--------------- END MULTIPLE INHERITANCE TESTS ---------------\n\n");
}

#endif /* MULTIPLE_H */
