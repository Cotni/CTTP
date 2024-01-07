
#include <hooks.hpp>
#include <types.hpp>
void OpaTest() {
    return;
}
kmCall(0x80001500, OpaTest);


int OpaTest2() {
    return 2;
}
kmCall(0x805201ac, OpaTest2);
