#include <kamek.hpp>
#include <MarioKartWii/Archive/ArchiveRoot.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <CTTP.hpp>
#include <PulsarEngine/IO/IO.hpp>

namespace CTTP {
void Hotswap(EGG::TaskThread* archiveTaskThread, EGG::TaskThread::TaskFunction func, void* args, u32 r6) {
    Pulsar::IO* io = Pulsar::IO::sInstance;
    if (io->type == Pulsar::IOType_DOLPHIN && RaceData::sInstance->sInstance->menusScenario.settings.gamemode <= MODE_BATTLE
        && Pulsar::CupsConfig::IsReg(Pulsar::CupsConfig::sInstance->winningCourse)) {

        register ArchiveRoot* root;
        asm(mr root, r31;);
        ArchiveLoadHandle& handle = root->loadHandles[2];
        ArchivesHolder* holder = handle.holder;

        char path[0x100];
        snprintf(path, 0x100, "%s%s", handle.archivePath + 4, holder->archiveSuffixes[0]);
        if (io->OpenFileDirectly(path, Pulsar::FILE_MODE_READ)) {
            u32 fileSize = io->GetFileSize();
            void* buffer = EGG::Heap::alloc(fileSize, 0x20, handle.mountHeap);
            io->Read(fileSize, buffer);
            io->Close();
            holder->archives[0].MountFromRawArchive(buffer, fileSize, handle.mountHeap, true);
            EGG::Heap::free(buffer, handle.mountHeap);
            return;
        }
    }
    archiveTaskThread->Request(func, args, r6);
}
kmCall(0x805408a8, Hotswap);

} //namespace CTTP