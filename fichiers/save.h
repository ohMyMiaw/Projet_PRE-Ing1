#ifndef SAVE_H
#define SAVE_H
#include "display.h"
#ifndef _WIN32
char getch();
#endif

void saveDisplay();
void saveGame(Player P1);
void saving(Player P1, int saveSlot);
Player loadGame(int saveSlot);

#endif