#ifndef DIALOG_H
#define DIALOG_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "engine/palette.h"
#include "engine/common.h"
#include "engine/register.h"


typedef struct DialogManager{
  Dialog* active_dialog;
  int count;
  int capacity;
  bool active;
  int currentID;
} DialogManager;


void Init_Dialog_Manager(DialogManager* manager, int initialCapacity);
void Draw_Dialog(DialogManager* manager);
void Set_Active_Dialog(DialogManager* manager, int id);
void Update_Dialog_Manager(DialogManager* manager);
#endif