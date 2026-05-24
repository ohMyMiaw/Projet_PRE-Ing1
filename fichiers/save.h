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
    int chaise_patient[4];
    Plateau plateaux[4];
    int patient_furieux;
    bool chaise_soignee[4];
    int timer_prochain;
    int prochain_patient;
} GameState;

void saveDisplay();
void saveGame(GameState gameState);
void saving(GameState gameState, int saveSlot);
GameState loadGame(int saveSlot);

#endif