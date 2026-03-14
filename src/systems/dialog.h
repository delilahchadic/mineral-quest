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


typedef struct Dialog_Manager{
  Dialog* active_dialog;
  int count;
  int capacity;
  bool active;
  int currentID;
} Dialog_Manager;


void Init_Dialog_Manager(Dialog_Manager* manager, int initialCapacity);
void Draw_Dialog(Dialog_Manager* manager);
void Set_Active_Dialog(Dialog_Manager* manager, int id);
void Update_Dialog_Manager(Dialog_Manager* manager);
#endif