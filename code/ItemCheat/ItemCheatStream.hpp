#ifndef _ITEMCHEATSTREAM_
#define _ITEMCHEATSTREAM_

#include <kamek.hpp>
#include <MarioKartWii/Input/InputManager.hpp>

namespace CTTP {

//typedef Input::GhostActionStream ItemCheatStream;

class ItemCheatStream : public Input::GhostActionStream {
public:
    static const int inputSize = 2;
    //void Write(u8 value) override; //0x8
    u8 Read() override; //0xC
    //int GetInputSize() const override; //0x10
private:
    //void WriteNewValue(Input* buffer, u8 value) const override; //0x14
    //void IncrementDuration(Input* buffer) const override; //0x18
    //bool IsNewInput(const Input* buffer, u8 value) const override; //0x1C
    //u8 ReadValue(const Input* buffer) const override; //0x20
    //bool IsCurReadInputFinished(const Input* buffer) const override; //0x24
};


}//namespace CTTP



#endif