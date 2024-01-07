#include <kamek.hpp>
#include <game/Archive/ArchiveRoot.hpp>
#include <game/Race/RaceData.hpp>
#include <CTTP.hpp>
#include <Pulsar/IO/IO.hpp>

namespace CTTP {
void Hotswap(EGG::TaskThread* archiveTaskThread, EGG::TaskThread::TaskFunction func, void* args, u32 r6) {
    Pulsar::IO* io = Pulsar::IO::sInstance;
    if (io->type == Pulsar::IOType_DOLPHIN && RaceData::sInstance->sInstance->menusScenario.settings.gamemode <= MODE_BATTLE
        && Pulsar::CupsDef::IsReg(Pulsar::CupsDef::sInstance->winningCourse)) {

        register ArchiveRoot* root;
        asm(mr root, r31;);
        ArchiveLoadHandle& handle = root->loadHandles[2];
        char path[0x100];
        ArchivesHolder* holder = handle.holder;
        snprintf(path, 0x100, "%s%s", handle.archivePath + 4, holder->archiveSuffixes[0]);
        if (io->OpenFile(path, Pulsar::FILE_MODE_READ)) {
            void* buffer = EGG::Heap::alloc(io->GetFileSize(), 0x20, handle.mountHeap);
            io->Read(io->GetFileSize(), buffer);
            io->Close();
            holder->archives[0].MountFromRawArchive(buffer, io->GetFileSize(), handle.mountHeap, true);
            EGG::Heap::free(buffer, handle.mountHeap);
            return;
        }
    }
    archiveTaskThread->Request(func, args, r6);
}
kmCall(0x805408a8, Hotswap);

} //namespace CTTP