#include <kamek.hpp>
#include <core/rvl/gx/GX.hpp>
#include <game/KMP/KMPManager.hpp>
#include <game/3D/Scn/Renderer.hpp>
#include <game/3D/Camera/RaceCamera.hpp>
#include <game/Item/ItemManager.hpp>
#include <CTTP.hpp>
#include <Pulsar/Settings/Settings.hpp>

namespace CTTP {
struct Quad {
    Quad(u32 color) : color(color) {}
    Quad(u32 color, float leftX, float bottomY, float leftZ, float rightX, float topY, float rightZ) :
        leftBottomPoint(leftX, bottomY, leftZ),
        rightTopPoint(rightX, topY, rightZ),
        color(color) {}
    Quad(u32 color, u32 outlineColor, float leftX, float bottomY, float leftZ, float rightX, float topY, float rightZ) :
        leftBottomPoint(leftX, bottomY, leftZ),
        rightTopPoint(rightX, topY, rightZ),
        color(color), outlineColor(outlineColor) {}

    Vec3 leftBottomPoint;
    Vec3 rightTopPoint;
    u32 color;
    u32 outlineColor;
};

void DrawQuad(const Quad& quad, bool outline) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GX_Position3f32(quad.leftBottomPoint.x, quad.rightTopPoint.y, quad.leftBottomPoint.z);
        GX_Color1u32(quad.color);

        GX_Position3f32(quad.rightTopPoint.x, quad.rightTopPoint.y, quad.rightTopPoint.z);
        GX_Color1u32(quad.color);

        GX_Position3f32(quad.rightTopPoint.x, quad.leftBottomPoint.y, quad.rightTopPoint.z);
        GX_Color1u32(quad.color);

        GX_Position3f32(quad.leftBottomPoint.x, quad.leftBottomPoint.y, quad.leftBottomPoint.z);
        GX_Color1u32(quad.color);
    }
    GXEnd();
    if (outline) {
        GXSetLineWidth(20, GX_TO_ZERO);
        GXBegin(GX_LINESTRIP, GX_VTXFMT0, 5);
        {
            GX_Position3f32(quad.leftBottomPoint.x, quad.rightTopPoint.y, quad.leftBottomPoint.z);
            GX_Color1u32(quad.outlineColor);

            GX_Position3f32(quad.rightTopPoint.x, quad.rightTopPoint.y, quad.rightTopPoint.z);
            GX_Color1u32(quad.outlineColor);

            GX_Position3f32(quad.rightTopPoint.x, quad.leftBottomPoint.y, quad.rightTopPoint.z);
            GX_Color1u32(quad.outlineColor);

            GX_Position3f32(quad.leftBottomPoint.x, quad.leftBottomPoint.y, quad.leftBottomPoint.z);
            GX_Color1u32(quad.outlineColor);

            GX_Position3f32(quad.leftBottomPoint.x, quad.rightTopPoint.y, quad.leftBottomPoint.z);
            GX_Color1u32(quad.outlineColor);

        }
        GXEnd();
    }
}

void SetupGX() {
    GXSetCullMode(GX_CULL_NONE); //OK
    GXSetZMode(true, GX_LEQUAL, true); //OK
    //GXSetZCompLoc(false);
    //GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, false);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_INVSRCALPHA, GX_BL_INVDSTALPHA, GX_LO_OR); //OK
    //GXSetAlphaUpdate(true);
    //GXSetColorUpdate(true);
    //GXColor color = { 0xFFFFFFFF };
    //GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, false);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_POS_XYZ, GX_RGBA8, false);
    //GXSetNumChans(1);
    //GXSetNumTexGens(0);
    //GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}

void DrawCylinder(const Vec3& point, float radius, float height, u32 color) {
    Quad quad(color);
    quad.leftBottomPoint.y = point.y;
    quad.rightTopPoint.y = point.y + height;

    quad.rightTopPoint.x = point.x + radius; //cos(0) = 1
    quad.rightTopPoint.z = point.z;
    float sin;
    float cos;
    for (float i = 0.0f; i < 270.0f; i += 13.0f) {
        quad.leftBottomPoint.x = quad.rightTopPoint.x;
        quad.leftBottomPoint.z = quad.rightTopPoint.z;
        nw4r::math::SinCosFIdx(&sin, &cos, i);
        quad.rightTopPoint.x = cos * radius + point.x;
        quad.rightTopPoint.z = sin * radius + point.z;
        DrawQuad(quad, false);
    }
}


inline void DrawCKPT(const CKPT& rawCKPT) {
    const u32 color = rawCKPT.type != 0xFF ? 0xd442f533 : 0x2235e033;
    const u32 outline = rawCKPT.type != 0xFF ? 0x68018080 : 0x03108a80;
    const JGPT* respawn = KMP::Manager::sInstance->GetHolder<JGPT>(rawCKPT.respawn)->raw;
    float top = 1000.0f + respawn->position.y;
    float bottom = -1000.0f + respawn->position.y;
    DrawQuad(Quad(color, outline, rawCKPT.leftPoint.x, bottom, rawCKPT.leftPoint.z, rawCKPT.rightPoint.x, top, rawCKPT.rightPoint.z), true);

}

inline void DrawITPT(const ITPT& rawITPT, bool isCurrent) {
    const u32 color = isCurrent ? 0xd442f533 : 0x2235e033;
    DrawCylinder(rawITPT.position, rawITPT.width, 500.0f, color);
}

void DrawKMPInfo(RendererWithModels& renderer, u32 projType, u32 r5) {

    if (RaceData::sInstance->racesScenario.settings.gamemode <= MODE_TIME_TRIAL) {
        SetupGX();
        renderer.screen.SetAndLoadProjection(1, 0);

        const u8 playerId = static_cast<RaceCamera*>(renderer.screen.perspectiveCam->camera)->playerId;
        const KMP::Manager* kmp = KMP::Manager::sInstance;
        const u8 kmpSetting = Pulsar::Settings::GetSettingValue(static_cast<Pulsar::SettingsType>(SETTINGSTYPE_DEBUG), SETTINGDEBUG_SCROLL_KMP);
        if (kmpSetting == DEBUGSETTING_KMP_ALLCPS || kmpSetting == DEBUGSETTING_KMP_KEYCPS) {
            for (int i = 0; i < kmp->ckptSection->pointCount; ++i) {
                const CKPT& cur = *kmp->GetHolder<CKPT>(i)->raw;
                if (cur.type != 0xFF || kmpSetting == DEBUGSETTING_KMP_ALLCPS) DrawCKPT(cur);
            }
        }
        else if (kmpSetting == DEBUGSETTING_KMP_ITPTS) {
            for (int i = 0; i < kmp->itptSection->pointCount; ++i) {
                DrawITPT(*kmp->GetHolder<ITPT>(i)->raw, Item::Manager::sInstance->players[playerId].itemPoint.ITPT == i);
            }
        }
    }
    renderer.DrawWithProjection(projType, r5);
}
kmCall(0x805b4390, DrawKMPInfo);

}//namespace CTTP


