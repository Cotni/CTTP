#include <CTTP.hpp>
#include <UI/CTTPUI.hpp>
#include <PulsarEngine/Settings/UI/SettingsPanel.hpp>
#include <PulsarEngine/Ghost/UI/GhostUI.hpp>
#include <PulsarEngine/UI/UI.hpp>
#include <PulsarEngine/IO/IO.hpp>
#include <MarioKartWii/Objects/ObjectsMgr.hpp>

namespace CTTP {
Pulsar::System::Inherit CTTPCreate(System::Create);

void System::SetUserInfo(Pulsar::Network::ResvInfo::UserInfo& userInfo) {
    userInfo.info[0] = Pulsar::IO::sInstance->type == Pulsar::IOType_RIIVO;
}

bool System::CheckUserInfo(const Pulsar::Network::ResvInfo::UserInfo& userInfo) {
    if (Pulsar::Settings::Mgr::GetSettingValue(static_cast<Pulsar::Settings::Type>(SETTINGSTYPE_DEBUG), SETTINGDEBUG_RADIO_BLOCK_EMU) == DEBUGSETTING_EMU_BLOCKED)
        return userInfo.info[0];
    return true;
}

void System::AfterInit() {
    Pulsar::Ghosts::Manager::SetCb(&System::CheckGhostCheatStatus);
    //ExtProcessMeter::Create();
    EGG::ProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->xOrigin = 26.0f;
    meter->yOrigin = 1.0f;
    meter->xSize = 60.0f;
    meter->ySize = 0.75f;

    Pulsar::UI::SettingsPanel::pageCount++;

    Pulsar::UI::SettingsPanel::radioButtonCount[SETTINGSTYPE_DEBUG] = 5; //debug, perf mon, item cheats, block emu, tt boxes
    Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[SETTINGSTYPE_DEBUG][0] = 4; //disabled, full, light, offroad
    Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[SETTINGSTYPE_DEBUG][1] = 2;
    Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[SETTINGSTYPE_DEBUG][2] = 2;
    Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[SETTINGSTYPE_DEBUG][3] = 2;
    Pulsar::UI::SettingsPanel::buttonsPerPagePerRow[SETTINGSTYPE_DEBUG][4] = 2;

    Pulsar::UI::SettingsPanel::scrollerCount[SETTINGSTYPE_DEBUG] = 1;
    Pulsar::UI::SettingsPanel::optionsPerPagePerScroller[SETTINGSTYPE_DEBUG][0] = 4;

    Pulsar::IO* io = Pulsar::IO::sInstance;
    if (io->type == Pulsar::IOType_DOLPHIN) {
        char ghostPath[IOS::ipcMaxPath];
        snprintf(ghostPath, IOS::ipcMaxPath, "%s%s", this->GetModFolder(), "/Ghosts");
        io->ReadFolder(ghostPath);
        GhostData data;
        RKG* rkg = new(this->heap, 0x20) RKG;
        for (int i = 0; i < io->GetFileCount(); ++i) {
            rkg->ClearBuffer();
            s32 ret = io->ReadFolderFile(rkg, i, sizeof(RKG));
            if (ret > 0 && rkg->CheckValidity()) {
                data.Init(*rkg);
                Pulsar::CupsConfig::sInstance->GetTrackGhostFolder(ghostPath, Pulsar::CupsConfig::ConvertTrack_RealIdToPulsarId(data.courseId));

                bool exists = io->FolderExists(ghostPath); //Create reg folder
                if (!exists) io->CreateFolder(ghostPath);
                char folderModePath[IOS::ipcMaxPath];
                snprintf(folderModePath, IOS::ipcMaxPath, "%s/%s", ghostPath, Pulsar::System::ttModeFolders[0]);
                exists = io->FolderExists(folderModePath); //Create 150/150F etc..
                if (!exists) io->CreateFolder(folderModePath);

                char folderFilePath[IOS::ipcMaxPath];
                char newPath[IOS::ipcMaxPath];
                io->GetFolderFilePath(folderFilePath, i);

                char* token = folderFilePath;
                char* fileName;
                do {
                    fileName = token;
                    token = strchr(token + 1, '/');
                } while (token != nullptr);


                snprintf(newPath, IOS::ipcMaxPath, "%s%s", folderModePath, fileName);
                io->RenameFile(folderFilePath, newPath);
            }
        }
        delete rkg;
    }
}

void System::CheckGhostCheatStatus(const RKG& decompressed, Pulsar::Ghosts::Manager::CBTiming timing, int idx) {
    CTTP::System& system = CTTP::System::Get();
    if (timing == Pulsar::Ghosts::Manager::IS_LOADING_LEADERBOARDS) {
        bool isCheated = false;
        if (decompressed.inputs.unused != 0) isCheated = true;
        system.isCheatedGhost[idx] = isCheated;
    }

}

// todo do this correctly
u8 GetSettingValue(Pulsar::Settings::Type type, u32 setting) {
    return Pulsar::Settings::Mgr::GetSettingValue(type, setting);
}
asmFunc TTBoxes() {
    ASM(
        nofralloc;
        stwu sp, -0x14(sp);
        mflr r0;
        stw r0, 0x4(sp);
        stw r3, 0x8(sp);
        stw r4, 0xc(sp);
        stw r5, 0x10(sp);
        
        li r3, SETTINGSTYPE_DEBUG;
        li r4, SETTINGDEBUG_RADIO_TTBOXES;
        bl GetSettingValue;

        cmpwi r3, DEBUGSETTING_TTBOXES_ENABLED;
        lbz r0, 0x55(r15);
        bne- end;
        li r0, 0;

        end:
        lwz r4, 0x4(sp);
        mtlr r4;
        lwz r3, 0x8(sp);
        lwz r4, 0xc(sp);
        lwz r5, 0x10(sp);
        addi sp, sp, 0x14;
        blr
    )
}
kmCall(0x8082757c, TTBoxes);


}//namespace CTTP