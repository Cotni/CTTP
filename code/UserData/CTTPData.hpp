#ifndef _SIPDATA_
#define _SIPDATA_
#include <kamek.hpp>
#include <File/File.hpp>
#include <SkillIssuePack.hpp>

//Contains trophy count and all the settings. 
class SettingsHook {
private:
    typedef void (Func)();
    Func *mFunc;
    SettingsHook * mNext;

    static SettingsHook * sHooks;

public:
    SettingsHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (SettingsHook * p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

struct SettingsPage{
    u8 radioSetting[6];
    u8 scrollSetting[6];
};

class SettingsDummy{
    //empty just exists for future version proofing
};


class Settings : public SettingsDummy{ 
public:
    Settings(){};
    
    /*Settings(SettingsDummy *oldSettings) //in case a new file needs to get built
    {
       magic[0] = 'T';
       magic[1] = 'E';
       magic[2] = 'S';
       magic[3] = 'T';
       version = 1;
       settingsRadioCount = 3;
    }*/
public:
    char magic[4];
    u32 version; //just in case more than 255 versions
    SettingsPage pages[3];
};



class SIPData : public Settings{
public:
    u32 reserved[20];
    u32 trackCount;
}__attribute((aligned (0x20)));

class SettingsHolder {
public:
    SettingsHolder();
    static void Create();
    static SettingsHolder *GetInstance(){return sInstance;}
    Settings *GetSettings() const {return this->settings;}
    void Update();
protected:
    static SettingsHolder *sInstance;
    virtual void Init(char *path, const char*curMagic, u32 version) = 0;
    virtual void RequestSave() = 0;
    
    char filePath[IPCMAXPATH];
    Settings *settings;
    //any global var can go here I guess
};

class SIPDataHolder : public SettingsHolder{
public:
    static SIPDataHolder *GetInstance(){return (SIPDataHolder*)sInstance;}
    SIPData *GetSettings() const {return (SIPData*) this->settings;} //hides parent
private:
    void Init(char *path, const char*curMagic, u32 curVersion) override;
    void RequestSave() override;
public:
    static void SaveTask(void *);
    void Save();
};




enum SIPSettingsPages{
    CTTP_MENU_SETTINGS,
    CTTP_RACE_SETTINGS,
    CTTP_HOST_SETTINGS
};

//MENU SETTINGS
enum SIPRadioMenuSettings{
   CTTP_RADIO_FASTMENUS
};

enum SIPRadioMenuFastMenus {
    CTTP_RADIO_FASTMENUS_DISABLED = 0x0,
    CTTP_RADIO_FASTMENUS_ENABLED = 0x1
};

enum SIPScrollMenuSettings{
    CTTP_SCROLL_BOOT
};

enum SIPScrollMenuBoot{
    CTTP_SCROLL_BOOT_DISABLED,
    CTTP_SCROLL_BOOT_L1,
    CTTP_SCROLL_BOOT_L2,
    CTTP_SCROLL_BOOT_L3,
    CTTP_SCROLL_BOOT_L4
};

//RACE SETTINGS
enum SIPRadioRaceSettings{
   CTTP_RADIO_MII,
   CTTP_RADIO_SPEEDUP,
   CTTP_RADIO_BATTLE,
   CTTP_RADIO_BLUES
};

enum SIPRadioRaceMII {
    CTTP_RADIO_MII_DISABLED = 0x0,
    CTTP_RADIO_MII_ENABLED = 0x1
};

enum SIPRadioRaceSPEEDUP {
    CTTP_RADIO_SPEEDUP_DISABLED = 0x0,
    CTTP_RADIO_SPEEDUP_ENABLED = 0x1
};

enum SIPRadioRaceBATTLE {
    CTTP_RADIO_BATTLE_GLITCH_DISABLED = 0x0,
    CTTP_RADIO_BATTLE_GLITCH_ENABLED = 0x1
};

enum SIPRadioRaceBLUES {
    CTTP_RADIO_DRAGGABLE_BLUES_DISABLED = 0x0,
    CTTP_RADIO_DRAGGABLE_BLUES_ENABLED = 0x1
};

//HOST SETTINGS
enum SIPRadioHostSettings{
   CTTP_RADIO_HOSTWINS,
   CTTP_RADIO_CC
};

enum SIPRadioHostHostWins {
    CTTP_RADIO_HOSTWINS_DISABLED,
    CTTP_RADIO_HOSTWINS_ENABLED
};

enum SIPRadioHostCC{
    CTTP_RADIO_CC_NORMAL,
    CTTP_RADIO_CC_150,
    CTTP_RADIO_CC_200    
};

enum SIPScrollHostSettings{
    CTTP_SCROLL_GP_RACES
};

enum SIPScrollHostGPRACES{
    CTTP_SCROLL_GP_RACES_4,
    CTTP_SCROLL_GP_RACES_8,
    CTTP_SCROLL_GP_RACES_12,
    CTTP_SCROLL_GP_RACES_24,
    CTTP_SCROLL_GP_RACES_32,
    CTTP_SCROLL_GP_RACES_64,
    CTTP_SCROLL_GP_RACES_2
};

#endif