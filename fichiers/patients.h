
// les différentes structures utilisées pour les patients


#ifndef PATIENTS_H
#define PATIENTS_H
#include <stdbool.h>

// --- Constantes arbitraires ---
#define MAX_SYMPTOMS 3      // nombre max de symptomes par patient
#define MAX_TOOLS    5      // nombre max d'outils par symptome
#define MAX_PATIENTS 4      // nombre max de patients dans la salle d'attente

// --- Outils disponibles ---
typedef enum {
    TOOL_GLOVES,
    TOOL_DRILL,
    TOOL_COTTON,
    TOOL_MIRROR,
    TOOL_PROBE,
    TOOL_SUCTION,
    TOOL_CLAMP,
    TOOL_SYRINGE,
    TOOL_NONE = -1
} ToolType;

// --- Symptome --- les symptomes qui vont donner la liste des outils à utiliser pour le patien
typedef struct {
    char      name[50];
    char      description[234];
    ToolType  tools[MAX_TOOLS];
    int       toolCount;
    bool      soigne;
    int       toolsUsed;
} Symptom;

/* --- Patient ---

Chaque patient a une liste d'un ou plusieurs symptomes  et un comportement (patient / impatient) 
qui va influencer la difficulté du niveau -> le temps d'attente pour le patient impatient est plus court que pour le patient patient

*/ 


typedef struct {
    char    name[50];
    Symptom symptoms[MAX_SYMPTOMS];
    int     symptomCount;
    int     patienceMax;
    int     patienceLeft;
    bool    estSoigne;
} Patient;



// --- Liste de patients ---


typedef struct {
    Patient patients[MAX_PATIENTS];
    int     count;
} PatientList;

// --- Fonctions ---
void initPatients(PatientList *list);
void updatePatience(PatientList *list);
void displayPatience(Patient *p, int ligne, int col); // la barre de patiente est affichée à la ligne et colonne spécifiée
bool appliquerOutil(Patient *p, ToolType outil);
bool estEntierementSoigne(Patient *p);
const char* toolName(ToolType t);

#endif // PATIENTS_H
