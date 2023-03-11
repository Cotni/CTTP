#include <kamek.hpp>
#include <game/UI/Ctrl/PushButton.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceWifi.hpp>
#include <Debug/CTTPDebug.hpp>
#include <UI/BMG.hpp>

namespace CTTPUI{
//No ghost saving
kmWrite32(0x8054913C, 0x60000000);
kmWrite32(0x80855f48, 0x48000148);

//BMG size patch (Diamond)
kmWrite32(0x8007B37C, 0x38000128);


//Credit to Kazuki for making the original ASM code, and Brawlbox for porting it to C++
void LaunchRiivolutionButton(MenuData *menuData){
    MenuId id = menuData->nextMenuId;
    if (id == CHANNEL_FROM_MENU || id == CHANNEL_FROM_CHECK_RANKINGS || id == CHANNEL_FROM_DOWNLOADS) CTTPDebug::LaunchSoftware();
    else menuData->LoadNextMenu();
}
kmCall(0x80553a60, LaunchRiivolutionButton);
} //namespace SIPUI