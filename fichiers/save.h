#ifndef SAVE_H
#define SAVE_H
#include "display.h"
#include <stdbool.h>
#ifndef _WIN32
char getch();
#endif

typedef struct {
    Player P1;
    PatientList patientList;
    int next_patient;         // CORRIGÉ : remplace le next_timer en double
    int chair_patient[4];     // CORRIGÉ : aligné avec display.c
    int next_timer;
    bool neat_chair[4];       // CORRIGÉ : aligné avec display.c
    Tray tray[4];             // CORRIGÉ : tray sans "s" pour matcher display.c
    int furious_patient;
} GameState;

void saveDisplay();
void saveGame(GameState gameState);
void saving(GameState gameState, int saveSlot);
GameState loadGame(int saveSlot);

#endif