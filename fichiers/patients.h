
// les différentes structures utilisées pour les patients


#ifndef PATIENTS_H
#define PATIENTS_H
#include <stdbool.h>

// --- Constantes arbitraires ---
#define MAX_SYMPTOMS 3      // nombre max de symptomes par patient
#define MAX_TOOLS    5      // nombre max d'tools par symptome
#define MAX_PATIENTS 4      // nombre max de patients dans la salle d'attente
#define MAX_TOOLS_ON_TRAY 8 // nombre max d'tools sur le plateau du dentiste
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

 

// Tray du dentiste, où il pose les tools pour soigner le patient
typedef struct {
    ToolType tools[MAX_TOOLS_ON_TRAY];
    int      count;
    bool     isDirty;   // true après utilisation sur un patient
    int      patientIdx; // -1 si pas assigné
} Tray;


// --- Symptome --- les symptomes qui vont donner la liste des tools à utiliser pour le patien
typedef struct {
    char      name[50];
    char      description[234];
    ToolType  tools[MAX_TOOLS];
    int       toolCount;
    bool      neat;
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
    bool    isTreated;
} Patient;



// --- Liste de patients ---


typedef struct {
    Patient patients[MAX_PATIENTS];
    int     count;
} PatientList;

// --- Fonctions ---
void initPatients(PatientList *list);
void updatePatience(PatientList *list);
void displayPatience(Patient *p, int row, int col); // la barre de patiente est affichée à la row et colonne spécifiée
bool applyTool(Patient *p, ToolType outil);
bool isEntirelyCared(Patient *p);
const char* toolName(ToolType t);

#endif // PATIENTS_H
