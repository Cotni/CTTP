#ifndef _PERFMON_
#define _PERFMON_
#include <kamek.hpp>
#include <core/egg/ProcessMeter.hpp>
#include <core/RK/RKSystem.hpp>

namespace CTTP {

class ExtProcessMeter : public EGG::ProcessMeter {
public:
    enum CTTPBars {
        BAR_PHYSICS,
        BAR_OBJECTS,
        BAR_ITEMS,
        BAR_EFFECTS,
        BAR_LAKITU,
        BAR_UI
    };
    static const int barCount = 7;
    static ExtProcessMeter* Create();
    ExtProcessMeter() : ProcessMeter(true),
        sceneMgrBar(nw4r::ut::Color(0xff50ffff), 3.0f),
        physicsBar(nw4r::ut::Color(0x00FF50FF), 4.0f),
        objsBar(nw4r::ut::Color(0x50500050), 5.0f),
        itemsBar(nw4r::ut::Color(0xb6ccf050), 6.0f),
        effectsBar(nw4r::ut::Color(0x00505050), 7.0f),
        lakituBar(nw4r::ut::Color(0x00005050), 8.0f),
        uiBar(nw4r::ut::Color(0x00505050), 9.0f)
    {
        bgBar.color = 0x000000FF;
        this->append(&sceneMgrBar);
        this->append(&physicsBar);
        this->append(&objsBar);
        this->append(&itemsBar);
        this->append(&effectsBar);
        this->append(&lakituBar);
        this->append(&uiBar);
    }
    EGG::ProcessMeter::CpuMonitor sceneMgrBar;
    EGG::ProcessMeter::CpuMonitor physicsBar;
    EGG::ProcessMeter::CpuMonitor objsBar;
    EGG::ProcessMeter::CpuMonitor itemsBar;
    EGG::ProcessMeter::CpuMonitor effectsBar;
    EGG::ProcessMeter::CpuMonitor lakituBar;
    EGG::ProcessMeter::CpuMonitor uiBar;
};
void DisableMonitor();

}//namespace CTTP



#endif