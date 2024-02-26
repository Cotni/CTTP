#include <kamek.hpp>
#include <core/rvl/gx/GX.hpp>
#include <MarioKartWii/KMP/KMPManager.hpp>
#include <MarioKartWii/3D/Scn/Renderer.hpp>
#include <MarioKartWii/3D/Camera/RaceCamera.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <CTTP.hpp>
#include <PulsarEngine/Settings/Settings.hpp>

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
    GX::Begin(GX::GX_QUADS, GX::GX_VTXFMT0, 4);
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
        GX::SetLineWidth(20, GX::GX_TO_ZERO);
        GX::Begin(GX::GX_LINESTRIP, GX::GX_VTXFMT0, 5);
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
    GX::SetCullMode(GX::GX_CULL_NONE); //OK
    GX::SetZMode(true, GX::GX_LEQUAL, true); //OK
    //GX::SetZCompLoc(false);
    //GX::SetAlphaCompare(GX::GX_ALWAYS, 0, GX::GX_AOP_OR, GX::GX_ALWAYS, false);
    GX::SetBlendMode(GX::GX_BM_BLEND, GX::GX_BL_INVSRCALPHA, GX::GX_BL_INVDSTALPHA, GX::GX_LO_OR); //OK
    //GX::SetAlphaUpdate(true);
    //GX::SetColorUpdate(true);
    //GX::Color color = { 0xFFFFFFFF };
    //GX::SetFog(GX::GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);

    GX::ClearVtxDesc();
    GX::SetVtxDesc(GX::GX_VA_POS, GX::GX_DIRECT);
    GX::SetVtxDesc(GX::GX_VA_CLR0, GX::GX_DIRECT);
    GX::SetVtxAttrFmt(GX::GX_VTXFMT0, GX::GX_VA_POS, GX::GX_POS_XYZ, GX::GX_F32, false);
    GX::SetVtxAttrFmt(GX::GX_VTXFMT0, GX::GX_VA_CLR0, GX::GX_POS_XYZ, GX::GX_RGBA8, false);
    //GX::SetNumChans(1);
    //GX::SetNumTexGens(0);
    //GX::SetNumTevStages(1);
    GX::SetTevOrder(GX::GX_TEVSTAGE0, GX::GX_TEXCOORD_NULL, GX::GX_TEXMAP_NULL, GX::GX_COLOR0A0);
    GX::SetChanCtrl(GX::GX_COLOR0A0, false, GX::GX_SRC_VTX, GX::GX_SRC_VTX, 0, GX::GX_DF_NONE, GX::GX_AF_NONE);
    GX::SetTevOp(GX::GX_TEVSTAGE0, GX::GX_PASSCLR);
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
        const u8 kmpSetting = Pulsar::Settings::Mgr::GetSettingValue(static_cast<Pulsar::Settings::Type>(SETTINGSTYPE_DEBUG), SETTINGDEBUG_SCROLL_KMP);
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


