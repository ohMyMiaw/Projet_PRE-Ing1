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

void initPatients(PatientList *list) {
    srand((unsigned int)time(NULL));
    list->count = 4; // 1 patient par chaise dans la salle d'attente pour 4 chaises

    const char *names_pool[] = {
        " Arno", " Aya", " Fares", " Marie", " Lucas", " Emma",
        " Noah", " Chloe", " Liam", " Lea", " Hugo", " Camille",
        " Louis", " Manon", " Gabriel", " Inès", " Raphael", " Jade",
        " Arthur", " Lucie"
    };
    int nb_names = 20;
    int patience[4]; // l'impatience maximale de chaque patient (entre 800 et 4000, soit entre 13 et 67 secondes à la baisse d'une unité par seconde)
    for (int i = 0; i < 4; i++) {
        patience[i] = 8000 + rand() % 3000; // a changer ici pour les test ( plus rapide)
    }
    for (int i = 0; i < 4; i++) {
        // Prénom aléatoire sans doublon
        int idx;
        bool already_taken;
        do {
            already_taken = false;
            idx = rand() % nb_names;
            for (int j = 0; j < i; j++) {
                if (strcmp(list->patients[j].name, names_pool[idx]) == 0) {
                    already_taken = true;
                    break;
                }
            }
        } while (already_taken);

        strcpy(list->patients[i].name, names_pool[idx]);
        list->patients[i].patienceMax  = patience[i];
        list->patients[i].patienceLeft = patience[i];
        list->patients[i].isTreated    = false;
        generateRandomSymptoms(&list->patients[i]);
    }
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