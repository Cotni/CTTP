#ifndef _CTTP_
#define _CTTP_
#include <MarioKartWii/File/RKG.hpp>
#include <MarioKartWii/Race/racedata.hpp>
#include <PulsarEngine/PulsarSystem.hpp>
#include <PulsarEngine/Ghost/GhostManager.hpp>
#include <PulsarEngine/SlotExpansion/CupsConfig.hpp>
#include <ItemCheat/ItemCheatStream.hpp>
#include <Debug/PerfMon.hpp>

namespace CTTP {



class System : public Pulsar::System {
    static void CheckGhostCheatStatus(const RKG& decompressed, Pulsar::Ghosts::Manager::CBTiming timing, int idx);

public:
    static Pulsar::System* Create() { return new System; }
    static System& Get() { return static_cast<System&>(*sInstance); }
    static const u32 magic = 'CTTP';
    void SetUserInfo(Pulsar::Network::ResvInfo::UserInfo& userInfo) override;
    bool CheckUserInfo(const Pulsar::Network::ResvInfo::UserInfo& userInfo) override;
    void AfterInit() override;
    static bool IsItemHackSituation() {
        bool isCTs = !Pulsar::CupsConfig::IsRegsSituation();
        const RaceData* racedata = RaceData::sInstance;
        GameMode gamemode = racedata->racesScenario.settings.gamemode;
        return isCTs && (gamemode < 7 || gamemode > 10);
    }

    ItemId cheatedItem[4];
    ItemCheatStream streams[4];
    u32 droppedFrames;
    bool isCheatedGhost[38];

};

struct RKGSection {
    RKGSection() : magic(System::magic) {
        padding[0] = 0;
        padding[1] = 0;
        padding[2] = 0;
    };
    u32 magic;
    bool hadItemBoxes;
    u8 padding[3];
    u32 seed;
};

enum SettingsType {
    SETTINGSTYPE_DEBUG = 3
};

enum DebugSettings {
    SETTINGDEBUG_RADIO_PANEL = 0,
    SETTINGDEBUG_RADIO_PERFMON = 1,
    SETTINGDEBUG_RADIO_ITEMCHEATS = 2,

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

enum DebugSettingITEMCHEATS {
    DEBUGSETTING_ITEMCHEATS_ENABLED = 0x0,
    DEBUGSETTING_ITEMCHEATS_DISABLED = 0x1
};

enum DebugSettingKMP {
    DEBUGSETTING_KMP_DISABLED = 0x0,
    DEBUGSETTING_KMP_ALLCPS = 0x1,
    DEBUGSETTING_KMP_KEYCPS = 0x2,
    DEBUGSETTING_KMP_ITPTS = 0x3
};

}//namespace CTTP



#endif