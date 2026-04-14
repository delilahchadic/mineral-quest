#include "environment/map.h"

static void LoadMapEntityFile(const char* filename, Map* map){
  map->entities = NULL;
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filename);
    return;
  }
  TraceLog(LOG_INFO,"File Loaded - %s", filename);
  char line[1024];
  fgets(line, sizeof(line),file);//header
  // int col =0;
  while (fgets(line, sizeof(line), file)) {
    // col = 0;
    line[strcspn(line, "\n")] = 0;
    char* typeToken = strtok(line,",");
    char* xToken = strtok(NULL,",");
    char* yToken = strtok(NULL,",");
    char* idToken = strtok(NULL,",");
    if(typeToken && xToken && yToken){
      MapEntity* m = malloc(sizeof(MapEntity)); 
      if (m == NULL) continue; // Safety check
      m->type = (EntityType) atoi(typeToken);
      m->jumpoffset = 0.0f;
      m->position = (Vector2) {(float)atoi(xToken), (float)atoi(yToken)};
      m->next = NULL;
      m->id = atoi(idToken);
      Add_Entity(map,m);
    }
    
  }
    fclose(file);
}

static void LoadMapGridFile(const char* filename, Map* map){
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filename);
    return;
  }
  TraceLog(LOG_INFO,"File Loaded - %s", filename);
  char line[1024];
  // Read Header
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

  //cycle through file to get types
  while (row<map->rows) {
    fgets(line, sizeof(line), file);
    line[strcspn(line, "\n")] = 0;
    char* colToken = strtok(line,",");
    for (col = 0;col<map->columns;col++){
      if (colToken != NULL)
      {map->grid[row][col].type = atoi(colToken);
      colToken = strtok(NULL,",");} 
    }
    row++;
  }
  // process blank line
  fgets(line, sizeof(line), file);
  //process for tile heigh
  row=0;
  while (row<map->rows) {
    fgets(line, sizeof(line), file);
    line[strcspn(line, "\n")] = 0;
    char* colToken = strtok(line,",");
    for (col = 0;col<map->columns;col++){
      if (colToken != NULL)
      {map->grid[row][col].height = atoi(colToken);
      colToken = strtok(NULL,",");
    }
      
    }
    
    row++;
  }
    fclose(file);
}

void LoadMap(const char* mapName, Map* map){
  memset(map, 0, sizeof(Map));
  
  char gridPath[256];
  char entsPath[256];

  // Construct paths automatically: "highway" -> "data/maps/highway.map"
  snprintf(gridPath, sizeof(gridPath), "data/maps/%s.map", mapName);
  snprintf(entsPath, sizeof(entsPath), "data/maps/%s.ents", mapName);
  LoadMapGridFile(gridPath, map);
  LoadMapEntityFile(entsPath, map);
}