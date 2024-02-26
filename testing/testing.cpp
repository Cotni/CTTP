#include <kamek.hpp>
#include <MarioKartWii/Objects/KCL/Aurora.hpp>

void DisableBoostRamp(KCLTypeHolder* holder, KCLBitfield bitfield, u16 kclFlag, float distance) {
    register Objects::Aurora* aurora;
    asm(mr aurora, r29;);
    if (aurora->gobj.raw->settings[0] != 0) {
        bitfield = KCL_BITFIELD_ROAD2;
        kclFlag = 0xf7; //flag 0x17 variant 7 for sfx
    }
    holder->ApplyFlagAndRegisterEntry(bitfield, kclFlag, distance);
}
kmCall(0x807fc198, DisableBoostRamp);