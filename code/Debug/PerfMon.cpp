#include <core/rvl/OS/OS.hpp>
#include <game/Objects/ObjectsMgr.hpp>
#include <game/Kart/KartManager.hpp>
#include <game/Item/ItemManager.hpp>
#include <game/3D/Effect/EffectMgr.hpp>
#include <game/Lakitu/LakituManager.hpp>
#include <game/UI/SectionMgr/SectionMgr.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceTime.hpp>
#include <CTTP.hpp>
#include <Debug/PerfMon.hpp>
#include <Pulsar/Settings/Settings.hpp>

namespace CTTP {

kmWrite32(0x800092bc, 0x60000000);
//kmWrite32(0x80238a74, 0x60000000); //prevent resume thread
ExtProcessMeter* ExtProcessMeter::Create() {

    ExtProcessMeter* meter = new ExtProcessMeter();

    meter->xOrigin = 28.0f;
    meter->yOrigin = 1.0f;
    meter->xSize = 60.0f;
    meter->ySize = 0.75f;

    meter->setVisible(false);
    return meter;
    //List_Remove(&meter->processBarList, &meter->bgBar);
}
kmCall(0x800092cc, ExtProcessMeter::Create);
//kmWrite32(0x802395c0, 0x60000000);



void measureBeginCPU(RKSystem* rkSystem) {
    //if (OS::GetTick() - system->asyncDisplay->tick >= 0xEF000) ++System::Get().droppedFrames;
    EGG::ProcessMeter* meter = rkSystem->processMeter;
    System* system = &System::Get();
    if (system != nullptr && meter->bgBar.xSize <= meter->cpuMonitor.bar.xSize) ++system->droppedFrames;
    EGG::PerformanceView* view = static_cast<EGG::PerformanceView*>(meter);
    view->measureBeginFrame();
    rkSystem->asyncDisplay->beginRender();
    view->measureBeginRender();
}
kmCall(0x80009608, measureBeginCPU);
kmWrite32(0x8000962c, 0x60000000);

//8000968c
void measureEndGPU(EGG::PerformanceView* view) {
    view->draw();
    view->measureEndRender();
}
kmCall(0x8000968c, measureEndGPU);

//800097a4
void measureEndCPU(RKSystem* system) {
    static_cast<EGG::PerformanceView*>(system->processMeter)->measureEndFrame();
}
kmCall(0x800097a4, measureEndCPU);


void ScnMgrMeasurement(RKSceneManager* sceneMgr) {
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    const bool visible = static_cast<EGG::PerformanceView*>(meter)->isVisible();
    if (visible) meter->sceneMgrBar.measureBegin();

    sceneMgr->calc();
    if (visible) meter->sceneMgrBar.measureEnd();
}
kmCall(0x80009760, ScnMgrMeasurement);

void ObjectMeasurement(ObjectsMgr* objectMgr) {
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->objsBar.measureBegin();
    objectMgr->Update();
    meter->objsBar.measureEnd();
}
kmCall(0x80554bb8, ObjectMeasurement);

void PhysicsMeasurement(Kart::Manager* kartMgr) {
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->physicsBar.measureBegin();
    kartMgr->Update();
    meter->physicsBar.measureEnd();
}
kmCall(0x80554bf0, PhysicsMeasurement);

void ItemMeasurement(Item::Manager* itemMgr) {
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->itemsBar.measureBegin();
    itemMgr->Update();
    meter->itemsBar.measureEnd();
}
kmCall(0x80554c68, ItemMeasurement);

void EffectsMeasurement(EffectsMgr* effectsMgr) {
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->effectsBar.measureBegin();
    effectsMgr->Update();
    meter->effectsBar.measureEnd();
}
kmCall(0x80554ce8, EffectsMeasurement);

void LakituMeasurement(Lakitu::Manager* lakituMgr) {
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->lakituBar.measureBegin();
    lakituMgr->Update();
    meter->lakituBar.measureEnd();
}
kmCall(0x80554bfc, LakituMeasurement);

void UIBeginMeasurement(Section* section, u32 r4) {
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->uiBar.measureBegin();
    section->Draw(r4);
}
kmCall(0x805b4224, UIBeginMeasurement);

void UIEndMeasurement(Section* section, u32 r4) {
    section->Draw(r4);
    ExtProcessMeter* meter = static_cast<ExtProcessMeter*>(RKSystem::mInstance.processMeter);
    meter->uiBar.measureEnd();
}
kmCall(0x805b4300, UIEndMeasurement);

void ToggleMonitor() {
    bool isOnline = CtrlRaceTime::IsOnlineVS(*RaceData::sInstance);
    bool isVisible = true;
    if (!isOnline) isVisible = !Pulsar::Settings::GetSettingValue(static_cast<Pulsar::SettingsType>(SETTINGSTYPE_DEBUG), SETTINGDEBUG_RADIO_PERFMON);
    RKSystem::mInstance.processMeter->EGG::ProcessMeter::setVisible(isVisible);
    System::Get().droppedFrames = 0;
}
RaceLoadHook TogglePerfMon(ToggleMonitor);

inline void DisableMonitor() {
    RKSystem::mInstance.processMeter->EGG::ProcessMeter::setVisible(false);
}
kmBranch(0x80554a90, DisableMonitor); //RaceScene::OnExit


}//namespace CTTP