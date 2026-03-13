#ifndef DIALOG_H
#define DIALOG_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
typedef struct Dialog
{
  char* character_name;
  char* text;
  int id;
} Dialog;

typedef struct Dialog_Manager{
  Dialog* active_dialog;
  Dialog* database;
  int count;
  int capacity;
  bool active;
  int currentID;
} Dialog_Manager;


void Init_Dialog_Manager(Dialog_Manager* manager, int initialCapacity);
void Draw_Dialog(Dialog_Manager* manager);
void Load_Dialogs_From_CSV(Dialog_Manager* manager, const char* filename);
void Close_Dialog_Manager(Dialog_Manager* manager);
void Set_Active_Dialog(Dialog_Manager* manager, int id);
void Update_Dialog_Manager(Dialog_Manager* manager);
#endif