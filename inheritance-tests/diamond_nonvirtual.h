#ifndef DIAMOND_NONVIRTUAL_H
#define DIAMOND_NONVIRTUAL_H

#include <string>
#include <vector>

class DiamondNonVirtualBase {
public:
    int dnvb_mem1 = 200;
    std::string dnvb_mem2 = "DiamondNonVirtualBase";
    DiamondNonVirtualBase(){};
    virtual ~DiamondNonVirtualBase(){};
    int dnvb_method() { return 201; }
    virtual int dnvb_virt_method() { return 202; }
    virtual int dnvb_pure_virt_method() = 0;
    virtual std::string dnvb_shared_method() = 0;
};

class DiamondNonVirtualIntermediate1 : public DiamondNonVirtualBase {
public:
    int dnvi1_mem1 = 203;
    DiamondNonVirtualIntermediate1(){};
    ~DiamondNonVirtualIntermediate1(){};
    int dnvi1_method() { return dnvi1_mem1; }
    int dnvb_virt_method() override { return 204; }
    int dnvb_pure_virt_method() override { return 205; }
    std::string dnvb_shared_method() override { return "DiamondNonVirtualIntermediate1"; }
};

class DiamondNonVirtualIntermediate2 : public DiamondNonVirtualBase {
public:
    int dnvi2_mem1;
    DiamondNonVirtualIntermediate2() : dnvi2_mem1(206){};
    ~DiamondNonVirtualIntermediate2(){};
    int dnvi2_method() { return dnvi2_mem1; }
    int dnvi2_method1() { return dnvi2_mem1; }
    int dnvi2_method2() { return dnvi2_mem1; }
    int dnvi2_method3() { return dnvi2_mem1; }
    int dnvi2_method4() { return dnvi2_mem1; }
    int dnvi2_method5() { return dnvi2_mem1; }
    int dnvi2_method6() { return dnvi2_mem1; }
    int dnvb_virt_method() override { return 207; }
    int dnvb_pure_virt_method() override { return 208; }
    std::string dnvb_shared_method() override { return "DiamondNonVirtualIntermediate2"; }
};

class DiamondNonVirtualDerived : public DiamondNonVirtualIntermediate1,
                                 public DiamondNonVirtualIntermediate2 {
public:
    int dnvd_mem1 = 209;
    DiamondNonVirtualDerived(){};
    ~DiamondNonVirtualDerived(){};
    int dnvd_method() { return dnvd_mem1; }
    int dnvb_virt_method() override { return 210; }
    int dnvb_pure_virt_method() override { return 211; }
    std::string dnvb_shared_method() override { return "DiamondNonVirtualDerived"; }
};

void diamond_nonvirtual_tests() {
    printf("--------------- DIAMOND NONVIRTUAL INHERITANCE TESTS ---------------\n\n");

    {
        printf("Casting between siblings through parent:\n");
        DiamondNonVirtualIntermediate1* dnvi1 = new DiamondNonVirtualIntermediate1();
        DiamondNonVirtualBase* dnvb = static_cast<DiamondNonVirtualBase*>(dnvi1);
        printf(
            "VALID: Casted DiamondNonVirtualIntermediate1* dnvi1 (%p) up to DiamondNonVirtualBase* "
            "dnvb (%p)\n",
            dnvi1, dnvb);
        DiamondNonVirtualIntermediate2* dnvi2 = static_cast<DiamondNonVirtualIntermediate2*>(dnvb);
        printf(
            "INVALID: Casted DiamondNonVirtualBase* dnvb (%p) down to "
            "DiamondNonVirtualIntermediate2* "
            "dnvi2 (%p)\n",
            dnvb, dnvi2);

        printf("dnvi1->dnvi1_method(): %d\n", dnvi1->dnvi1_method());
        printf("dnvi2->dnvi2_method(): %d\n", dnvi2->dnvi2_method());
        printf("\n");
    }

    {
        printf("Casting between siblings through child:\n");
        DiamondNonVirtualIntermediate1* dnvi1 = new DiamondNonVirtualIntermediate1();
        DiamondNonVirtualDerived* dnvd = static_cast<DiamondNonVirtualDerived*>(dnvi1);
        printf(
            "INVALID: Casted DiamondNonVirtualIntermediate1* dnvi1 (%p) down to "
            "DiamondNonVirtualDerived* "
            "dnvd (%p)\n",
            dnvi1, dnvd);
        DiamondNonVirtualIntermediate2* dnvi2 = static_cast<DiamondNonVirtualIntermediate2*>(dnvd);
        printf(
            "INVALID: Casted DiamondNonVirtualDerived* dnvd (%p) up to "
            "DiamondNonVirtualIntermediate2* "
            "dnvi2 (%p)\n",
            dnvd, dnvi2);
        printf("dnvi1->dnvb_shared_method().c_str(): %s\n", dnvi1->dnvb_shared_method().c_str());
        printf("dnvd->dnvb_shared_method().c_str(): %s\n", dnvd->dnvb_shared_method().c_str());
        printf("dnvi2->dnvb_shared_method().c_str(): <segfaults>\n");
        printf("dnvi1->dnvi1_method(): %d\n", dnvi1->dnvi1_method());
        printf("dnvd->dnvi1_method(): %d\n", dnvd->dnvi1_method());
        printf("dnvi2->dnvi2_method(): %d\n", dnvi2->dnvi2_method());
        printf("dnvd->dnvi2_method(): %d\n", dnvd->dnvi2_method());
        printf("dnvi2->dnvi2_method6(): %d\n", dnvi2->dnvi2_method6());
        printf("dnvd->dnvi2_method6(): %d\n", dnvd->dnvi2_method6());
        printf("dnvd->dnvd_method(): %d\n", dnvd->dnvd_method());
        printf("\n");
    }

    printf("--------------- END DIAMOND NONVIRTUAL INHERITANCE TESTS ---------------\n\n");
}

#endif /* DIAMOND_NONVIRTUAL_H */
