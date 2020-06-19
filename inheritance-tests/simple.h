#ifndef SIMPLE_H
#define SIMPLE_H

#include <string>
#include <vector>

class SimpleBase {
public:
    int sb_member1;
    std::string sb_member2;
    SimpleBase(){};
    ~SimpleBase(){};
};

class SimpleDerived1 : public SimpleBase {
public:
    char sd1_member1;
    std::vector<int> sd2_member2;
    SimpleDerived1(){};
    ~SimpleDerived1(){};
};

class SimpleDerived2 : public SimpleBase {
public:
    double sd2_member1;
    std::vector<char> sd2_member2;
    SimpleDerived2(){};
    ~SimpleDerived2(){};
};

class SimpleDerived3 : public SimpleBase {
public:
    int sd3_member1;
    std::string sd3_member2;
    SimpleDerived3(){};
    ~SimpleDerived3(){};
};

void simple_tests() {
    printf("--------------- SIMPLE TESTS ---------------\n");

    // Cast base to derived
    SimpleBase* sb = new SimpleBase();
    SimpleDerived1* sd1 = static_cast<SimpleDerived1*>(sb);
    printf("SimpleBase* sb (%p) casted down to SimpleDerived1* sd1 (%p)\n", sb,
           sd1);

    // Cast derived to base
    SimpleDerived1* sd1_2 = new SimpleDerived1();
    SimpleBase* sb_2 = static_cast<SimpleBase*>(sd1_2);
    printf("SimpleDerived* sd1_2 (%p) casted up to SimpleBase* sb_2 (%p)\n",
           sd1_2, sb_2);

    // Cast from one derived to another
    // // This fails with a compiler error that SimpleDerived2 and
    // // SimpleDerived3 are not related by inheritance.
    // SimpleDerived2* sd2_1 = new SimpleDerived2();
    // SimpleDerived3* sd3_1 = static_cast<SimpleDerived3*>(sd2_1);
    // // However, we can do the same thing indirectly:
    printf("\nIndirectly Casting between siblings:\n");
    printf("Pushed back 0x6f, 0x6f, 0x70, 0x73 to sd2_1->sd2_member2\n");
    SimpleDerived2* sd2_1 = new SimpleDerived2();
    sd2_1->sd2_member2.push_back(0x6f);
    sd2_1->sd2_member2.push_back(0x6f);
    sd2_1->sd2_member2.push_back(0x70);
    sd2_1->sd2_member2.push_back(0x73);
    SimpleBase* sb_3 = static_cast<SimpleBase*>(sd2_1);
    SimpleDerived3* sd3_1 = static_cast<SimpleDerived3*>(sb_3);
    printf("SimpleDerived2* sd2_1 (%p) casted up to SimpleBase* sb_3 (%p), then casted down to SimpleDerived3* sd3_1 (%p)\n", sd2_1, sb_3, sd3_1);
    printf("Value of sd3_1->sd3_member2.cstr(): %s\n\n", sd3_1->sd3_member2.c_str());

    printf("--------------- END SIMPLE TESTS ---------------\n\n");
}

#endif /* SIMPLE_H */
