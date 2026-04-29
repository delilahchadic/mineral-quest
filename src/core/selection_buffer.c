#include "core/selection_buffer.h"

void InitSelection(SelectionBuffer* sb, int w, int h) {
    sb->width = w;
    sb->height = h;
    int totalTiles = w * h;
    // Calculate how many 32-bit integers we need
    int numChunks = (totalTiles + 31) / 32;
    sb->bits = (uint32_t*)calloc(numChunks, sizeof(uint32_t));
}

void SetTileSelected(SelectionBuffer* sb, int x, int y, bool selected) {
    if (x < 0 || x >= sb->width || y < 0 || y >= sb->height) return;
    int index = y * sb->width + x;
    int chunk = index / 32;
    int bit = index % 32;

    if (selected) {
        sb->bits[chunk] |= (1U << bit);  // Set bit to 1
    } else {
        sb->bits[chunk] &= ~(1U << bit); // Set bit to 0
    }
}

bool IsTileSelected(SelectionBuffer* sb, int x, int y) {
    if (x < 0 || x >= sb->width || y < 0 || y >= sb->height) return false;
    int index = y * sb->width + x;
    int chunk = index / 32;
    int bit = index % 32;
    return (sb->bits[chunk] & (1U << bit)) != 0;
}

void ClearSelection(SelectionBuffer* sb) {
    if (sb == NULL || sb->bits == NULL) return;

    int totalTiles = sb->width * sb->height;
    int numChunks = (totalTiles + 31) / 32;

    // Efficiently zero out the entire memory block
    memset(sb->bits, 0, numChunks * sizeof(uint32_t));
}

void CloseSelection(SelectionBuffer* sb) {
    if (sb == NULL) return;

    if (sb->bits != NULL) {
        free(sb->bits);   // 1. Free the array of bits
        sb->bits = NULL;  // 2. Safety: prevent accidental double-free
    }

}
