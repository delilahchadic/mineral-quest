#ifndef PALETTE_H
#define PALETTE_H

#include "raylib.h"

// --- THE UI CORE (Systems & Text) ---
#define COLOR_PULP_PAPER     (Color){ 225, 210, 185, 255 } // Old paper/Background
#define COLOR_SUNKEN_INK     (Color){ 35, 30, 20, 255 }    // Primary text
#define COLOR_BONE_WHITE     (Color){ 220, 215, 201, 255 } // Soft UI white
#define COLOR_OFFICE_LEMON   (Color){ 210, 200, 80, 255 }  // 90s Highlighter
#define COLOR_TEXAS_HAZE     (Color){ 220, 210, 190, 255 } // Dirty sky/Panel accent

// --- THE AMERICANA / DUSTY SPECTRUM ---
#define COLOR_ASPHALT        (Color){ 45, 45, 48, 255 }
#define COLOR_DUSTY_ROAD     (Color){ 214, 190, 150, 255 }
#define COLOR_DUSTY_ROSE     (Color){ 194, 118, 128, 255 }
#define COLOR_DUSTY_SALMON   (Color){ 214, 150, 131, 255 }
#define COLOR_DUSTY_CORAL    (Color){ 205, 126, 107, 255 }
#define COLOR_DUSTY_SAP      (Color){ 131, 145, 102, 255 }
#define COLOR_SAGE_BRUSH     (Color){ 140, 160, 145,255 }
#define COLOR_OCHRE_SILT     (Color){ 191, 156, 107, 255 }
#define COLOR_TERRA_PALE     (Color){ 175, 128, 114, 255 }
#define COLOR_PALE_SAND      (Color){ 228, 214, 182, 255 }
#define COLOR_BLUE_OCHRE     (Color){ 128,136,160, 255 }
#define COLOR_BASALT     (Color){ 60,59, 77, 255 }
#define COLOR_PERIWINKLE_SLATE     (Color){ 128, 128, 191, 255 }
#define COLOR_GUITAR_AMBER (Color){ 215, 150, 85, 255 }

// --- THE CERULEAN CORE & HUE SHIFTS ---
#define COLOR_CERULEAN_CORE     (Color){ 42, 82, 190, 255 }
#define COLOR_CERULEAN_VIVID    (Color){ 0, 123, 167, 255 }
#define COLOR_CERULEAN_FADED    (Color){ 93, 118, 130, 255 }
#define COLOR_CERULEAN_DUSTY    (Color){ 135, 165, 190, 255 }
#define COLOR_CERULEAN_TEAL     (Color){ 54, 117, 136, 255 }
#define COLOR_CERULEAN_COBALT   (Color){ 0, 71, 171, 255 }
#define COLOR_CERULEAN_WISTFUL  (Color){ 100, 149, 237, 255 }
#define COLOR_CERULEAN_BERYL    (Color){ 0, 210, 225, 255 }

// --- THE WARM SPECTRUM (Artist Pigments) ---
#define COLOR_MAGENTA        (Color){ 202, 31, 123, 255 }
#define COLOR_ROSE           (Color){ 184, 32, 72, 255 }
#define COLOR_RED_OCHRE      (Color){ 145, 95, 85, 255 }
#define COLOR_SCARLET        (Color){ 255, 36, 0, 255 }
#define COLOR_PERYLENE_RED   (Color){ 177, 24, 44, 255 }
#define COLOR_POTTERS_PINK   (Color){ 195, 141, 131, 255 }
#define COLOR_PURPLE_OCHRE   (Color){ 196, 160, 170, 255 }
#define COLOR_SHELL_PINK      (Color){ 215, 185, 185, 255 } // Muted Berry-leaning Pink
#define COLOR_BRILLIANT_JAUNE (Color){ 225, 190, 140, 255 } // Muted Golden Earth

// --- THE GOLDEN SPECTRUM ---
#define COLOR_QUIN_GOLD      (Color){ 178, 125, 38, 255 }
#define COLOR_AMBER          (Color){ 255, 191, 0, 255 }
#define COLOR_SAFFRON        (Color){ 244, 196, 48, 255 }
#define COLOR_YELLOW_OCHRE   (Color){ 204, 158, 78, 255 }
#define COLOR_BURNT_SIENNA   (Color){ 140, 71, 44, 255 }
#define COLOR_CADMIUM_YELLOW (Color){ 255, 246, 0, 255 }
#define COLOR_CHROME_YELLOW  (Color){ 255, 183, 0, 255 }
#define COLOR_NICKEL_TITANITE (Color){ 238, 230, 150, 255 }
#define COLOR_NAPLES_YELLOW  (Color){ 250, 218, 94, 255 }
#define COLOR_BUFF_TITANIUM  (Color){ 235, 222, 194, 255 }
#define COLOR_FIRED_GOLD_OCHRE  (Color){ 200, 120, 70,255 }

// --- THE COOL SPECTRUM ---
#define COLOR_ULTRAMARINE      (Color){ 18, 10, 143, 255 }
#define COLOR_COBALT_BLUE      (Color){ 0, 71, 171, 255 }
#define COLOR_PRUSSIAN_BLUE    (Color){ 0, 49, 83, 255 }
#define COLOR_INDANTHRONE_BLUE (Color){ 19, 35, 77, 255 }
#define COLOR_PTHALO_BLUE      (Color){ 0, 15, 137, 255 }
#define COLOR_PTHALO_TEAL      (Color){ 0, 128, 128, 255 }
#define COLOR_JADE             (Color){ 0, 168, 107, 255 }
#define COLOR_VIRIDIAN         (Color){ 0, 127, 106, 255 }
#define COLOR_WITHERED_VIRIDIAN (Color){ 96, 127, 121, 255 } // Ultra-muted, chalky mineral viridian slate
#define COLOR_SAP_GREEN        (Color){ 106, 125, 45, 255 }
#define COLOR_GREEN_GOLD       (Color){ 170, 163, 58, 255 }
#define COLOR_MAY_GREEN        (Color){ 115, 190, 69, 255 }
#define COLOR_CELADON          (Color){ 172, 192, 172, 255 }
#define COLOR_DEEP_MOSS        (Color){ 85, 94, 70, 255 }
#define COLOR_AMAZONITE        (Color){ 135, 195, 185,255 }


// --- SPECIAL EFFECTS & MISC ---
#define COLOR_VHS_BLUE       (Color){ 0, 20, 100, 255 }
#define COLOR_NEON_CRIMSON   (Color){ 220, 20, 60, 255 }
#define COLOR_TAROT_GOLD     (Color){ 255, 215, 0, 255 }
#define COLOR_BEAVIS_SHIRT   (Color){ 51, 153, 255, 255 }
#define COLOR_SLATE_MIST     (Color){ 118, 134, 146, 255 }
#define COLOR_WITHERED_LILAC (Color){ 150, 140, 148, 255 } // Desaturated dusty mineral lilac
#define COLOR_MOONGLOW       (Color){ 111, 103, 138, 255 }
#define COLOR_AUREOLIN       (Color){ 255, 238, 0, 255 }

#define COLOR_PYRITE_BRASS    (Color){ 185, 160, 50, 255 }

// --- CAMOUFLAGE & RHYOLITE SPECTRUM ---
#define COLOR_CAMO_WOODLAND_GREEN (Color){ 78, 91, 49, 255 }  // Standard US Woodland base green
#define COLOR_CAMO_WOODLAND_BROWN (Color){ 89, 62, 44, 255 }  // Deep foliage earth brown
#define COLOR_CAMO_WOODLAND_KHAKI (Color){ 148, 137, 102, 255 } // Muted olive-drab neutral ground
#define COLOR_CAMO_DESERT_SAND    (Color){ 212, 185, 150, 255 } // Pale desert sand ground
#define COLOR_CAMO_DESERT_EARTH   (Color){ 168, 128, 92, 255 }  // Muted clay tan shadow
#define COLOR_CAMO_DESERT_OAT    (Color){ 188, 168, 132, 255 } // Bleached brush neutral
#define COLOR_RHYOLITE_TUFF        (Color){ 222, 192, 180, 255 }
// --- EXPERIMENTAL SPECTRUM ---
#define COLOR_COBALT_TEAL_PALE    (Color){ 150, 205, 200, 255 } // Muted PG50 mineral cyan
#define COLOR_LIVID_SLATE         (Color){  92, 105, 118, 255 } // Low-chroma 90s CRT shadow tone
#define COLOR_KHMER_SANDSTONE     (Color){ 198, 172, 142, 255 } // Soft weathered temple stone
#define COLOR_CAPUT_MORTUUM       (Color){ 112,  66,  65, 255 } // Deep granulating iron-oxide purple-brown
#define COLOR_BLEACHED_DRIFTWOOD  (Color){ 185, 180, 170, 255 } // Low-chroma coast/desert bone neutral
#define COLOR_SNOOT_PINK          (Color){ 238, 218, 215, 255 } // High-value, desaturated pale-rose highlight

// --- METALLIC BASE & LUSTER SPECTRUM ---
// Pyrite / Brass Set (Complements COLOR_PYRITE_BRASS)
#define COLOR_PYRITE_SHADOW      (Color){ 85, 72, 25, 255 }   // Deep greenish-brass shadow / oxidation
#define COLOR_PYRITE_GLEAM       (Color){ 225, 205, 85, 255 }  // High-chroma metallic mid-shine
#define COLOR_PYRITE_SPECULAR    (Color){ 245, 240, 180, 255 } // Pale, near-white specular point light

// Hematite / Specular Iron Set
#define COLOR_HEMATITE_BASE      (Color){ 52, 50, 56, 255 }   // Dark metallic charcoal base
#define COLOR_HEMATITE_LUSTER    (Color){ 115, 118, 130, 255 } // Cool metallic specular midtone
#define COLOR_HEMATITE_SPECULAR  (Color){ 210, 215, 225, 255 } // Sharp metallic glint highlight

// Raw Copper / Aged Bronze Set
#define COLOR_COPPER_OXIDE       (Color){ 75, 42, 30, 255 }   // Dark burnt shadow
#define COLOR_RAW_COPPER         (Color){ 184, 95, 62, 255 }  // Warm metallic copper midtone
#define COLOR_COPPER_SPECULAR    (Color){ 235, 180, 150, 255 } // Pale metallic peach highlight

// --- ROW 6: EXPANDED SHADOW, FOILAGE & MINERAL TRANSITIONS ---
#define COLOR_PRUSSIAN_SHADOW    (Color){ 25, 45, 55, 255 }   // Deep, dark blue-green for deep water/night shadows
#define COLOR_PTHALO_EMERALD     (Color){ 35, 105, 85, 255 }  // Cool-leaning pine/emerald green for foliage depth
#define COLOR_OLIVE_DRAB         (Color){ 95, 100, 55, 255 }  // Dark neutral yellow-green bridging grass and deep moss
#define COLOR_RAW_UMBER          (Color){ 70, 55, 45, 255 }    // Cold, dark earth brown for unlit soil and bark
// #define COLOR_HEMATITE_BASE      (Color){ 52, 50, 56, 255 }   // Dark metallic charcoal base for iron and slate
#define COLOR_PERIWINKLE_SLATE   (Color){ 128, 128, 191, 255 }// Cool, muted twilight violet-blue
// #define COLOR_AUREOLIN           (Color){ 240, 210, 60, 255 }  // Cold, bright mineral yellow highlight
// --- MUTED FUCHSIA / COBALT VIOLET SPECTRUM ---
#define COLOR_MUTED_FUCHSIA      (Color){ 148, 118, 134, 255 } // 25% more muted / dusty slate-fuchsia
#define COLOR_DUSTY_MAGENTA      (Color){ 182, 112, 142, 255 } // Slightly warmer, desaturated quinacridone tone
#define COLOR_SLATE_FUCHSIA      (Color){ 145, 100, 130, 255 } // Low-chroma, twilight shadow fuchsia
// Cool chalky limestone / salt flat highlight
#define COLOR_LIMESTONE_CHALK   (Color){ 222, 220, 212, 255 }

// Neutral granite / exposed bedrock stone mid-tone
#define COLOR_GRANITE_MID       (Color){ 138, 136, 132, 255 }

// Deep damp peat / charred underbrush shadow
#define COLOR_PEAT_BLACK        (Color){  42,  36,  34, 255 }
#endif
