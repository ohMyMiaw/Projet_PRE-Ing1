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

enum TRAIL{
    NONE,
    TRAIL_CLEAN,
    TRAIL_DIRTY,
};



typedef struct {
    int x;
    int y;
    enum GLOVES_STATUS hasGloves;
    enum Object objetId;
    enum TRAIL trail;
    bool objetInfected;
    int money;
    int moneyEarned;
} Player;


void display(Player P1, PatientList patientList, int prochain_patient, int chaise_patient[4], int timer_prochain, bool chaise_soignee[4], Plateau plateaux[4], int patient_furieux);


#endif
