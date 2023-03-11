#include <kamek.hpp>
#include <game/Race/racedata.hpp>
#include <game/Network/RKNetController.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceBalloon.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceResult.hpp>
#include <game/Item/ItemManager.hpp>
#include <SkillIssuePack.hpp>
#include <UserData/CTTPData.hpp>


//From JoshuaMK, ported to C++ by Brawlbox and adapted as a setting
int MiiHeads(RaceData *racedata, int r4, int r5, u8 id){
    u32 charId = racedata->racesScenario.players[id].characterId;
    if (SIPDataHolder::GetInstance()->GetSettings()->pages[CTTP_RACE_SETTINGS].radioSetting[CTTP_RADIO_MII] == CTTP_RADIO_MII_ENABLED){
        if (charId < 0x2a){
            if (id == 0) charId = 0x2a;
            else if(RKNetController::sInstance->connectionState != 0) charId = 0x2a;
        }
    }
    return charId;
}
kmCall(0x807eb154, MiiHeads);
kmWrite32(0x807eb15c, 0x60000000);
kmWrite32(0x807eb160, 0x88de01b4);

//credit to XeR for finding the float address
void BattleGlitchEnable(){ 
    float maxDistance = 7500.0;
    if(SIPDataHolder::GetInstance()->GetSettings()->pages[CTTP_RACE_SETTINGS].radioSetting[CTTP_RADIO_BATTLE] == CTTP_RADIO_BATTLE_GLITCH_ENABLED) maxDistance = 75000.0;
    RaceBalloons::maxDistanceNames = maxDistance;
}
static SettingsHook BattleGlitch(BattleGlitchEnable);


kmWrite32(0x8085C914 ,0x38000000); //times at the end of races in VS
void DisplayTimesInsteadOfNames(CtrlRaceResult *result, u8 id){
    result->DisplayFinishTime(id);
}
kmCall(0x8085d460, DisplayTimesInsteadOfNames); //for WWs

//don't hide position tracker (MrBean35000vr)
kmWrite32(0x807F4DB8, 0x38000001); 
