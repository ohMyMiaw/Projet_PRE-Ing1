#include "dentist.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define RESET  "\033[0m"

// Prix des outils
#define GLOVE_PRICE   2
#define PROBE_PRICE   10
#define MIRROR_PRICE  15
#define SUCTION_PRICE 15
#define SYRINGE_PRICE 20
#define CLAMP_PRICE   20
#define DRILL_PRICE   10
#define COTTON_PRICE  30

ToolType objectToTool(enum Object obj) {
    switch (obj) {
        case GLOVES:  return TOOL_GLOVES;
        case DRILL:   return TOOL_DRILL;
        case COTTON:  return TOOL_COTTON;
        case MIRROR:  return TOOL_MIRROR;
        case PROBE:   return TOOL_PROBE;
        case SUCTION: return TOOL_SUCTION;
        case CLAMP:   return TOOL_CLAMP;
        case SYRINGE: return TOOL_SYRINGE;
        default:      return TOOL_NONE;
    }
}

// Gère la touche ESPACE : ramasser un outil / vider poubelle / déposer sur plateau
void action_pick_up(Player *P1, Cells grid[M_HEIGHT][M_WIDTH], Tray tray[4],
                    int chair_patient[4], PatientList *patientList) {

    if (grid[P1->y][P1->x].obj == GLOVES && P1->hasGloves == BAREHANDS) {
        P1->hasGloves = GLOVES_CLEAN;
        P1->money -= GLOVE_PRICE;
        P1->moneySpent += GLOVE_PRICE;
    }
    else if (grid[P1->y][P1->x].obj == PROBE && P1->objectId == 0) {
        P1->objectId = PROBE;
        P1->money -= PROBE_PRICE;
        P1->moneySpent += PROBE_PRICE;
        if (P1->hasGloves == BAREHANDS) P1->objectInfected = true;
    }
    else if (grid[P1->y][P1->x].obj == CLAMP && P1->objectId == 0) {
        P1->objectId = CLAMP;
        P1->money -= CLAMP_PRICE;
        P1->moneySpent += CLAMP_PRICE;
        if (P1->hasGloves == BAREHANDS) P1->objectInfected = true;
    }
    else if (grid[P1->y][P1->x].obj == SYRINGE && P1->objectId == 0) {
        P1->objectId = SYRINGE;
        P1->money -= SYRINGE_PRICE;
        P1->moneySpent += SYRINGE_PRICE;
        if (P1->hasGloves == BAREHANDS) P1->objectInfected = true;
    }
    else if (grid[P1->y][P1->x].obj == MIRROR && P1->objectId == 0) {
        P1->objectId = MIRROR;
        P1->money -= MIRROR_PRICE;
        P1->moneySpent += MIRROR_PRICE;
        if (P1->hasGloves == BAREHANDS) P1->objectInfected = true;
    }
    else if (grid[P1->y][P1->x].obj == SUCTION && P1->objectId == 0) {
        P1->objectId = SUCTION;
        P1->money -= SUCTION_PRICE;
        P1->moneySpent += SUCTION_PRICE;
        if (P1->hasGloves == BAREHANDS) P1->objectInfected = true;
    }
    else if (grid[P1->y][P1->x].obj == DRILL && P1->objectId == 0) {
        P1->objectId = DRILL;
        P1->money -= DRILL_PRICE;
        P1->moneySpent += DRILL_PRICE;
        if (P1->hasGloves == BAREHANDS) P1->objectInfected = true;
    }
    else if (grid[P1->y][P1->x].obj == COTTON && P1->objectId == 0) {
        P1->objectId = COTTON;
        P1->money -= COTTON_PRICE;
        P1->moneySpent += COTTON_PRICE;
        if (P1->hasGloves == BAREHANDS) P1->objectInfected = true;
    }
    else if (grid[P1->y][P1->x].obj == TRASH1 && P1->objectId != 0) {
        P1->objectId = 0;
        P1->objectInfected = false;
    }
    else if (grid[P1->y][P1->x].obj == TRASH2) {
        for (int i = 0; i < 4; i++) {
            if (tray[i].isDirty) {
                memset(tray[i].tools, 0, sizeof(tray[i].tools));
                tray[i].count      = 0;
                tray[i].isDirty    = false;
                tray[i].patientIdx = -1;
                P1->objectId       = 0;
                P1->objectInfected = false;
                P1->hasGloves      = BAREHANDS;
                break;
            }
        }
    }
    else if (grid[P1->y][P1->x].obj == TRAY && P1->objectId != 0) {
        int chair_idx = P1->y - 1;
        if (chair_idx >= 0 && chair_idx <= 3 && !tray[chair_idx].isDirty && !P1->objectInfected) {
            if (tray[chair_idx].count < MAX_TOOLS_ON_TRAY) {
                ToolType t = objectToTool(P1->objectId);
                if (t != TOOL_NONE && chair_patient[chair_idx] != 0) {
                    Patient *pat = &patientList->patients[chair_idx];
                    bool tool_needed = false;
                    for (int s = 0; s < pat->symptomCount; s++)
                        for (int ti = 0; ti < pat->symptoms[s].toolCount; ti++)
                            if (pat->symptoms[s].tools[ti] == t)
                                tool_needed = true;
                    bool already_present = false;
                    for (int p = 0; p < tray[chair_idx].count; p++)
                        if (tray[chair_idx].tools[p] == t)
                            already_present = true;
                    if (tool_needed && !already_present) {
                        tray[chair_idx].tools[tray[chair_idx].count++] = t;
                        P1->objectId = 0;
                    }
                }
            }
        }
    }
}

// Gère la touche E : soigner le patient
void action_interact(Player *P1, Cells grid[M_HEIGHT][M_WIDTH], Tray tray[4],
                     int chair_patient[4], PatientList *patientList, bool neat_chair[4],
                     int *furious_patient, int *unsatisfied_patient, int *satisfied_patient) {

    int chair_idx = P1->y - 1;
    if (chair_idx < 0 || chair_idx > 3) return;

    if (P1->hasGloves != GLOVES_CLEAN) {
        if (chair_patient[chair_idx] != 0) {
            tray[chair_idx].isDirty = true;
            chair_patient[chair_idx] = 0;
            neat_chair[chair_idx] = true;
            (*furious_patient)++;
        }
        return;
    }

    if (chair_patient[chair_idx] == 0) return;
    if (tray[chair_idx].count == 0)    return;
    if (tray[chair_idx].isDirty)       return;

    Patient *pat = &patientList->patients[chair_idx];

    for (int s = 0; s < pat->symptomCount; s++) {
        Symptom *sym = &pat->symptoms[s];
        if (sym->neat) continue;
        bool all_present = true;
        for (int t = 0; t < sym->toolCount; t++) {
            bool found = false;
            for (int p = 0; p < tray[chair_idx].count; p++)
                if (tray[chair_idx].tools[p] == sym->tools[t])
                    found = true;
            if (!found) { all_present = false; break; }
        }
        if (all_present) {
            sym->neat = true;
            sym->toolsUsed = sym->toolCount;
        }
    }

    pat->isTreated = isEntirelyCared(pat);
    tray[chair_idx].isDirty    = true;
    tray[chair_idx].patientIdx = chair_idx;
    P1->hasGloves = GLOVES_USED;

    if (pat->isTreated) {
        chair_patient[chair_idx] = 0;
        neat_chair[chair_idx]    = true;
        float ratio = (float)pat->patienceLeft / (float)pat->patienceMax;
        int payment;
        if      (ratio > 0.6f) { payment = 200; (*satisfied_patient)++;   }
        else if (ratio > 0.3f) { payment = 100; (*unsatisfied_patient)++; }
        else                   { payment = 50;  (*unsatisfied_patient)++; }
        P1->money += payment;
    }
}