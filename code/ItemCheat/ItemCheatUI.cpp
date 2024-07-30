#include <CTTP.hpp>
#include <UI/CTTPUI.hpp>
#include <PulsarEngine/Settings/UI/SettingsPanel.hpp>
#include <PulsarEngine/Ghost/GhostManager.hpp>
#include <PulsarEngine/Ghost/UI/GhostUI.hpp>
#include <PulsarEngine/UI/UI.hpp>
#include <PulsarEngine/IO/IO.hpp>

namespace CTTP {
namespace UI {


bool PatchBMGOnArrow(Pulsar::UI::ExpGhostSelect* ghostSel, u32 bmgId, Text::Info* info) {
    ghostSel->pageNumber.SetMessage(bmgId, info);

    const GhostListEntry& entry = ghostSel->ghostList->entries[ghostSel->page];
    const Pulsar::Ghosts::Manager* mgr = Pulsar::Ghosts::Manager::GetInstance();
    int idx = 0;
    for (int i = 0; i < mgr->GetRKGCount(); ++i) {
        if (entry.data == &mgr->GetGhostData(i)) {
            idx = i;
            break;
        }
    }
    bool isCheated = System::Get().isCheatedGhost[idx];
    bool changeWatch = isCheated && (mgr->GetSelGhostIdx(0) != 0xff || mgr->GetSelGhostIdx(1) != 0xff || mgr->GetSelGhostIdx(2) != 0xff);

    u32 challengeBmg = Pulsar::UI::BMG_CHALLENGE_GHOST;
    u32 watchBmg = Pulsar::UI::BMG_WATCH_REPLAY;
    if (isCheated) challengeBmg = BMG_CANNOT_RACE_GHOST;
    if (changeWatch) watchBmg = BMG_CANNOT_WATCH;

    PushButton* challengeButton = &ghostSel->challengeGhostButton;
    challengeButton->manipulator.inaccessible = isCheated;
    challengeButton->SetMessage(challengeBmg);
    PushButton* watchButton = &ghostSel->watchReplayButton;
    watchButton->manipulator.inaccessible = changeWatch;
    watchButton->SetMessage(watchBmg);
    ghostSel->ToggleSelectButton(isCheated);
    return isCheated;
}

kmWrite32(0x8063a268, 0x7fc3f378);
kmCall(0x8063a288, PatchBMGOnArrow);
kmWrite32(0x8063a364, 0x7fc3f378);
kmCall(0x8063a384, PatchBMGOnArrow);

void PatchBMGOnActivate(Pulsar::UI::ExpGhostSelect* ghostSel, u32 bmgId, Text::Info* info) {
    bool isCheated = PatchBMGOnArrow(ghostSel, bmgId, info);
    PushButton* initial = isCheated ? &ghostSel->soloTTButton : &ghostSel->challengeGhostButton;
    initial->SelectInitialButton(0);
}
kmWrite32(0x80639cf4, 0x7fa3eb78);
kmCall(0x80639d14, PatchBMGOnActivate);

void PreventCheatGhostRacing(Pages::GhostManager* ghostMgr, bool isRace, bool isStaffGhost) {
    System& system = System::Get();
    if (system.cheatedItem[0] != 0xff) return;
    else ghostMgr->PrepareRaceState(isRace, isStaffGhost);
}
kmCall(0x8085b29c, PreventCheatGhostRacing);
kmCall(0x8085b6b0, PreventCheatGhostRacing);

}//namespace UI
}//namespace CTTP