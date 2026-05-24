#include "patients.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define RESET   "\033[0m"
#define ROUGE   "\033[31m"
#define JAUNE   "\033[33m"
#define VERT    "\033[32m"





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

  



static Symptom symptomePool[] = {
    // a changer les outils avec le plateau arno bb tu t'en occupe
    {"Carie",               "Douleur intense dans une molaire",          {TOOL_GLOVES, TOOL_MIRROR, TOOL_PROBE, TOOL_DRILL, TOOL_COTTON}, 5, false, 0},
    {"Saignement gencives", "Les gencives saignent au moindre contact",  {TOOL_GLOVES, TOOL_SUCTION, TOOL_COTTON},                        3, false, 0},
    {"Sensibilite dentaire","Douleur vive au contact du froid",          {TOOL_PROBE, TOOL_SYRINGE},                                      2, false, 0},
    {"Abces",               "Gonflement douloureux autour d'une dent",   {TOOL_GLOVES, TOOL_CLAMP, TOOL_SYRINGE, TOOL_SUCTION},           4, false, 0},
    {"Tartre",              "Depot calcifie sur les dents",              {TOOL_GLOVES, TOOL_PROBE, TOOL_SUCTION, TOOL_COTTON},            4, false, 0},
    {"Extraction",          "Dent a extraire d'urgence",                 {TOOL_GLOVES, TOOL_SYRINGE, TOOL_CLAMP},                         3, false, 0},
    {"Gingivite",           "Inflammation des gencives",                 {TOOL_GLOVES, TOOL_MIRROR, TOOL_SUCTION},                        3, false, 0},
    {"Fracture dentaire",   "Dent ebrechée suite a un choc",             {TOOL_GLOVES, TOOL_PROBE, TOOL_DRILL, TOOL_COTTON},              4, false, 0},
};

#define NB_SYMPTOMES_POOL 8

static void genererSymptomesAleatoires(Patient *p) {
    int indices[NB_SYMPTOMES_POOL];
    for (int i = 0; i < NB_SYMPTOMES_POOL; i++) indices[i] = i;

    // alogrithme de mélange de Fisher-Yates pour avoir des symptomes aléatoires à chaque lancement ( jsp ou je l'ai trouvé mais il est la )
    for (int i = NB_SYMPTOMES_POOL - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
    }

    int nb = 1 + rand() % 3; // 1 à 3 symptomes aléatoires par patient
    p->symptomCount = nb;
    for (int i = 0; i < nb; i++) {
        p->symptoms[i] = symptomePool[indices[i]];
        p->symptoms[i].soigne    = false; // remet à zéro les soins déjà appliqués
        p->symptoms[i].toolsUsed = 0; // remet à zéro les outils déjà utilisés
    }
}

void initPatients(PatientList *list) {
    srand((unsigned int)time(NULL));
    list->count = 4; // 1 patient par chaise dans la salle d'attente pour 4 chaises

    const char *noms_pool[] = {
        " Arno", " Aya", " Fares", " Marie", " Lucas", " Emma",
        " Noah", " Chloe", " Liam", " Lea", " Hugo", " Camille",
        " Louis", " Manon", " Gabriel", " Inès", " Raphael", " Jade",
        " Arthur", " Lucie"
    };
    int nb_noms = 20;
    int patience[4]; // l'impatience maximale de chaque patient (entre 800 et 4000, soit entre 13 et 67 secondes à la baisse d'une unité par seconde)
    for (int i = 0; i < 4; i++) {
        patience[i] = 8000 + rand() % 3000; // a changer ici pour les test ( plus rapide)
    }
    for (int i = 0; i < 4; i++) {
        // Prénom aléatoire sans doublon
        int idx;
        bool deja_pris;
        do {
            deja_pris = false;
            idx = rand() % nb_noms;
            for (int j = 0; j < i; j++) {
                if (strcmp(list->patients[j].name, noms_pool[idx]) == 0) {
                    deja_pris = true;
                    break;
                }
            }
        } while (deja_pris);

        strcpy(list->patients[i].name, noms_pool[idx]);
        list->patients[i].patienceMax  = patience[i];
        list->patients[i].patienceLeft = patience[i];
        list->patients[i].estSoigne    = false;
        genererSymptomesAleatoires(&list->patients[i]);
    }
}
// changement de la patience de chacun des patients de la salle d'attente ( le patient perd de la patience chaque seconde, et si il arrive à 0, c'est perdu )
void updatePatience(PatientList *list) {
    for (int i = 0; i < list->count; i++) {
        if (!list->patients[i].estSoigne && list->patients[i].patienceLeft > 0)
            list->patients[i].patienceLeft--;
    }
}

bool estEntierementSoigne(Patient *p) {
    for (int s = 0; s < p->symptomCount; s++)
        if (!p->symptoms[s].soigne) return false;
    return true;
} // regarde pour chacun des symptomes du patient si il est soigné, et si un seul ne l'est pas, alors le patient n'est pas entièrement soigné

bool appliquerOutil(Patient *p, ToolType outil) {
    for (int s = 0; s < p->symptomCount; s++) {
        Symptom *sym = &p->symptoms[s];
        if (sym->soigne) continue;

        int idx = sym->toolsUsed;
        if (idx < sym->toolCount && sym->tools[idx] == outil) {
            sym->toolsUsed++;
            if (sym->toolsUsed == sym->toolCount)
                sym->soigne = true;
            p->estSoigne = estEntierementSoigne(p);
            return true;
        }
        
    }
    return false;
}

void displayPatience(Patient *p, int ligne, int col) {
    float ratio = (float)p->patienceLeft / (float)p->patienceMax;
    const char *color = ratio > 0.6f ? VERT : ratio > 0.3f ? JAUNE : ROUGE;

    int filled = (int)(ratio * 20);
    char bar[21];
    for (int i = 0; i < 20; i++) bar[i] = (i < filled) ? '#' : '-';
    bar[20] = '\0';

    printf("\e7");
    printf("\e[%d;%dH", ligne, col);
    printf("%-15s | Patience: %s[%-20s]" RESET " %ds/%ds  Symptomes: ",
        p->name, color, bar, p->patienceLeft, p->patienceMax);
    for (int s = 0; s < p->symptomCount; s++) {
        if (p->symptoms[s].soigne) continue;
        printf("%s[%s]" RESET " -> ", p->symptoms[s].soigne ? VERT : ROUGE, p->symptoms[s].name);
        for (int t = 0; t < p->symptoms[s].toolCount; t++) {
            printf("%s ", toolName(p->symptoms[s].tools[t]));
        }
    }
    printf("\e8");
}