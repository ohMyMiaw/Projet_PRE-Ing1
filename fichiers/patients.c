#include "patients.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define RESET   "\033[0m"
#define RED   "\033[31m"
#define YELLOW   "\033[33m"
#define GREEN    "\033[32m"





const char* toolName(ToolType t) {
    switch (t) {
        case TOOL_GLOVES:  return "Gloves";
        case TOOL_DRILL:   return "Drill";
        case TOOL_COTTON:  return "Cotton";
        case TOOL_MIRROR:  return "Mirror";
        case TOOL_PROBE:   return "Probe";
        case TOOL_SUCTION: return "Suction";
        case TOOL_CLAMP:   return "Clamp";
        case TOOL_SYRINGE: return "Syringe";
        default:           return "Unknown";
    } 
}

  



static Symptom symptomPool[] = {
    // a changer les tools avec le plateau arno bb tu t'en occupe
    {"Cavity",               "Severe pain in a molar",                    {TOOL_MIRROR, TOOL_PROBE, TOOL_DRILL, TOOL_COTTON}, 4, false, 0},
    {"Bleeding gums",        "Gums bleed at the slightest touch",         {TOOL_SUCTION, TOOL_COTTON},                        2, false, 0},
    {"Tooth sensitivity",    "Sharp pain when touching cold",             {TOOL_PROBE, TOOL_SYRINGE},                         2, false, 0},
    {"Abscess",              "Painful swelling around a tooth",           {TOOL_CLAMP, TOOL_SYRINGE, TOOL_SUCTION},           3, false, 0},
    {"Tartar",               "Calcified deposit on teeth",                {TOOL_PROBE, TOOL_SUCTION, TOOL_COTTON},            3, false, 0},
    {"Extraction",           "Tooth to be extracted urgently",            {TOOL_SYRINGE, TOOL_CLAMP},                         2, false, 0},
    {"Gingivitis",           "Gum inflammation",                          {TOOL_MIRROR, TOOL_SUCTION},                        2, false, 0},
    {"Dental fracture",      "Chipped tooth following an impact",         {TOOL_PROBE, TOOL_DRILL, TOOL_COTTON},              3, false, 0},
};

#define NB_SYMPTOMS_POOL 8

static void generateRandomSymptoms(Patient *p) {
    int index[NB_SYMPTOMS_POOL];
    for (int i = 0; i < NB_SYMPTOMS_POOL; i++) index[i] = i;

    // alogrithme de mélange de Fisher-Yates pour avoir des symptomes aléatoires à chaque lancement ( jsp ou je l'ai trouvé mais il est la )
    for (int i = NB_SYMPTOMS_POOL - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = index[i]; index[i] = index[j]; index[j] = tmp;
    }

    int nb = 1 + rand() % 3; // 1 à 3 symptomes aléatoires par patient
    p->symptomCount = nb;
    for (int i = 0; i < nb; i++) {
        p->symptoms[i] = symptomPool[index[i]];
        p->symptoms[i].neat    = false; // remet à zéro les soins déjà appliqués
        p->symptoms[i].toolsUsed = 0; // remet à zéro les tools déjà utilisés
    }
}

static const char *names_pool[] = {
    " Arno", " Aya", " Fares", " Marie", " Lucas", " Emma",
    " Noah", " Chloe", " Liam", " Lea", " Hugo", " Camille",
    " Louis", " Manon", " Gabriel", " Inès", " Raphael", " Jade",
    " Arthur", " Lucie"
};
#define NB_NAMES 20

// Crée un seul patient à l'index donné
void initPatient(Patient *p) {
    memset(p, 0, sizeof(Patient));
    strcpy(p->name, names_pool[rand() % NB_NAMES]);
    int pat = 8000 + rand() % 3000;
    p->patienceMax  = pat;
    p->patienceLeft = pat;
    p->isTreated    = false;
    generateRandomSymptoms(p);
}
// Appelé une seule fois au lancement : crée le premier patient sur la chaise 0

void initPatients(PatientList *list) {
    srand((unsigned int)time(NULL));
    memset(list, 0, sizeof(PatientList));
    // un seul patient au départ, sur la chaise 0
    initPatient(&list->patients[0]);
    list->count = 1;
}

// Appelé quand une chaise se libère
void spawnNewPatient(PatientList *list, int chair_idx) {
    // réutilise le slot de la chaise directement (0-3)
    initPatient(&list->patients[chair_idx]);
}


// changement de la patience de chacun des patients de la salle d'attente ( le patient perd de la patience chaque seconde, et si il arrive à 0, c'est perdu )
void updatePatience(PatientList *list) {
    for (int i = 0; i < list->count; i++) {
        if (!list->patients[i].isTreated && list->patients[i].patienceLeft > 0)
            list->patients[i].patienceLeft--;
    }
}

bool isEntirelyCared(Patient *p) {
    for (int s = 0; s < p->symptomCount; s++)
        if (!p->symptoms[s].neat) return false;
    return true;
} // regarde pour chacun des symptomes du patient si il est soigné, et si un seul ne l'est pas, alors le patient n'est pas entièrement soigné

bool applyTool(Patient *p, ToolType tool) {
    for (int s = 0; s < p->symptomCount; s++) {
        Symptom *sym = &p->symptoms[s];
        if (sym->neat) continue;

        int idx = sym->toolsUsed;
        if (idx < sym->toolCount && sym->tools[idx] == tool) {
            sym->toolsUsed++;
            if (sym->toolsUsed == sym->toolCount)
                sym->neat = true;
            p->isTreated = isEntirelyCared(p);
            return true;
        }
        
    }
    return false;
}

void displayPatience(Patient *p, int row, int col) {
    float ratio = (float)p->patienceLeft / (float)p->patienceMax;
    const char *color = ratio > 0.6f ? GREEN : ratio > 0.3f ? YELLOW : RED;

    int filled = (int)(ratio * 20);
    char bar[21];
    for (int i = 0; i < 20; i++) bar[i] = (i < filled) ? '#' : '-';
    bar[20] = '\0';

    printf("\e7");
    printf("\e[%d;%dH", row, col);
    printf("%-15s | Patience: %s[%-20s]" RESET " %ds/%ds  Symptoms: ",
        p->name, color, bar, p->patienceLeft, p->patienceMax);
    for (int s = 0; s < p->symptomCount; s++) {
        if (p->symptoms[s].neat) continue;
        printf("%s[%s]" RESET " -> ", p->symptoms[s].neat ? GREEN : RED, p->symptoms[s].name);
        for (int t = 0; t < p->symptoms[s].toolCount; t++) {
            printf("%s ", toolName(p->symptoms[s].tools[t]));
        }
    }
    printf("\e8");
}




void update_patients_waiting_room(int *next_patient, int chair_patient[4],
                                   int *next_timer, Tray tray[4], PatientList *patientList) {
    int free_chairs = 0;
    for (int i = 0; i < 4; i++) {
        if (chair_patient[i] == 0 && !tray[i].isDirty)
            free_chairs++;
    }
    if (free_chairs == 0) return;

    (*next_timer) -= 1 + free_chairs;

    if (*next_timer <= 0) {
        for (int i = 0; i < 4; i++) {
            if (chair_patient[i] == 0 && !tray[i].isDirty) {
                spawnNewPatient(patientList, i); // i = index chaise = index patient
                chair_patient[i] = i + 1;        // i+1 pour éviter 0 (= chaise vide)
                *next_timer = 150 + rand() % 20;
                break;
            }
        }
    }
}

void treat_patient(int chair_idx, int chair_patient[4], bool neat_chair[4]) {
    if (chair_idx >= 0 && chair_idx <= 3 && chair_patient[chair_idx] != 0) {
        chair_patient[chair_idx] = 0;
        neat_chair[chair_idx] = true;
    }
}