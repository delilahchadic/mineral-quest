#include "registry/weapon_register.h"
#include <stdio.h>
#include "defs/types_entities.h"
#include "engine/palette.h"
#include "raylib.h"
#include <math.h>

void DrawSimpleGuitar(Vector2 position, float rotation) {
    float upperBoutRadius = 10.0f;
    float lowerBoutRadius = 12.0f;
    float handleLength = 12.0f;
    // Palette using your existing game colors
    Color woodBase = COLOR_GUITAR_AMBER;
    Color woodDark = ColorBrightness(COLOR_BURNT_SIENNA, -0.3f);
    Color soundhole = COLOR_SUNKEN_INK;
    Color gold = COLOR_PYRITE_BRASS;
    Color highlight = WHITE;

    float cosR = cosf(rotation);
    float sinR = sinf(rotation);
    float cosP = cosf(rotation + PI/2.0f);
    float sinP = sinf(rotation + PI/2.0f);

    // GUITAR BODY
    Vector2 upperCenter = { position.x + cosR * 18.0f, position.y + sinR * 18.0f };
    Vector2 lowerCenter = { position.x + cosR * 36.0f, position.y + sinR * 36.0f };

    DrawCircleV(upperCenter, upperBoutRadius, woodBase);
    DrawCircleV(lowerCenter, lowerBoutRadius, woodBase);

    // SMOOTHING THE WAIST
    Vector2 wLeft1  = { upperCenter.x - cosP * (upperBoutRadius * 0.8f), upperCenter.y - sinP * (upperBoutRadius * 0.8f) };
    Vector2 wLeft2  = { lowerCenter.x - cosP * (lowerBoutRadius * 0.85f), lowerCenter.y - sinP * (lowerBoutRadius * 0.85f) };
    Vector2 wRight1 = { upperCenter.x + cosP * (upperBoutRadius * 0.8f), upperCenter.y + sinP * (upperBoutRadius * 0.8f) };
    Vector2 wRight2 = { lowerCenter.x + cosP * (lowerBoutRadius * 0.85f), lowerCenter.y + sinP * (lowerBoutRadius * 0.85f) };

    DrawTriangle(wLeft1, wRight1, wLeft2, woodBase);
    DrawTriangle(wRight1, wRight2, wLeft2, woodBase);

    // SOUNDHOLE & BRIDGE
    Vector2 soundHolePos = { upperCenter.x + cosR * 4.0f, upperCenter.y + sinR * 4.0f };
    Vector2 bridgePos = { lowerCenter.x + cosR * 3.0f, lowerCenter.y + sinR * 3.0f };

    DrawCircleV(soundHolePos, 5.0f, COLOR_FIRED_GOLD_OCHRE);
    DrawCircleV(soundHolePos, 4.0f, soundhole);

    // Bridge detail near the bottom of the lower bout
    DrawLineEx(
        (Vector2){ bridgePos.x - cosP * 4.5f, bridgePos.y - sinP * 4.5f },
        (Vector2){ bridgePos.x + cosP * 4.5f, bridgePos.y + sinP * 4.5f },
        2.0f, gold
    );

    // NECK / FRETBOARD
    Vector2 handleEnd = { position.x - cosR * handleLength, position.y - sinR * handleLength };
    Vector2 neckBodyJoint = { position.x + cosR * 16.0f, position.y + sinR * 16.0f };
    DrawLineEx(handleEnd, neckBodyJoint, 6.0f, woodDark);
    // Push down the neck toward the soundhole/body joint
    float octaveDist = 22.0f;
    Vector2 octaveCenter = { handleEnd.x + cosR * octaveDist, handleEnd.y + sinR * octaveDist };

    // Two side-by-side dots across the fretboard width
    float dotSpread = 1.8f;
    Vector2 dot1 = { octaveCenter.x - cosP * dotSpread, octaveCenter.y - sinP * dotSpread };
    Vector2 dot2 = { octaveCenter.x + cosP * dotSpread, octaveCenter.y + sinP * dotSpread };

    DrawCircleV(dot1, 0.25f, COLOR_PULP_PAPER);
    DrawCircleV(dot2, 0.25f, COLOR_PULP_PAPER);

    // NUT ACCENT LINE (Where neck ends and headstock begins)
    Vector2 nutLeft  = { handleEnd.x - cosP * 4.5f, handleEnd.y - sinP * 4.5f };
    Vector2 nutRight = { handleEnd.x + cosP * 4.5f, handleEnd.y + sinP * 4.5f };
    DrawLineEx(nutLeft, nutRight, 1.5f, gold);

    // PROMINENT HEADSTOCK WOOD (Extending fully from the nut to house everything)
    // PROMINENT HEADSTOCK WOOD (Extending fully from the nut to house everything)
    float headstockLen = 8.0f; // Shortened to 8.0f
    Vector2 headstockTip = { handleEnd.x - cosR * headstockLen, handleEnd.y - sinR * headstockLen };
    Vector2 hsLeft_top  = { handleEnd.x - cosP * 4.5f, handleEnd.y - sinP * 4.5f };
    Vector2 hsRight_top = { handleEnd.x + cosP * 4.5f, handleEnd.y + sinP * 4.5f };
    Vector2 hsLeft_bot  = { headstockTip.x - cosP * 3.5f, headstockTip.y - sinP * 3.5f };
    Vector2 hsRight_bot = { headstockTip.x + cosP * 3.5f, headstockTip.y + sinP * 3.5f };

    DrawTriangle(hsLeft_top, hsRight_bot, hsRight_top, woodDark);
    DrawTriangle(hsLeft_top, hsLeft_bot, hsRight_bot, woodDark);

    // 6 TUNING PEGS SCALED TO FIT THE SHORTER HEADSTOCK
    float peg1 = 2.0f;
    float peg2 = 4.5f;
    float peg3 = 7.0f;
    float pegW = 3.2f; // Distance outward from the center
    float pRad = 0.6f; // Peg radius

    // Left side pegs
    DrawCircleV((Vector2){ handleEnd.x - cosR * peg1 - cosP * pegW, handleEnd.y - sinR * peg1 - sinP * pegW }, pRad, gold);
    DrawCircleV((Vector2){ handleEnd.x - cosR * peg2 - cosP * pegW, handleEnd.y - sinR * peg2 - sinP * pegW }, pRad, gold);
    DrawCircleV((Vector2){ handleEnd.x - cosR * peg3 - cosP * pegW, handleEnd.y - sinR * peg3 - sinP * pegW }, pRad, gold);

    // Right side pegs
    DrawCircleV((Vector2){ handleEnd.x - cosR * peg1 + cosP * pegW, handleEnd.y - sinR * peg1 + sinP * pegW }, pRad, gold);
    DrawCircleV((Vector2){ handleEnd.x - cosR * peg2 + cosP * pegW, handleEnd.y - sinR * peg2 + sinP * pegW }, pRad, gold);
    DrawCircleV((Vector2){ handleEnd.x - cosR * peg3 + cosP * pegW, handleEnd.y - sinR * peg3 + sinP * pegW }, pRad, gold);

    // 6 DISTINCT 1-PIXEL STRINGS (Running cleanly from bridge all the way to headstock tip over the wood)
    float stringSpread = 9.0f;
    for (int i = 0; i < 6; i++) {
        float t = (float)i / 5.0f;
        float offset = -stringSpread / 2.0f + t * stringSpread;
        float nutOffset = offset * 0.6f;

        Vector2 stringStart = { bridgePos.x + cosP * offset, bridgePos.y + sinP * offset };
        Vector2 stringEnd   = { headstockTip.x + cosP * nutOffset, headstockTip.y + sinP * nutOffset };

        DrawLineV(stringStart, stringEnd, highlight);
    }
}

void DrawSimpleSword(Vector2 position, float rotation) {
    float bladeLength = 34.0f;
    float tipLength = 12.0f;
    float bladeWidth = 4.5f;
    float guardWidth = 14.0f;
    float guardThick = 2.5f;
    float hiltLength = 14.0f;

    Color metalShadow = COLOR_SUNKEN_INK;
    Color metalBase = ColorBrightness(COLOR_SUNKEN_INK, 0.4f);
    Color gold = COLOR_PYRITE_BRASS;
    Color leather = COLOR_BURNT_SIENNA;

    float cosR = cosf(rotation);
    float sinR = sinf(rotation);
    float cosP = cosf(rotation + PI/2.0f);
    float sinP = sinf(rotation + PI/2.0f);

    // HILT & POMMEL
    Vector2 hiltEnd = { position.x - cosR * hiltLength, position.y - sinR * hiltLength };
    DrawLineEx(position, hiltEnd, 3.0f, leather);
    DrawCircleV(hiltEnd, 2.5f, gold);

    // THE GUARD
    Vector2 gL = { position.x - cosP * guardWidth/2, position.y - sinP * guardWidth/2 };
    Vector2 gR = { position.x + cosP * guardWidth/2, position.y + sinP * guardWidth/2 };
    DrawLineEx(gL, gR, guardThick, gold);

    // BLADE POINTS
    Vector2 bL = { position.x - cosP * bladeWidth/2, position.y - sinP * bladeWidth/2 };
    Vector2 bR = { position.x + cosP * bladeWidth/2, position.y + sinP * bladeWidth/2 };
    Vector2 sL = { bL.x + cosR * bladeLength, bL.y + sinR * bladeLength };
    Vector2 sR = { bR.x + cosR * bladeLength, bR.y + sinR * bladeLength };
    Vector2 tip = { position.x + cosR * (bladeLength + tipLength), position.y + sinR * (bladeLength + tipLength) };

    // BLADE GEOMETRY
    DrawTriangle(sL, bL, position, metalBase);
    DrawTriangle(tip, sL, position, metalBase);
    DrawTriangle(bR, sR, position, metalBase);
    DrawTriangle(sR, tip, position, metalBase);

    // FINISHING SHINE
    DrawLineV(position, tip, metalShadow);
}

void DrawSimpleBat(Vector2 position, float rotation) {
    float handleLength = 10.0f;
    float barrelLength = 32.0f;
    float handleWidth  = 2.5f;
    float barrelWidth  = 6.5f;
    float knobRadius   = 2.2f;

    Color woodBase   = COLOR_GUITAR_AMBER;
    Color woodShadow = ColorBrightness(COLOR_BURNT_SIENNA, -0.3f);
    Color tapeGrip   = COLOR_PULP_PAPER;
    Color knobColor  = COLOR_BURNT_SIENNA;

    float cosR = cosf(rotation);
    float sinR = sinf(rotation);
    float cosP = cosf(rotation + PI / 2.0f);
    float sinP = sinf(rotation + PI / 2.0f);

    // 1. KNOB / POMMEL
    Vector2 knobPos = { position.x - cosR * 2.0f, position.y - sinR * 2.0f };
    DrawCircleV(knobPos, knobRadius, knobColor);

    // 2. TAPED HANDLE
    Vector2 handleEnd = { position.x + cosR * handleLength, position.y + sinR * handleLength };
    DrawLineEx(position, handleEnd, handleWidth, tapeGrip);

    // Diagonal tape lines for grip texture
    for (float i = 2.0f; i < handleLength; i += 2.5f) {
        Vector2 tapeSeg = { position.x + cosR * i, position.y + sinR * i };
        Vector2 tL = { tapeSeg.x - cosP * 1.5f, tapeSeg.y - sinP * 1.5f };
        Vector2 tR = { tapeSeg.x + cosP * 1.5f, tapeSeg.y + sinP * 1.5f };
        DrawLineV(tL, tR, COLOR_SUNKEN_INK);
    }

    // 3. BARREL GEOMETRY (CORRECTED VERTEX WINDING)
    Vector2 barrelEnd = { handleEnd.x + cosR * barrelLength, handleEnd.y + sinR * barrelLength };

    float halfHandle = handleWidth * 0.5f;
    float halfBarrel = barrelWidth * 0.5f;

    // Corner points offset perpendicular to rotation
    Vector2 hL = { handleEnd.x - cosP * halfHandle, handleEnd.y - sinP * halfHandle };
    Vector2 hR = { handleEnd.x + cosP * halfHandle, handleEnd.y + sinP * halfHandle };
    Vector2 bL = { barrelEnd.x - cosP * halfBarrel, barrelEnd.y - sinP * halfBarrel };
    Vector2 bR = { barrelEnd.x + cosP * halfBarrel, barrelEnd.y + sinP * halfBarrel };

    // Wood body fill (Counter-Clockwise order for both triangles forming the quad)
    DrawTriangle(hL, hR, bR, woodBase);
    DrawTriangle(hL, bR, bL, woodBase);

    // 4. ROUNDED BARREL END CAP
    // Radius matches halfBarrel (3.25f) to flush perfectly with bL and bR
    DrawCircleV(barrelEnd, halfBarrel, woodBase);

    // 5. ACCENT SHADOW LINE (Underneath edge only, no huge triangle overwrite)
    // DrawLineEx(hL, bL, 1.2f, woodShadow);
}

void DrawWeapon(int weapon_id,Vector2 position, float rotation){
    switch (weapon_id) {
        case 5: DrawSimpleSword(position, rotation);return;
        case 12: DrawSimpleGuitar(position, rotation);return;
        case 14: DrawSimpleBat(position, rotation);return;
        default: return;
    }
}
