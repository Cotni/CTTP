#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceTime.hpp>
#include <UI/CtrlRaceBase/DebugPanel.hpp>
#include <PulsarEngine/SlotExpansion/CupsConfig.hpp>
#include <PulsarEngine/Settings/Settings.hpp>
#include <PulsarEngine/IO/IO.hpp>


namespace CTTP {
namespace UI {
u32 CtrlRaceDebug::Count() {
    GameMode mode = RaceData::sInstance->racesScenario.settings.gamemode;
    bool isBattle = mode == MODE_BATTLE || mode == MODE_PUBLIC_BATTLE || mode == MODE_PRIVATE_BATTLE;
    bool isOnlineVS = mode == MODE_PUBLIC_VS || mode == MODE_PRIVATE_VS;

    bool isVisible;
    if (isBattle) isVisible = false;
    else if (isOnlineVS) isVisible = true;
    else isVisible = Pulsar::Settings::Mgr::GetSettingValue(static_cast<Pulsar::Settings::Type>(SETTINGSTYPE_DEBUG), SETTINGDEBUG_RADIO_PANEL) != DEBUGSETTING_PANEL_DISABLED;
    return isVisible;
}

void CtrlRaceDebug::Create(Page& page, u32 index, u32 count) {
    CtrlRaceDebug* debug = new(CtrlRaceDebug);
    page.AddControl(index, *debug, 0);
    DebugSettingPANEL type = static_cast<DebugSettingPANEL>(Pulsar::Settings::Mgr::GetSettingValue(static_cast<Pulsar::Settings::Type>(SETTINGSTYPE_DEBUG),
        SETTINGDEBUG_RADIO_PANEL));

    GameMode mode = RaceData::sInstance->racesScenario.settings.gamemode;
    if (mode >= MODE_PRIVATE_VS && mode <= MODE_PRIVATE_BATTLE) {
        type = DEBUGSETTING_PANEL_LIGHT;
    }
    debug->Load(type);
}
static Pulsar::UI::CustomCtrlBuilder Debug(CtrlRaceDebug::Count, CtrlRaceDebug::Create);

void CtrlRaceDebug::Load(DebugSettingPANEL type) {
    this->hudSlotId = 0;
    this->type = type;

    ControlLoader loader(this);
    const char* brctr = "DebugFull";
    if (type == DEBUGSETTING_PANEL_LIGHT) brctr = "DebugLight";
    else if (type == DEBUGSETTING_PANEL_OFFROAD) brctr = "DebugOffroad";
    loader.Load(Pulsar::UI::raceFolder, brctr, brctr, nullptr);
    return;
}

void CtrlRaceDebug::InitSelf() {
    const u8 id = RaceData::sInstance->menusScenario.settings.hudPlayerIds[this->hudSlotId];
    this->minOffSpeed = Kart::Manager::sInstance->players[id]->pointers.kartMovement->baseSpeed;
}

void CtrlRaceDebug::OnUpdate() {
    this->UpdatePausePosition();
    const u8 id = this->GetPlayerId();
    const RaceInfoPlayer* raceinfoPlayer = RaceInfo::sInstance->players[id];
    const s8 iterator = 0x0;
    const u8 respawn = raceinfoPlayer->respawn;
    const u8 cp = raceinfoPlayer->checkpoint;
    const u8 kcp = raceinfoPlayer->currentKCP;
    const u8 maxkcp = raceinfoPlayer->maxKCP;
    const float raceComp = raceinfoPlayer->raceCompletion;
    const u8 ITPT = Item::Manager::sInstance->players[id].itemPoint.ITPT;

    const Kart::Pointers& pointers = Kart::Manager::sInstance->players[id]->pointers;
    const Audio::KartActor* kartActor = pointers.kartActor;
    const u16 kclFlag = kartActor->kclFlag;
    const u16 kclVariant = kartActor->variant;

    const char* trick = "Can't  Trick";
    const u32 frontCollisionType = pointers.wheels[0]->wheelPhysics->hitboxGroup->collisionData.types;
    const u32 backCollisionType = pointers.wheels[1]->wheelPhysics->hitboxGroup->collisionData.types;
    if (kclFlag == 0x7 || kclFlag == 0x6 || (frontCollisionType & 0x8) || (backCollisionType & 0x8)) trick = "Can  Trick";

    const char* drop = KMP::Manager::sInstance->itptSection->holdersArray[ITPT]->raw->setting2 & 1 != 0 ? "Can't Drop" : "Can  Drop";

    wchar_t debugPanel[0x200];
    const wchar_t* format;
    u32 length = 0;


    bool isConsole = Pulsar::IO::sInstance->type != Pulsar::IOType_DOLPHIN;
    if (type == DEBUGSETTING_PANEL_FULL) {
        const Vec3& position = pointers.kartBody->kartPhysicsHolder->physics->position;
        if (isConsole) {
            format = L"X %5.0f\nY %5.0f\nZ %5.0f\nRespawn %2d\nCurCP    %3d\nCur/MaxKCP %2d %2d\n"
                "Race%%    %01.3f\nITPT      %3d\nKCL %02x %02x\n%s\n%s\nFrames    %3d";
        }
        else {
            format = L"X %5.0f\nY %5.0f\nZ %5.0f\nRespawn %2d\nCurCP    %3d\nCur/MaxKCP %2d %2d\n"
                "Race%%    %01.3f\nITPT      %3d\nKCL %02x %02x\n%s\n%s";
        }

        length = swprintf(debugPanel, 0x200, format, position.x, position.y, position.z, respawn, cp, kcp, maxkcp,
            raceComp, ITPT, kclFlag, kclVariant, trick, drop, System::Get().droppedFrames);
    }
    else if (type == DEBUGSETTING_PANEL_LIGHT) {
        if (isConsole) {
            format = L"Respawn %2d\nCurCP    %3d\nCurKCP   %2d\nRace%%    %01.3f\nITPT      %3d\nKCL %02x %02x\n%s\n%s\nFrames    %3d";
        }
        else {
            format = L"Respawn %2d\nCurCP    %3d\nCurKCP   %2d\nRace%%    %01.3f\nITPT      %3d\nKCL %02x %02x\n%s\n%s";
        }

        length = swprintf(debugPanel, 0x200, format, respawn, cp, maxkcp,
            raceComp, ITPT, kclFlag, kclVariant, trick, drop, System::Get().droppedFrames);
    }
    else {
        format = L"Off Frames    %4d\nKCLFlag Mul %03.1f%%\nMin Off Speed %3.1f";
        const Kart::Movement* mov = pointers.kartMovement;
        const float curSpeed = nw4r::ut::Abs(mov->engineSpeed);
        const float curMult = mov->kclSpeedFactor * 100.0f;
        if (kclFlag >= 2 && kclFlag <= 4 && curMult < 100.0f && curSpeed < mov->baseSpeed) {
            ++this->framesOffroad;
            this->layout.textPaneHandlerArray[0].pane->colour1 = 0xEA0D0DFF;
            if (curSpeed < this->minOffSpeed) this->minOffSpeed = curSpeed;
        }
        length = swprintf(debugPanel, 0x200, format, this->framesOffroad, curMult, this->minOffSpeed);
    }


    for (u32 i = 0; i < length; ++i) if (debugPanel[i] >= 0x30 && debugPanel[i] <= 0x39) debugPanel[i] += 0x2430;
    TextInfo text;
    text.strings[0] = debugPanel;
    this->SetMessage(Pulsar::UI::BMG_TEXT, &text);
}
}//namespace UI
}//namespace CTTP