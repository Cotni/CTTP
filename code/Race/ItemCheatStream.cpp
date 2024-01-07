#include <core/nw4r/ut/Misc.hpp>
#include <game/UI/Page/Other/GhostManager.hpp>
#include <game/UI/SectionMgr/SectionMgr.hpp>
#include <Race/ItemCheatStream.hpp>
#include <CTTP.hpp>

namespace CTTP {


/*
void ItemCheatStream::Write(u8 value) {
    if (mode != MODE_WRITE) return;
    u32 curPosition = this->curBufferPosition;

    Input* curInputBuf = &this->bufferStart[curPosition];
    bool isNewInput = this->IsNewInput(curInputBuf, value);
    if (isNewInput) {
        this->curBufferPosition++;
        if (this->curBufferPosition * inputSize >= this->bufferSize) {
            this->mode = MODE_OFF;
            return;
        }
    }

    curPosition = this->curBufferPosition;
    curInputBuf = &this->bufferStart[curPosition];
    if (isNewInput) this->WriteNewValue(curInputBuf, value);
    this->IncrementDuration(curInputBuf);
}



/*
bool ItemCheatStream::IsNewInput(const Input* buffer, u8 value) const {
    return buffer->value != value || buffer->duration == 255;
}

bool ItemCheatStream::IsCurReadInputFinished(const Input* buffer) const {
    s16 cur = this->curReadInputDuration;
    u8 total = buffer->duration;
    return cur >= total;
}


void ItemCheatStream::WriteNewValue(Input* buffer, u8 value) const {
    buffer->value = value;
    buffer->duration = 0;
}


u8 ItemCheatStream::ReadValue(const Input* buffer) const {
    return buffer->value;
}


void ItemCheatStream::IncrementDuration(Input* buffer) const {
    ++buffer->duration;
}

int ItemCheatStream::GetInputSize() const {
    return 2;
}
*/

u8 ItemCheatStream::Read() {
    if (mode != MODE_READ) return 0xFF;
    bool isNewInput = false;
    if (curBufferPosition < 0) {
        curBufferPosition = 0;
        isNewInput = true;
    }
    u32 curPosition = this->curBufferPosition;
    Input* curInputBuf = &this->bufferStart[curPosition];
    bool isFinished = this->IsCurReadInputFinished(curInputBuf);
    if (isFinished) {
        ++this->curBufferPosition;
        isNewInput = true;
        if (this->curBufferPosition * inputSize >= this->bufferSize) {
            this->mode = MODE_OFF;
            return 0xFF;
        }
    }
    curPosition = this->curBufferPosition;
    curInputBuf = &this->bufferStart[curPosition];
    if (isNewInput) this->curReadInputDuration = 0;
    ++this->curReadInputDuration;
    return this->ReadValue(curInputBuf);
}

void CopyToRKG(Input::RealControllerHolder* controllerHolder, u8 id) {
    controllerHolder->EndGhostWriting();
    u32 rkgInputsSize = controllerHolder->rkgInputsSize;
    System& cttp = System::Get();
    ItemCheatStream& stream = cttp.streams[id];
    if (rkgInputsSize != 0 && cttp.cheatedItem[id] != 0xFF) {

        stream.mode = Input::GhostStream::MODE_OFF;
        Input::RKGInputs* inputsBuffer = controllerHolder->filledRkgInputBuffer;
        u32 inputsCount = stream.curBufferPosition + 1;
        u32 cheatBufSize = stream.inputSize * inputsCount;
        if (stream.curBufferPosition >= 0 && cheatBufSize + rkgInputsSize <= 0x276c) {
            inputsBuffer->unused = inputsCount;
            controllerHolder->rkgInputsSize = rkgInputsSize + cheatBufSize;
            memcpy(nw4r::ut::AddU32ToPtr(inputsBuffer, rkgInputsSize), stream.bufferStart, cheatBufSize);
        }
        else {
            controllerHolder->rkgInputsSize = 0;
            controllerHolder->filledRkgInputBuffer = nullptr;
        }
    }
}
kmBranch(0x805245d8, CopyToRKG);

void InitStreamForReading(Input::Manager* inputMgr, u8 id, Input::RKGInputs* rawInputs, bool isAuto) {
    inputMgr->InitGhostController(id, rawInputs, isAuto);
    System& cttp = System::Get();
    ItemCheatStream& stream = cttp.streams[id];
    stream.bufferStart = reinterpret_cast<Input::GhostStream::Input*>(nw4r::ut::AddU32ToPtr(rawInputs, 8 +
        (rawInputs->actionInputsCount + rawInputs->directionInputsCount + rawInputs->trickInputsCount) * stream.inputSize));
    stream.curBufferPosition = -1;
    stream.curReadInputDuration = 0;
    stream.bufferSize = rawInputs->unused * stream.inputSize;
    stream.mode = Input::GhostStream::MODE_READ;
}
kmCall(0x8052f610, InitStreamForReading);

void InitStreamForWriting() {
    const RacedataScenario& scenario = RaceData::sInstance->menusScenario;
    System& cttp = System::Get();
    ItemCheatStream& stream = cttp.streams[0];
    for (int hudSlotId = 0; hudSlotId < 4; hudSlotId++) {
        cttp.cheatedItem[hudSlotId] = static_cast<ItemId>(0xFF);
    }
    if (scenario.players[0].playerType == PLAYER_REAL_LOCAL && scenario.settings.gamemode == MODE_TIME_TRIAL) {
        RKG* rkg = &SectionMgr::sInstance->curSection->Get<Pages::GhostManager>(PAGE_GHOST_MANAGER)
            ->rkgBuffer;
        rkg->ClearBuffer();
        stream.bufferStart = &rkg->inputs.actionInputs[0];
        stream.curBufferPosition = -1;
        stream.curReadInputDuration = 0;
        stream.bufferSize = sizeof(Input::RKGInputs) - 8; //remove header
        stream.curReadInputDuration = 0;
        stream.mode = Input::GhostStream::MODE_WRITE;
    }
    else if (scenario.players[0].playerType != PLAYER_GHOST) stream.mode = Input::GhostStream::MODE_OFF;
}
RaceLoadHook WriteItems(InitStreamForWriting);


}//namespace CTTP