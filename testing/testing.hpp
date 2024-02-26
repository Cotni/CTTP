
#include <kamek.hpp>

struct C;

struct A {
    typedef u32(A::* ptrs);
    A(int a, int b) : offsetToC(a), offsetToB(b) {}
    u32 offsetToC;
    u32 offsetToB;

    float c;
    template<typename T>
    u32 GetInt() {
        return this->*T::offsetz;
    }
};

struct C {
    static const A::ptrs offset;
    int ints[10];
};