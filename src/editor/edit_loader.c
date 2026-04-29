#include "editor/edit_loader.h"

#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "engine/palette.h"

void LoadFiles(EditSession* session){
    DIR *d;
    struct dirent *dir;
    d = opendir("data/maps/");
    if (d) {
        int jump = 0;
        int i =0;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".map") != NULL) {
                // Strip the extension for a cleaner look
                char cleanName[256];
                // Color text_color = i++ == session->current_load ? COLOR_CELADON : COLOR_SUNKEN_INK;
                strncpy(cleanName, dir->d_name, strlen(dir->d_name) - 4);
                cleanName[strlen(dir->d_name) - 4] = '\0';
                int width = MeasureText(cleanName, 20);
                session->loaded_files.buttons[i]=(Rectangle) {SCREEN_WIDTH/4, 180+jump , width, 20};
                sprintf(session->loaded_files.names[i++],"%s", cleanName);
                jump += 30;
            }
        }
        session->loaded_files.file_count = i;
        closedir(d);
    } else {
        printf("Could not open directory 'data/maps/'\n");
    }
}

bool UpdateLoadCurrent(EditSession* session, Input* input){
  if(input->buttons_pressed & KEY_W_PRESSED){
    session->loaded_files.current_file--;
    if(session->loaded_files.current_file == -1){
      session->loaded_files.current_file = session->loaded_files.file_count -1;
    }
  }
  if(input->buttons_pressed & KEY_S_PRESSED){
      session->loaded_files.current_file++;
    if(session->loaded_files.current_file== session->loaded_files.file_count){
      session->loaded_files.current_file = 0;
    }
  }
  if (input->buttons_pressed & ENTER_PRESSED) return false;

  return true;
}


bool UpdateLoadPrompt(EditSession* session,Input* input){
    for(int i =0;i< session->loaded_files.file_count;i++){
        if(CheckCollisionPointRec(input->mouse, session->loaded_files.buttons[i])) {
            session->loaded_files.current_file = i;
            if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
                return true;
            }
        }
    }
    if(!UpdateLoadCurrent(session, input)){
        return true;
    }
    return false;
}

void DrawLoadForm(EditSession* session){
    ClearBackground(COLOR_PULP_PAPER);
    int margin = 50;
    int uiWidth = SCREEN_WIDTH - (margin * 2);

    DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line
    DrawText("--- Available Maps ---\n", margin, 40, 30, COLOR_DUSTY_ROSE);
    DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); //
      for(int i = 0;i<session->loaded_files.file_count;i++){
          Color current_color = session->loaded_files.current_file == i ? COLOR_AUREOLIN: COLOR_SUNKEN_INK;
          DrawText(session->loaded_files.names[i], session->loaded_files.buttons[i].x, session->loaded_files.buttons[i].y,20, current_color);
      }
    }
