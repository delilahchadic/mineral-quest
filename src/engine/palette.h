// palette.h
#ifndef PALETTE_H
#define PALETTE_H

#include "raylib.h"

// The "Highland" Sun-Bleach Colors
#define COLOR_ASPHALT     (Color){ 45, 45, 48, 255 }
#define COLOR_DUSTY_ROAD  (Color){ 214, 190, 150, 255 }
#define COLOR_BEAVIS_SHIRT (Color){ 51, 153, 255, 255 }

// The "Bebop" Night Colors
#define COLOR_VHS_BLUE    (Color){ 0, 20, 100, 255 }
#define COLOR_NEON_CRIMSON (Color){ 220, 20, 60, 255 }
#define COLOR_TAROT_GOLD   (Color){ 255, 215, 0, 255 }

// The "Americana" Warm Tones (Faded, dusty, sun-bleached)
#define COLOR_DUSTY_ROSE    (Color){ 194, 118, 128, 255 }
#define COLOR_DUSTY_SALMON  (Color){ 214, 140, 121, 255 }
#define COLOR_DUSTY_CORAL   (Color){ 205, 126, 107, 255 }
#define COLOR_MUSTARD       (Color){ 203, 153, 50, 255 }

// The "Bebop/Space" Cool Tones (Deep, punchy, technical)
#define COLOR_PRUSSIAN_BLUE (Color){ 0, 49, 83, 255 }
#define COLOR_ULTRAMARINE   (Color){ 18, 10, 143, 255 }
#define COLOR_JADE          (Color){ 0, 168, 107, 255 }

// A bonus "Slacker" background color (The color of a dirty Texas sky)
#define COLOR_TEXAS_HAZE    (Color){ 220, 210, 190, 255 }

// The "Pulp Novel" - An off-white/beige that feels like old paper
#define COLOR_PULP_PAPER    (Color){ 225, 210, 185, 255 } 

// The "Sun-Aged Ink" - A deep, dark yellow-tinted black (Game Boy "Dark" style)
#define COLOR_SUNKEN_INK    (Color){ 35, 30, 20, 255 }

// Optional: A "Highlighter" color for that 90s office look
#define COLOR_OFFICE_LEMON  (Color){ 210, 200, 80, 255 }
// --- THE CERULEAN CORE ---
// The anchor point: a classic, pigment-rich sky blue.
#define COLOR_CERULEAN_CORE      (Color){ 42,  82,  190, 255 }
// High Chroma: More "electric," good for highlighting interactive items.
#define COLOR_CERULEAN_VIVID     (Color){ 0,   123, 167, 255 }
// Low Chroma: Desaturated, like weathered paint or old pottery.
#define COLOR_CERULEAN_FADED     (Color){ 93,  118, 130, 255 }
#define COLOR_CERULEAN_DUSTY     (Color){ 135, 165, 190, 255 }
// --- HUE SHIFTS (Warm & Cool) ---
// Toward Yellow: A warmer, sea-depth blue.
#define COLOR_CERULEAN_TEAL      (Color){ 54,  117, 136, 255 }
// Toward Blue: A purer, punchier cobalt.
#define COLOR_CERULEAN_COBALT    (Color){ 0,   71,  171, 255 }
// Toward Red: Deep, dark, and slightly "bloody" blue.
#define COLOR_CERULEAN_PRUSSIAN  (Color){ 0,   49,  83,  255 }

// --- THE BRIDGES ---
// Between Cerulean and Wisteria (The Lavender/Violet bridge)
#define COLOR_CERULEAN_WISTFUL   (Color){ 100, 149, 237, 255 }

// Between Cerulean and Mint (The Blue-Green bridge)
#define COLOR_CERULEAN_BERYL     (Color){ 119, 158, 145, 255 }

// The "Dusty" Greens & Earths
#define COLOR_DUSTY_SAP      (Color){ 131, 145, 102, 255 } // Muted, olive-leaning green
#define COLOR_SAGE_BRUSH     (Color){ 147, 157, 143, 255 } // Very low chroma, almost silver-green
#define COLOR_OCHRE_SILT     (Color){ 191, 156, 107, 255 } // A dusty, sandy yellow-brown
#define COLOR_TERRA_PALE     (Color){ 175, 128, 114, 255 } // A lighter, clay-like earth tone

// Low-Chroma Accents
#define COLOR_SLATE_MIST     (Color){ 118, 134, 146, 255 } // A blue-gray that isn't too "electric"
#define COLOR_WITHERED_LILAC (Color){ 158, 136, 152, 255 } // A purple that feels like it’s been in the sun
#define COLOR_BONE_WHITE     (Color){ 220, 215, 201, 255 } // Better for "white" text/UI than pure 255,255,255
#define COLOR_DEEP_MOSS      (Color){ 85,  94,  70,  255 } // A dark green for shadows or deeper foliage

// --- THE WARM SPECTRUM (Reds & Earths) ---
#define COLOR_MAGENTA          (Color){ 202,  31, 123, 255 } // Quin. Magenta (PR122)
#define COLOR_ROSE             (Color){ 184,  32,  72, 255 } // Quin. Rose (PV19)
#define COLOR_RED_OCHRE        (Color){ 166,  68,  46, 255 } // Earthy PR101
#define COLOR_SCARLET          (Color){ 255,  36,   0, 255 } // Vibrant Scarlet
#define COLOR_PERYLENE_RED     (Color){ 177,  24,  44, 255 } // Deep, bloody red (PR149)
#define COLOR_POTTERS_PINK     (Color){ 195, 141, 131, 255 } // Granulating, desaturated pink

// --- THE GOLDEN SPECTRUM (Yellows & Browns) ---
#define COLOR_QUIN_GOLD        (Color){ 178, 125,  38, 255 } // Glowing PO49/PY150 blend
#define COLOR_AMBER            (Color){ 255, 191,   0, 255 } 
#define COLOR_SAFFRON          (Color){ 244, 196,  48, 255 } 
#define COLOR_YELLOW_OCHRE     (Color){ 203, 153,  50, 255 } // Golden-earth PY42
#define COLOR_BURNT_SIENNA     (Color){ 140,  71,  44, 255 } // Warm brown PBr7
#define COLOR_CADMIUM_YELLOW   (Color){ 255, 246,   0, 255 } // Punchy, opaque yellow
#define COLOR_NICKEL_TITANITE  (Color){ 238, 230, 150, 255 } // Pale, cool "Lemon Sorbet"
#define COLOR_NAPLES_YELLOW    (Color){ 250, 218,  94, 255 } // Creamy, earthy yellow
// #define COLOR_MUSTARD          (Color){ 225, 173,   1, 255 } 

// --- THE COOL SPECTRUM (Blues & Greens) ---
#define COLOR_ULTRAMARINE      (Color){  18,  10, 143, 255 } // Deep warm blue (PB29)
#define COLOR_CERULEAN         (Color){  42,  82, 190, 255 } // Sky-like PB35
#define COLOR_COBALT_BLUE      (Color){   0,  71, 171, 255 } // Pure, neutral blue
#define COLOR_PRUSSIAN_BLUE    (Color){   0,  49,  83, 255 } // Moody, dark PB27
#define COLOR_PTHALO_BLUE      (Color){   0,  15, 137, 255 } // Intense, staining blue
#define COLOR_PTHALO_TEAL      (Color){   0, 128, 128, 255 } 
#define COLOR_VIRIDIAN         (Color){   0, 127, 106, 255 } // Cool, transparent green
#define COLOR_SAP_GREEN        (Color){  106, 125, 45, 255 } // Natural, leafy green
#define COLOR_GREEN_GOLD       (Color){  170, 163,  58, 255 } // Glowing, mossy yellow-green
#define COLOR_MAY_GREEN        (Color){  115, 190,  69, 255 } // Bright, spring green
#define COLOR_CELADON          (Color){  172, 192, 172, 255 } // Pale, minty ceramic gray

// --- THE "ARTIST'S CHOICE" (Bonus Recommendations) ---
#define COLOR_MOONGLOW         (Color){  111, 103, 138, 255 } // Daniel Smith's famous granulating violet-grey
#define COLOR_AUREOLIN         (Color){  255, 238,   0, 255 } // The essential transparent cool yellow
#define COLOR_INDANTHRONE_BLUE (Color){   19,  35,  77, 255 }
#endif