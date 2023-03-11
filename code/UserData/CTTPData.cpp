#include <UserData/CTTPData.hpp>

//SETTINGS HOLDER, IMPLEMENT INIT AND SAVE BASED ON YOUR SPECIFIC SETTINGS
SettingsHolder *SettingsHolder::sInstance = NULL;
SettingsHook *SettingsHook::sHooks = NULL;

SettingsHolder::SettingsHolder(){ 
    settings = NULL;
}

void SettingsHolder::Update(){
    SettingsHook::exec();
    this->RequestSave();
}

void SIPDataHolder::Init(char *path, const char*curMagic, u32 curVersion){
    strncpy(this->filePath, path, IPCMAXPATH);
    SIPData * buffer = new (RKSystem::mInstance.EGGSystem, 0x20) SIPData;
    
    CTTP::FileHandler *loader = CTTP::FileHandler::sInstance;
    loader->CreateAndOpen(this->filePath, CTTP::FILE_MODE_READ_WRITE);
    loader->Read(buffer, sizeof(SIPData));
    if(strcmp(buffer->magic, curMagic) != 0 || buffer->version != curVersion){
        memset(buffer, 0, sizeof(SIPData));
        strncpy(buffer->magic, curMagic, 4);
        buffer->version = curVersion;
    }
    this->settings = buffer;
    loader->Overwrite(sizeof(SIPData), buffer);
    
    loader->Close();
}

void SIPDataHolder::RequestSave(){
    CTTP::FileHandler::sInstance->taskThread->Request(&SIPDataHolder::SaveTask, NULL, 0);
}

void SIPDataHolder::SaveTask(void*){
    SIPDataHolder::GetInstance()->Save();
}

void SIPDataHolder::Save(){
    CTTP::FileHandler *loader = CTTP::FileHandler::sInstance;
    loader->Open(this->filePath, CTTP::FILE_MODE_WRITE);
    loader->Overwrite(sizeof(SIPData), this->settings);
    loader->Close();
};


void SettingsHolder::Create(){
    SettingsHolder *settings = new(RKSystem::mInstance.EGGSystem) SIPDataHolder();
    char path[IPCMAXPATH];
    snprintf(path, IPCMAXPATH, "%s/%s", CTTP::modFolder, "SIPData.bin");
    settings->Init(path, "SIPD", 1);
    SettingsHolder::sInstance = settings;
}
BootHook TestSettings(SettingsHolder::Create, MEDIUM);