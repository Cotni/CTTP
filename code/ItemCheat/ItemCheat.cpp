#include <kamek.hpp>
#include <MarioKartWii/Input/InputManager.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/UI/SectionMgr/SectionMgr.hpp>
#include <CTTP.hpp>

namespace CTTP {

ItemId WriteItem(u32 hudSlotId) {
    static const u8 cheatedItems[16] = {
        GOLDEN_MUSHROOM,
        THUNDER_CLOUD,
        MUSHROOM,
        BULLET_BILL,
        STAR,
        BOBOMB,
        FAKE_ITEM_BOX,
        MEGA_MUSHROOM,


        BLUE_SHELL,
        GREEN_SHELL,
        BANANA,
        RED_SHELL,
        BLOOPER,
        POW_BLOCK,
        LIGHTNING,
        ITEM_NONE
    };

    const Input::RealControllerHolder* controllerHolder = SectionMgr::sInstance->pad.padInfos[hudSlotId].controllerHolder;
    const ControllerType controllerType = controllerHolder->curController->GetType();
    const u16 inputs = controllerHolder->inputStates[0].buttonRaw;
    const u16 newInputs = (inputs & ~controllerHolder->inputStates[1].buttonRaw);
    const u8* items = &cheatedItems[0];

    u8 itemTableOffset = 0;
    ItemId ret = static_cast<ItemId>(0xFF);
    if (controllerType == WHEEL || controllerType == NUNCHUCK) {
        u32 switchButton1 = WPAD::WPAD_BUTTON_B;
        u32 switchButton2 = WPAD::WPAD_BUTTON_C;
        u32 switchButton3 = WPAD::WPAD_BUTTON_B + WPAD::WPAD_BUTTON_C;
        if (controllerType == WHEEL) {
            switchButton1 = WPAD::WPAD_BUTTON_MINUS;
            switchButton2 = WPAD::WPAD_BUTTON_1;
            switchButton3 = WPAD::WPAD_BUTTON_MINUS + WPAD::WPAD_BUTTON_1;
        }
        if ((inputs & switchButton3) == switchButton3) items += 12; //check the combo first
        else if ((inputs & switchButton1) == switchButton1) items += 4;
        else if ((inputs & switchButton2) == switchButton2) items += 8;

        for (int i = 0; i < 4; ++i) {
            if (newInputs & 1 << i) ret = static_cast<ItemId>(items[i]);
        }
    }
    else {
        register const float* cSticks;
        u32 switchButton1;
        if (controllerType == CLASSIC) {
            Input::WiiController* wiiController = static_cast<Input::WiiController*>(controllerHolder->curController);
            cSticks = &wiiController->kpadStatus[0].extStatus.cl.stickR.x;
            switchButton1 = WPAD::WPAD_CL_TRIGGER_ZL;
        }
        else {
            Input::GCNController* gcnController = static_cast<Input::GCNController*>(controllerHolder->curController);
            cSticks = &gcnController->cStickHorizontal;
            switchButton1 = PAD::PAD_BUTTON_Z;
        }
        if ((inputs & switchButton1) == switchButton1) items += 8;
        register float vec;
        register u32 oldGQRValue;
        register u32 quantization = 0x0706;
        s8 quantizedCSticks[2];
        register s8* intPtr = &quantizedCSticks[0];

        asm(
            mfspr oldGQRValue, GQR7;
        mtspr GQR7, quantization;
        psq_l vec, 0(cSticks), 0, 0;
        psq_st vec, 0(intPtr), 0, 7;
        mtspr GQR7, oldGQRValue;
        );
        const s32 h = quantizedCSticks[0];
        const s32 v = quantizedCSticks[1];

        //u32 idx;
        if (v >= 118) ret = static_cast<ItemId>(items[0]);
        else if (v >= 49 && h >= 49) ret = static_cast<ItemId>(items[1]);
        else if (h >= 118) ret = static_cast<ItemId>(items[2]);
        else if (-v >= 49 && h >= 49) ret = static_cast<ItemId>(items[3]);
        else if (-v >= 118) ret = static_cast<ItemId>(items[4]);
        else if (-v >= 49 && -h >= 49) ret = static_cast<ItemId>(items[5]);
        else if (-h >= 118) ret = static_cast<ItemId>(items[6]);
        else if (v >= 49 && -h >= 49) ret = static_cast<ItemId>(items[7]);
        /*

        for (int i = 0; i < 8; ++i) {
            const float compHorCStick = i < 4 ? h : -h;
            const float compVertCStick = i < 3 || i == 7 ? v : -v;
            const bool firstInput = compVertCStick >= consts[i & 1];
            const bool secondInput = !(i & 1) || compHorCStick >= 49;
            if (firstInput && secondInput) {
                ret = static_cast<ItemId>(cheatedItems[i + itemTableOffset]);
                break;
            }
        }
        */
    }

    System::Get().streams[0].Write(ret);
    return ret;
}

ItemId ReadItem(u32 hudSlotId) {
    return static_cast<ItemId>(System::Get().streams[hudSlotId].Read());
}

void ItemHack(Item::Player* player) {
    const Input::RealControllerHolder* controllerHolder = SectionMgr::sInstance->pad.padInfos[player->hudSlotId].controllerHolder;
    const ControllerType controllerType = controllerHolder->curController->GetType();

    if (
        controllerType != WHEEL &&
        System::IsItemHackSituation() &&
        Pulsar::Settings::Mgr::GetSettingValue(static_cast<Pulsar::Settings::Type>(SETTINGSTYPE_DEBUG), SETTINGDEBUG_RADIO_ITEMCHEATS) != DEBUGSETTING_ITEMCHEATS_DISABLED
    ) {
        PlayerType type = RaceData::sInstance->racesScenario.players[player->id].playerType;
        if (type == PLAYER_REAL_LOCAL || type == PLAYER_GHOST) {
            ItemId ret;
            if (type == PLAYER_REAL_LOCAL) ret = WriteItem(player->hudSlotId);
            else ret = ReadItem(player->hudSlotId);


            System& cttp = System::Get();
            if (ret != 0xFF) cttp.cheatedItem[player->hudSlotId] = ret;
            ItemId cur = cttp.cheatedItem[player->hudSlotId];
            Item::PlayerInventory& inventory = player->inventory;
            if (cur == GOLDEN_MUSHROOM) {
                inventory.hasGolden = true;
                if (inventory.goldenTimer == 1 && ret != GOLDEN_MUSHROOM) {
                    inventory.hasGolden = false;
                    inventory.currentItemId = ITEM_NONE;
                    inventory.currentItemCount = 0;
                    cur = ITEM_NONE;
                }
            }
            if (cur != 0xFF) {
                cttp.cheatedItem[player->hudSlotId] = cur;
                inventory.currentItemId = static_cast<ItemId>(cur);
                inventory.currentItemCount = cur == ITEM_NONE ? 0 : 1;
            }
        }
    }
    player->Update();
}

kmCall(0x8079994c, ItemHack);
kmWrite32(0x80799CC8, 0x38000000);
}//namespace CTTP