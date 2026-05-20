#include "patients.h"
#include <stdio.h>
#include <string.h>

// -------------------------------------------------------
// Couleurs ANSI
// -------------------------------------------------------
#define RESET   "\033[0m"
#define ROUGE   "\033[31m"
#define JAUNE   "\033[33m"
#define VERT    "\033[32m"

// -------------------------------------------------------
// Noms des outils (pour affichage)
// -------------------------------------------------------
const char* toolName(ToolType t) {
    switch (t) {
        case TOOL_GLOVES:  return "Gants";
        case TOOL_DRILL:   return "Roulette";
        case TOOL_COTTON:  return "Coton";
        case TOOL_MIRROR:  return "Miroir";
        case TOOL_PROBE:   return "Sonde";
        case TOOL_SUCTION: return "Aspirateur";
        case TOOL_CLAMP:   return "Pince";
        case TOOL_SYRINGE: return "Seringue";
        default:           return "Inconnu";
    }
}

/* -------------------------------------------------------
 Initialisation des patients
   -------------------------------------------------------
*/
void initPatients(PatientList *list) {
    list->count = 0;

    // --- Patient 1 : carie ---
    Patient p1;
    strcpy(p1.name, "Arno Ta dac");
    p1.patienceMax  = 300;  // 300 secondes
    p1.patienceLeft = 300;
    p1.symptomCount = 1;

    strcpy(p1.symptoms[0].name, "Carie");
    strcpy(p1.symptoms[0].description, "Douleur intense dans une molaire");
    p1.symptoms[0].tools[0] = TOOL_GLOVES;
    p1.symptoms[0].tools[1] = TOOL_MIRROR;
    p1.symptoms[0].tools[2] = TOOL_PROBE;
    p1.symptoms[0].tools[3] = TOOL_DRILL;
    p1.symptoms[0].tools[4] = TOOL_COTTON;
    p1.symptoms[0].toolCount = 5;

    list->patients[list->count++] = p1;

    // --- Patient 2 : saignement + sensibilite ---
    Patient p2;
    strcpy(p2.name, "Aya Tikitaka");
    p2.patienceMax  = 150;  // très impatiente
    p2.patienceLeft = 150;
    p2.symptomCount = 2;

    strcpy(p2.symptoms[0].name, "Saignement des gencives");
    strcpy(p2.symptoms[0].description, "Les gencives saignent au moindre contact");
    p2.symptoms[0].tools[0] = TOOL_GLOVES;
    p2.symptoms[0].tools[1] = TOOL_SUCTION;
    p2.symptoms[0].tools[2] = TOOL_COTTON;
    p2.symptoms[0].toolCount = 3;

    strcpy(p2.symptoms[1].name, "Sensibilite dentaire");
    strcpy(p2.symptoms[1].description, "Douleur vive au contact du froid");
    p2.symptoms[1].tools[0] = TOOL_PROBE;
    p2.symptoms[1].tools[1] = TOOL_SYRINGE;
    p2.symptoms[1].toolCount = 2;

    list->patients[list->count++] = p2;

    // --- Patient 3 : abces ---
    Patient p3;
    strcpy(p3.name, "Fares Hamdouni");
    p3.patienceMax  = 450;  // très patient
    p3.patienceLeft = 450;
    p3.symptomCount = 1;

    strcpy(p3.symptoms[0].name, "Abces");
    strcpy(p3.symptoms[0].description, "Gonflement douloureux autour d'une dent");
    p3.symptoms[0].tools[0] = TOOL_GLOVES;
    p3.symptoms[0].tools[1] = TOOL_CLAMP;
    p3.symptoms[0].tools[2] = TOOL_SYRINGE;
    p3.symptoms[0].tools[3] = TOOL_SUCTION;
    p3.symptoms[0].toolCount = 4;

    list->patients[list->count++] = p3;
}

// -------------------------------------------------------
// Mise à jour du temps d'impatience (à appeler chaque seconde)
// -------------------------------------------------------
void updatePatience(PatientList *list) {
    for (int i = 0; i < list->count; i++) {
        if (list->patients[i].patienceLeft > 0)
            list->patients[i].patienceLeft--;
    }
}

// -------------------------------------------------------
// Affichage de la barre de patience avec couleur ANSI aaaaaaaaaaa j'en peux plus 
// vert (patient) → jaune → rouge (impatient)
// -------------------------------------------------------
void displayPatience(Patient *p, int ligne, int col) {
    float ratio = (float)p->patienceLeft / (float)p->patienceMax;

    const char *color;
    if (ratio > 0.6f)
        color = VERT;
    else if (ratio > 0.3f)
        color = JAUNE;
    else
        color = ROUGE;

    // Barre de progression (20 caractères)
    printf("\e7");

    // Titre de l'affichage
    printf("\e[0;0H"); // aller en haut à gauche pour éviter d'écraser les autres affichages
    int filled = (int)(ratio * 20);
    char bar[21];
    for (int i = 0; i < 20; i++)
        bar[i] = (i < filled) ? '#' : '-';
    bar[20] = '\0';

    printf("\e[%d;%dH", ligne, col);
    printf("%-15s | Patience: %s[%-20s]" RESET " %ds/%ds \n",
        p->name, color, bar, p->patienceLeft, p->patienceMax);
    printf("\e8");
}