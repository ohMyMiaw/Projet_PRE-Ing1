#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
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


void display(Player P1, PatientList patientList, int next_patient, int chair_patient[4], int next_timer, bool neat_chair[4], Tray tray[4], int furious_patient, int unsatisfied_patient, int satisfied_patient);
#endif
