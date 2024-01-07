#include <CTTP.hpp>
#include <Pulsar/Settings/UI/SettingsPanel.hpp>
#include <Pulsar/IO/IO.hpp>

namespace CTTP {
Pulsar::System::Inherit CTTPCreate(System::Create);

void System::SetUserInfo(Pulsar::Network::ResvInfo::UserInfo& userInfo) {
    userInfo.info[0] = Pulsar::IO::sInstance->type == Pulsar::IOType_DOLPHIN;
}

bool System::CheckUserInfo(const Pulsar::Network::ResvInfo::UserInfo& userInfo) {
    return true; //!userInfo.info[0];
}

void System::AfterInit() {
    ExtProcessMeter::Create();

    Pulsar::UI::SettingsPanel::pageCount++;

    Pulsar::UI::SettingsPanel::radioButtonCount[SETTINGSTYPE_DEBUG] = 2; //debug and perf mon
    Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[SETTINGSTYPE_DEBUG][0] = 4; //disabled, full, light, offroad
    Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[SETTINGSTYPE_DEBUG][1] = 2; //disabled, enabled

    Pulsar::UI::SettingsPanel::scrollerCount[SETTINGSTYPE_DEBUG] = 1;
    Pulsar::UI::SettingsPanel::optionsPerPagePerScroller[SETTINGSTYPE_DEBUG][0] = 4;

    //u32 raceRadioCount = Pulsar::UI::SettingsPanel::radioButtonCount[Pulsar::SETTINGSTYPE_RACE];
    //Pulsar::UI::SettingsPanel::radioButtonCount[Pulsar::SETTINGSTYPE_RACE] = raceRadioCount + 1;
    //Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[Pulsar::SETTINGSTYPE_RACE][raceRadioCount] = 2;

    //u32 scrollerCount = Pulsar::UI::SettingsPanel::scrollerCount[Pulsar::SETTINGSTYPE_RACE];
    //Pulsar::UI::SettingsPanel::scrollerCount[Pulsar::SETTINGSTYPE_RACE] = scrollerCount + 1;
    //Pulsar::UI::SettingsPanel::optionsPerPagePerScroller[Pulsar::SETTINGSTYPE_RACE][scrollerCount] = 4;


}

}//namespace CTTP