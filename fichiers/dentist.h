#ifndef ACTIONS_H
#define ACTIONS_H

#include "map.h"
#include "items.h"
#include "patients.h"


enum GLOVES_STATUS {
    BAREHANDS,
    GLOVES_CLEAN,
    GLOVES_USED,
};

enum TRAIL {
    NONE,
    TRAIL_CLEAN,
    TRAIL_DIRTY,
};


typedef struct {
    int x;
    int y;
    enum GLOVES_STATUS hasGloves;
    enum Object objectId;
    enum TRAIL trail;
    bool objectInfected;
    int money;
    int moneySpent;
} Player;

void action_pick_up   (Player *P1, Cells grid[M_HEIGHT][M_WIDTH], Tray tray[4], int chair_patient[4], PatientList *patientList);
void action_interact  (Player *P1, Cells grid[M_HEIGHT][M_WIDTH], Tray tray[4], int chair_patient[4], PatientList *patientList, bool neat_chair[4], int *furious_patient, int *unsatisfied_patient, int *satisfied_patient);

#endif