#ifndef DIAMOND_VIRTUAL_H
#define DIAMOND_VIRTUAL_H

#include <string>
#include <vector>

class DiamondVirtualBase {
public:
    int dvb_mem1 = 100;
    int dvb_mem2 = 101;
    std::string dvb_mem3 = "DiamondVirtualBase";
    DiamondVirtualBase(){};
    virtual ~DiamondVirtualBase(){};
    int dvb_method() { return dvb_mem2; }
    virtual int dvb_virt_method() { return 102; }
    virtual int dvb_pure_virt_method() = 0;
    virtual std::string dvb_shared_method() = 0;
};

class DiamondVirtualIntermediate1 : virtual public DiamondVirtualBase {
public:
    int dvi1_mem1 = 103;
    DiamondVirtualIntermediate1(){};
    ~DiamondVirtualIntermediate1(){};
    int dvi1_method() { return dvi1_mem1; }
    int dvb_virt_method() override { return 104; }
    int dvb_pure_virt_method() override { return 105; }
    std::string dvb_shared_method() override { return "DiamondVirtualIntermediate1"; }
};

class DiamondVirtualIntermediate2 : virtual public DiamondVirtualBase {
public:
    int dvi2_mem1;
    DiamondVirtualIntermediate2() : dvi2_mem1(106){};
    ~DiamondVirtualIntermediate2(){};
    int dvi2_method() { return dvi2_mem1; }
    int dvi2_method1() { return dvi2_mem1; }
    int dvi2_method2() { return dvi2_mem1; }
    int dvi2_method3() { return dvi2_mem1; }
    int dvi2_method4() { return dvi2_mem1; }
    int dvi2_method5() { return dvi2_mem1; }
    int dvi2_method6() { return dvi2_mem1; }
    int dvb_virt_method() override { return 107; }
    int dvb_pure_virt_method() override { return 108; }
    std::string dvb_shared_method() override { return "DiamondVirtualIntermediate2"; }
};

class DiamondVirtualDerived : public DiamondVirtualIntermediate1,
                              public DiamondVirtualIntermediate2 {
public:
    int dvd_mem1 = 109;
    DiamondVirtualDerived(){};
    ~DiamondVirtualDerived(){};
    int dvd_method() { return dvd_mem1; }
    int dvb_virt_method() override { return 110; }
    int dvb_pure_virt_method() override { return 111; }
    std::string dvb_shared_method() override { return "DiamondVirtualDerived"; }
};

void diamond_virtual_tests() {
    printf("--------------- DIAMOND VIRTUAL INHERITANCE TESTS ---------------\n\n");

    printf(
        "NOTE: Cannot cast between intermediate siblings DiamondVirtualIntermediate{1,2} via "
        "virtual parent DiamondVirtualBase\n");
    printf("Casting between siblings through child:\n");
    DiamondVirtualIntermediate1* dvi1 = new DiamondVirtualIntermediate1();
    DiamondVirtualDerived* dvd = static_cast<DiamondVirtualDerived*>(dvi1);
    printf(
        "INVALID: Casted DiamondVirtualIntermediate1* dvi1 (%p) down to DiamondVirtualDerived* "
        "dvd (%p)\n",
        dvi1, dvd);
    DiamondVirtualIntermediate2* dvi2 = static_cast<DiamondVirtualIntermediate2*>(dvd);
    printf(
        "INVALID: Casted DiamondVirtualDerived* dvd (%p) up to DiamondVirtualIntermediate2* "
        "dvi2 (%p)\n",
        dvd, dvi2);
    printf("dvi1->dvb_shared_method().c_str(): %s\n", dvi1->dvb_shared_method().c_str());
    printf("dvd->dvb_shared_method().c_str(): %s\n", dvd->dvb_shared_method().c_str());
    printf("dvi2->dvb_shared_method().c_str(): %s\n", dvi2->dvb_shared_method().c_str());
    printf("dvi1->dvi1_method(): %d\n", dvi1->dvi1_method());
    printf("dvd->dvi1_method(): %d\n", dvd->dvi1_method());
    printf("dvi2->dvi2_method(): %d\n", dvi2->dvi2_method());
    printf("dvd->dvi2_method(): %d\n", dvd->dvi2_method());
    printf("dvi2->dvi2_method6(): %d\n", dvi2->dvi2_method6());
    printf("dvd->dvi2_method6(): %d\n", dvd->dvi2_method6());
    printf("dvd->dvd_method(): %d\n", dvd->dvd_method());
    printf("dvd->DiamondVirtualIntermediate1::dvb_method(): %d\n", dvd->DiamondVirtualIntermediate1::dvb_method());
    printf("dvd->DiamondVirtualIntermediate2::dvb_method(): %d\n", dvd->DiamondVirtualIntermediate2::dvb_method());
    printf("\n");

    printf("--------------- END DIAMOND VIRTUAL INHERITANCE TESTS ---------------\n\n");
}

#endif /* DIAMOND_VIRTUAL_H */
