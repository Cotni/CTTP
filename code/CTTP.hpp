#ifndef _CTTP_
#define _CTTP_
#include <game/File/RKG.hpp>
#include <game/Race/racedata.hpp>
#include <Pulsar/PulsarSystem.hpp>
#include <Pulsar/SlotExpansion/CupsDef.hpp>
#include <Race/ItemCheatStream.hpp>
#include <Debug/PerfMon.hpp>

namespace CTTP {
class System : public Pulsar::System {
public:
    static Pulsar::System* Create() { return new System; }
    static System& Get() { return static_cast<System&>(*sInstance); }
    void SetUserInfo(Pulsar::Network::ResvInfo::UserInfo& userInfo) override;
    bool CheckUserInfo(const Pulsar::Network::ResvInfo::UserInfo& userInfo) override;
    void AfterInit() override;
    static bool IsItemHackSituation() {
        bool isCTs = !Pulsar::CupsDef::IsRegsSituation();
        const RaceData* racedata = RaceData::sInstance;
        GameMode gamemode = racedata->racesScenario.settings.gamemode;
        return isCTs && (gamemode < 7 || gamemode > 10);
    }

    ItemId cheatedItem[4];
    ItemCheatStream streams[4];
    u32 droppedFrames;
};

enum SettingsType {
    SETTINGSTYPE_DEBUG = 3
};

enum DebugSettings {
    SETTINGDEBUG_RADIO_PANEL = 0,
    SETTINGDEBUG_RADIO_PERFMON = 1,

    SETTINGDEBUG_SCROLL_KMP = 0 + 6,
};


enum DebugSettingPANEL {
    DEBUGSETTING_PANEL_DISABLED = 0x0,
    DEBUGSETTING_PANEL_LIGHT = 0x1,
    DEBUGSETTING_PANEL_FULL = 0x2,
    DEBUGSETTING_PANEL_OFFROAD = 0x3
};

enum DebugSettingPERFMON {
    DEBUGSETTING_PERFMON_ENABLED = 0x0,
    DEBUGSETTING_PERFMON_DISABLED = 0x1
};

enum DebugSettingKMP {
    DEBUGSETTING_KMP_DISABLED = 0x0,
    DEBUGSETTING_KMP_ALLCPS = 0x1,
    DEBUGSETTING_KMP_KEYCPS = 0x2,
    DEBUGSETTING_KMP_ITPTS = 0x3
};

}//namespace CTTP



#endif