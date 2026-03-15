#include "map.h"

TileDefinition TILE_REGISTRY[6] = {
    { TILE_WATER, true,  0.5f, 101, COLOR_PRUSSIAN_BLUE },
    { TILE_STONE, false,  1.0f, 102, COLOR_TEXAS_HAZE    },
    { TILE_SAND,  false, 0.4f, 103, COLOR_DUSTY_ROSE    },
    { TILE_DIRT,  false, 0.8f, 104, COLOR_DUSTY_SALMON  },
    { TILE_GRASS, false, 0.9f, 105, COLOR_JADE          },
    { TILE_ROAD,  false, 1.0f, 106, COLOR_ASPHALT       }
};

void LoadMapGridFile(const char* filename, Map* map){
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filename);
    return;
  }
  TraceLog(LOG_INFO,"File Loaded - %s", filename);
  char line[1024];
  fgets(line, sizeof(line),file);
  char* rowToken = strtok(line,",");
  char* colToken = strtok(NULL,",");
  char* nameToken = strtok(NULL,",");
  
  if(rowToken && colToken){
    map->rows = atoi(rowToken);
    map->columns = atoi(colToken);
    strncpy(map->name, nameToken, sizeof(map->name) - 1);
    map->name[sizeof(map->name) - 1] = '\0'; 
  }
  int row =0;
  int col =0;
  while (fgets(line, sizeof(line), file) && row<map->rows) {
    // col = 0;
    line[strcspn(line, "\n")] = 0;
    char* colToken = strtok(line,",");
    // map->grid[row][0] = atoi(colToken);
    for (col = 0;col<map->columns;col++){
      map->grid[row][col] = atoi(colToken);
      colToken = strtok(NULL,",");
      
    }
    
    row++;
  }
    fclose(file);
}

void InitMap(Map* map){
  map->entities = NULL;
  LoadMapGridFile("data/maps/highway.map", map);
  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
}

void Draw_Map(Map* map) {
    for (int y = 0; y < map->rows; y++) {
        for (int x = 0; x < map->columns; x++) {
            Color tileColor = TILE_REGISTRY[map->grid[y][x]].color;
            // Tile t = world->types[world->tiles[y][x]];
            DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
            // Draw a subtle grid line
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, Fade(COLOR_SUNKEN_INK, 0.1f));
        }
    }
}

bool Check_Collision(Map* map, Vector2 nextPos) {
    // 1. Calculate the tiles for the edges of the character
    // Assuming character is 32x32
    int leftTile   = (int)(nextPos.x / TILE_SIZE);
    int rightTile  = (int)((nextPos.x + 31) / TILE_SIZE);
    int topTile    = (int)(nextPos.y / TILE_SIZE);
    int bottomTile = (int)((nextPos.y + 31) / TILE_SIZE);

    // 2. Bounds Check (The "Police Line")
    // If ANY part of the bounding box is outside 0-14 or 0-9, BLOCK IT.
    if (leftTile < 0 || rightTile >= map->columns || topTile < 0 || bottomTile >= map->rows) {
        return true; 
    }

    // 3. Tile Property Check (Check the point at his feet)
    int feetX = (int)((nextPos.x + 16) / TILE_SIZE);
    int feetY = (int)((nextPos.y + 31) / TILE_SIZE);
    
    int typeID = map->grid[feetY][feetX];
    return TILE_REGISTRY[typeID].is_blocking;
}

