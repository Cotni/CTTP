#include <SkillIssuePack.hpp>
#include <core/rvl/ipc/ipc.hpp>

//Create Mod Folders


namespace CTTP{

using IOS::Open2ndInst;
asm s32 Open(char *path, IOS::Mode mode){ //wiimmfi patch...
    ASM(
        nofralloc;
        stwu sp, -0x0020 (sp);
        b Open2ndInst;
    )
}
u32 winningCourse = -1;
}//namespace SIP

void CreateFolders(){
    CTTP::FolderHandler *folder = CTTP::FolderHandler::Create();
    folder->CreateFolder(CTTP::modFolder);
    folder->CreateFolder(CTTP::ghostFolder);
    delete(folder);
}
BootHook Folders(CreateFolders, HIGH);

void CTTP::ChangeImage(LayoutUIControl *control, const char *paneName, const char *tplName){
   void *tplRes = control->layout.resources->multiArcResourceAccessor.GetResource(res::RESOURCETYPE_TEXTURE, tplName);
   control->layout.GetPaneByName(paneName)->GetMaterial()->GetTexMapAry()->ReplaceImage((TPLPalettePtr) tplRes);
};

//Unlock Everything Without Save (_tZ)
kmWrite32(0x80549974, 0x38600001);

//Skip ESRB page
kmWriteRegionInstruction(0x80604094, 0x4800001c, 'E');