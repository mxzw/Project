#pragma once

#include "Common.h"

void SetCurPos(int x, int y);
void HideCursor();
void DrawCol(int x, int y);
void DrawRow(int x, int y);
void DrawFrame(char* title);
void DrawMenu();
void SystemEnd();

#define MAX_TITLE_SIZE  128
#define WIDTH 120
#define HEIGHT 30


