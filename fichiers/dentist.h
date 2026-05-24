#ifndef ACTIONS_H
#define ACTIONS_H

#include "map.h"
#include "items.h"
#include "patients.h"

// Prix des outils
#define GLOVE_PRICE   2
#define PROBE_PRICE   10
#define MIRROR_PRICE  15
#define SUCTION_PRICE 15
#define SYRINGE_PRICE 20
#define CLAMP_PRICE   20
#define DRILL_PRICE   10
#define COTTON_PRICE  30


enum GLOVES_STATUS {
    BAREHANDS,
    GLOVES_CLEAN,
    GLOVES_USED,
}; // l'état des gants du joueur, qui va influencer la propreté des outils ramassés et donc le soin des patients

enum TRAIL {
    NONE,
    TRAIL_CLEAN,
    TRAIL_DIRTY,
};

// Les infos générale du joueur
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