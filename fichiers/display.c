#include <stdio.h>
#include <stdbool.h> // avoir des bool
#include <stdlib.h>
#include <time.h>   // pour la fonction rand() et srand()
#include "map.h"
#include "display.h"
#include "items.h"
#include "patients.h"
#include "save.h"


// les couleurs ANSI, pour rendre le terminal plus joli
#define RED      "\033[31m" 
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE     "\033[34m"
#define RESET    "\033[0m"

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>

    int kbhit() {
        struct termios oldt, newt;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        struct timeval tv = {0L, 0L};
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        int result = select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return result;
    }
#endif



int getch_portable() {
#ifdef _WIN32
    return getch();
#else
    struct termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);


    return ch;
#endif
}
// les différentes structures utilisées pour l'affichage


typedef struct {
    int ligne;
    int col;
} DisplayBase;


const char* getObjetSymbol(Player P1) {
    
    static char buffer[64];
    if (P1.objetId == 0) return "-";

    const char* nom = getObjectSymbol(P1.objetId);

    if (P1.objetInfected)
        sprintf(buffer, RED "%s infected" RESET, nom);
    else
        sprintf(buffer, GREEN "%s" RESET, nom);

    return buffer;
}

ToolType objetToTool(enum Object obj) {
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




void update_patients_salle_attente(int *prochain_patient, int chaise_patient[4], int *timer_prochain) {
    // Si tous les patients sont arrivés, on arrête
    if (*prochain_patient > 4) return;

    (*timer_prochain)--;

    if (*timer_prochain <= 0) {
        int chaise_idx = *prochain_patient - 1; // P1→chaise 0, P2→chaise 1, etc.

        // Le patient s'assoit sur SA chaise
        chaise_patient[chaise_idx] = *prochain_patient;

        (*prochain_patient)++;

        // Attendre au moins 15 secondes avant le suivant (+ aléatoire)
        *timer_prochain = 50 + rand() % 10;
    }
}

void soigner_patient(int chaise_idx, int chaise_patient[4], bool chaise_soignee[4]) {
    if (chaise_idx >= 0 && chaise_idx <= 3 && chaise_patient[chaise_idx] != 0) {
        chaise_patient[chaise_idx] = 0;
        chaise_soignee[chaise_idx] = true;
    }
}

void display(Player P1, PatientList patientList, int prochain_patient, int chaise_patient[4], int timer_prochain, bool chaise_soignee[4], Plateau plateaux[4], int patient_furieux) {
    char c = 0;
    /* ici c était un char car x arrête le programme, mais pour les touches directionnelles, 
    on a besoin d'un int pour contenir les codes spéciaux (ex: 1000 pour KEY_UP), d'où le changement de type de c en int
    */
        // Agrandir le terminal automatiquement ( sur Windows)   AYA te le met sur MAC
    #ifdef _WIN32
        system("mode con: cols=200 lines=50");
#endif



    // Initialisation de la grille et du joueur
    Cells grid[M_HEIGHT][M_WIDTH] = {0}; // Exemple de grille 9x9
    grid[0][1].obj = GLOVES; // Placer des gants à la position (0,1)
    grid[0][2].obj = PROBE; // Placer une sonde à la position (0,2)
    grid[0][3].obj = COTTON; // Placer un coton à la position (0,3)
    grid[0][4].obj = MIRROR; // Placer un miroir à la position (0,4)

    grid[1][0].obj = SUCTION; // Placer un extracteur de salive à la position (1,0)
    grid[2][0].obj = SYRINGE; // Placer un seringue à la position (2,0)
    grid[3][0].obj = CLAMP; // Placer une pince à la position (3,0)
    grid[4][0].obj = DRILL; // Placer une roulette à la position (4,0)

    grid[8][2].obj = TRASH1; // Placer une poubelle à la position (8,2)
    grid[8][4].obj = TRASH2; // Placer une autre poubelle à la position (8,4)
    grid[8][6].obj = TRASH3; // Placer une autre poubelle à la position (8,6)

    grid[1][8].obj = PLATEAU; // Placer le plateau du dentiste à la position (1,8)
    grid[2][8].obj = PLATEAU; // Placer le plateau du dentiste à la position (2,8)
    grid[3][8].obj = PLATEAU; // Placer le plateau du dentiste à la position (3,8)
    grid[4][8].obj = PLATEAU; // Placer le plateau du dentiste à la position (4,8)







    while (c != 'x') {
        for (int i = 0; i < 4; i++) {
            if (chaise_patient[i] != 0) {
                int num = chaise_patient[i] - 1;
                if (!patientList.patients[num].estSoigne && patientList.patients[num].patienceLeft > 0){
                    patientList.patients[num].patienceLeft--;
                }
                if (!patientList.patients[num].estSoigne && patientList.patients[num].patienceLeft == 0) {
                    soigner_patient(i, chaise_patient, chaise_soignee); // patient part furieux, sans paiement
                    patient_furieux++;
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            if (chaise_patient[i] != 0) {  // affiche seulement si la chaise est occupée
                int num_patient = chaise_patient[i] - 1;
                displayPatience(&patientList.patients[num_patient], 15 + i, 1);
            }
        }


    /* 1. input (non bloquant) Z Q S D pour déplacer le joueur
    Le ifdef ne sert pas à grand chose ici, mais il est là pour montrer que la fonction kbhit() est différente selon le système d'exploitation
    */
    if (kbhit()) {
        
    c = getch_portable();
    if (patient_furieux >= 3) {
            c = 'x'; // force la sortie du while
        }
    switch (c) {
    case 'z': if (P1.y > 0)            P1.y--; break;
    case 's': if (P1.y < M_HEIGHT - 1) P1.y++; break;
    case 'q': if (P1.x > 0)            P1.x--; break;
    case 'd': if (P1.x < M_WIDTH - 1)  P1.x++; break;
    case ' ':
        if (grid[P1.y][P1.x].obj == GLOVES && P1.hasGloves == BAREHANDS) { // ramasser les gants seulement si on n'en a pas déjà
            P1.hasGloves = GLOVES_CLEAN; P1.money -= 10;
        }
        else if (grid[P1.y][P1.x].obj == PROBE && P1.objetId == 0) { // ramasser la sonde seulement si on n'en a pas déjà
            P1.objetId = PROBE; P1.money -= 10;
            if (P1.hasGloves == BAREHANDS) P1.objetInfected = true;
        }
        else if (grid[P1.y][P1.x].obj == CLAMP && P1.objetId == 0) { // ramasser la pince seulement si on n'en a pas déjà
            P1.objetId = CLAMP; P1.money -= 10;
            if (P1.hasGloves == BAREHANDS) P1.objetInfected = true;
        }
        else if (grid[P1.y][P1.x].obj == SYRINGE && P1.objetId == 0) { // ramasser la seringue seulement si on n'en a pas déjà
            P1.objetId = SYRINGE; P1.money -= 10;
            if (P1.hasGloves == BAREHANDS) P1.objetInfected = true;
        }
        else if (grid[P1.y][P1.x].obj == MIRROR && P1.objetId == 0) { // ramasser le miroir seulement si on n'en a pas déjà
            P1.objetId = MIRROR; P1.money -= 10;
            if (P1.hasGloves == BAREHANDS) P1.objetInfected = true;
        }
        else if (grid[P1.y][P1.x].obj == SUCTION && P1.objetId == 0) { // ramasser l'extracteur de salive seulement si on n'en a pas déjà
            P1.objetId = SUCTION; P1.money -= 10;
            if (P1.hasGloves == BAREHANDS) P1.objetInfected = true;
        }
        else if (grid[P1.y][P1.x].obj == DRILL && P1.objetId == 0) { // ramasser la roulette seulement si on n'en a pas déjà
            P1.objetId = DRILL; P1.money -= 10;
            if (P1.hasGloves == BAREHANDS) P1.objetInfected = true;
        }
        else if (grid[P1.y][P1.x].obj == COTTON && P1.objetId == 0) { // ramasser le coton seulement si on n'en a pas déjà
            P1.objetId = COTTON; P1.money -= 10;
            if (P1.hasGloves == BAREHANDS) P1.objetInfected = true;
        }
       else if (grid[P1.y][P1.x].obj == TRASH1 && P1.objetId != 0) { // recyclage : outil propre ou contaminé
            P1.objetId = 0; P1.objetInfected = false;
        }
        else if (grid[P1.y][P1.x].obj == TRASH2) {
    // vider le plateau sale le plus proche (celui du patient soigné)
            for (int i = 0; i < 4; i++) {
                if (plateaux[i].estSale) {
                    plateaux[i].count      = 0;
                    plateaux[i].estSale    = false;
                    plateaux[i].patientIdx = -1;
                    P1.objetId             = 0;
                    P1.objetInfected       = false;
                    P1.hasGloves           = BAREHANDS;
                    break; // un seul à la fois
                }
            }
        }

        else if (grid[P1.y][P1.x].obj == PLATEAU && P1.objetId != 0) {
            int chaise_idx = P1.y - 1;
            if (chaise_idx >= 0 && chaise_idx <= 3 && !plateaux[chaise_idx].estSale) {
                if (P1.objetInfected) {
                    // outil contaminé interdit sur plateau stérile, ne rien faire
                }
                else if (plateaux[chaise_idx].count < MAX_TOOLS_ON_TRAY) {
                    ToolType t = objetToTool(P1.objetId);
                    if (t != TOOL_NONE && chaise_patient[chaise_idx] != 0) {
                        int num_patient = chaise_patient[chaise_idx] - 1;
                        Patient *pat = &patientList.patients[num_patient];
                        // Vérifier que l'outil est nécessaire pour ce patient
                        bool outil_necessaire = false;
                        for (int s = 0; s < pat->symptomCount; s++) {
                            for (int ti = 0; ti < pat->symptoms[s].toolCount; ti++) {
                                if (pat->symptoms[s].tools[ti] == t) {
                                    outil_necessaire = true;
                                    break;
                                }
                            }
                        }
                        // Vérifier que l'outil n'est pas déjà sur le plateau
                        bool deja_present = false;
                        for (int p = 0; p < plateaux[chaise_idx].count; p++) {
                            if (plateaux[chaise_idx].outils[p] == t) { deja_present = true; break; }
                        }
                        if (outil_necessaire && !deja_present) {
                            plateaux[chaise_idx].outils[plateaux[chaise_idx].count++] = t;
                            P1.objetId = 0;
                        }
                    }
                }
            }
        }
        
        break;

    case 'e': {
    int chaise_idx = P1.y - 1;
    
    if (P1.hasGloves != GLOVES_CLEAN) {
        if (chaise_idx >= 0 && chaise_idx <= 3 && chaise_patient[chaise_idx] != 0) {
            plateaux[chaise_idx].estSale = true;
            soigner_patient(chaise_idx, chaise_patient, chaise_soignee); // patient part furieux, sans paiement
            patient_furieux++;
        }
        break;
    }

    if (chaise_idx >= 0 && chaise_idx <= 3 &&
        chaise_patient[chaise_idx] != 0 &&
        plateaux[chaise_idx].count > 0 &&
        !plateaux[chaise_idx].estSale) {
 
        int num_patient = chaise_patient[chaise_idx] - 1;
        Patient *pat = &patientList.patients[num_patient];
 
        for (int s = 0; s < pat->symptomCount; s++) {
            Symptom *sym = &pat->symptoms[s];
 
            if (sym->soigne)
                continue;
 
            bool tous_presents = true;
 
            for (int t = 0; t < sym->toolCount; t++) {
                bool trouve = false;
 
                for (int p = 0; p < plateaux[chaise_idx].count; p++) {
                    if (plateaux[chaise_idx].outils[p] == sym->tools[t]) {
                        trouve = true;
                        break;
                    }
                }
 
                if (!trouve) {
                    tous_presents = false;
                    break;
                }
            }
 
            if (tous_presents) {
                sym->soigne = true;
                sym->toolsUsed = sym->toolCount;
            }
        }
 
        pat->estSoigne = estEntierementSoigne(pat);
        plateaux[chaise_idx].estSale = true;
        plateaux[chaise_idx].patientIdx = num_patient;
        P1.hasGloves = GLOVES_USED; // gants deviennent sales après le soin
 
        if (pat->estSoigne) {
            soigner_patient(chaise_idx, chaise_patient, chaise_soignee);
        }
    }
    break;
    }
    }
    }

    
    // 2. affichage
    update_patients_salle_attente(&prochain_patient, chaise_patient, &timer_prochain);

    // 2.1 affichage grille
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
    printf("----------------------------------------------\n");
    printf("|   |   | " GREEN "G" "\033[0m" " | " GREEN "P" "\033[0m" " | " GREEN "C" "\033[0m" " | " GREEN "M" "\033[0m" " |   |   |   |   |   |\n");
    for (int i = 0; i < M_HEIGHT; i++) {
        printf("| %s |", (i == 1) ? GREEN "S" RESET : (i == 2) ? GREEN "S" RESET : (i == 3) ? GREEN "C" RESET : (i == 4) ? GREEN "D" RESET : " ");
        for (int j = 0; j < M_WIDTH; j++) {
            if (P1.x == j && P1.y == i) {
                printf("😷 |");
            } 
            else if (grid[i][j].obj == PLATEAU) {
                printf("🫙  |");
            }
            else {
                printf("   |");
            }

        }
        const char* noms_patients[] = {"   ", "🤕", "🤒", "🤧", "🥶"};
        int chaise_idx = i - 1;
        if (chaise_idx >= 0 && chaise_idx <= 3) {
            int p = chaise_patient[chaise_idx];
            if (p == 0)
                printf(" " YELLOW "-" RESET " |");
            else
             printf("" YELLOW "%s" RESET " |", noms_patients[p]);
        } else {
    printf("   |");
        }

        printf("\n");

    }
    printf("|   |   |   |T1 |   |T2 |   |T3 |   |   |   |\n");
    printf("----------------------------------------------\n");

    //2.2 affichage infos des outils (à droite de la grille)
    DisplayBase display1 = {2, 70};
printf("\e7");

// Titre de l'affichage
printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("===== Office =====");

// Liste des outils verticalement avec leur état (possédé ou non, utilisé ou non)
printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Gloves       : %s", P1.hasGloves == BAREHANDS ? RED "NO" RESET : GREEN "YES" RESET);

printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Gloves Used  : %s", P1.hasGloves == GLOVES_USED ? GREEN "YES" RESET : RED "NO" RESET);

printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Dentist      : %s", getObjetSymbol(P1));

printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Money        : %d", P1.money);

printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("--- Plateau ---");

for (int i = 0; i < 4; i++) {
    printf("\e[%d;%dH", display1.ligne++, display1.col);
    printf("Plateau %d (%s):", i + 1,
        plateaux[i].estSale ? RED "SALE" RESET : GREEN "PROPRE" RESET);
    if (plateaux[i].count == 0) {
        printf(" (vide)");
    } else {
        for (int t = 0; t < plateaux[i].count; t++) {
            printf(" %s", toolName(plateaux[i].outils[t]));
        }
    }
}

printf("\e8");
// 2.3 affichage dans la cellule (à droite de la grille)

// 2.3 affichage des symptomes de chaque patient avec outils colorés
DisplayBase display_patients = {20, 2};
printf("\e7");

printf("\e[%d;%dH", display_patients.ligne++, display_patients.col);
printf("===== Patients & Symptomes =====");

for (int i = 0; i < 4; i++) {
    if (chaise_patient[i] == 0) continue;

    int num_patient = chaise_patient[i] - 1;
    Patient *pat = &patientList.patients[num_patient];

    printf("\e[%d;%dH", display_patients.ligne++, display_patients.col);
    printf(YELLOW "[ Fauteuil %d ] %s" RESET, i + 1, pat->name);

    for (int s = 0; s < pat->symptomCount; s++) {
        Symptom *sym = &pat->symptoms[s];

        printf("\e[%d;%dH", display_patients.ligne++, display_patients.col);

        if (sym->soigne)
            printf("  " GREEN "[OK] %s" RESET, sym->name);
        else
            printf("  " RED "[ ] %s" RESET, sym->name);

        printf(" -> ");

        for (int t = 0; t < sym->toolCount; t++) {
            ToolType outil = sym->tools[t];

            bool sur_plateau = false;
            for (int p = 0; p < plateaux[i].count; p++) {
                if (plateaux[i].outils[p] == outil) {
                    sur_plateau = true;
                    break;
                }
            }

            if (sur_plateau)
                printf(GREEN "%s " RESET, toolName(outil));
            else
                printf("%s ", toolName(outil));
        }
    }

    display_patients.ligne++;
}

printf("\e8");


// -----------------------------------------------------------------------------------------------------------------
DisplayBase display2 = {1, 110};
printf("\e7");

printf("\e[%d;%dH", display2.ligne++, display2.col);
printf("+============ PRIX DES OUTILS ============+");

// Tableau des outils et leurs prix
const char* noms_outils[]  = {"Gloves ", "Probe ", "Mirror ", "Suction ", "Syringe ", "Clamp ", "Drill ", "Cotton "};
const int   prix_outils[]  = {10,       10,      10,       10,        10,        10,      10,      10}; // prix des outils dans l'ordre
// (remplace les prix par les vraies valeurs)

for (int z = 0; z < 8; z++) {
    printf("\e[%d;%dH", display2.ligne++, display2.col);
    printf("| %-12s :  " GREEN "%3d$" RESET "                   |", noms_outils[z], prix_outils[z]);
}

printf("\e[%d;%dH", display2.ligne++, display2.col);
printf("+========================================+");

printf("\e8");

DisplayBase display3 = {15, 110}; // a changer pour ne pas cacher les symptomes des patients dans la salle d'attente
printf("\e7");
printf("\e[%d;%dH", display3.ligne++, display3.col);
printf("+============ SCORE ============+");
printf("\e[%d;%dH", display3.ligne++, display3.col);
printf("| Argent : " GREEN "%5d$" RESET "           |", P1.money );
printf("\e[%d;%dH", display3.ligne++, display3.col);
printf("| Patients furieux : " RED "%5d" RESET "       |", patient_furieux);
printf("\e[%d;%dH", display3.ligne++, display3.col);
printf("+===============================+");

    

 

    printf("\n \n \n \n \n \n Press x to quit \n");

    


    // 3. pause (contrôle vitesse)
    #ifdef _WIN32
        Sleep(200);      // 0.2 s
    #else
        usleep(200000);  // 0.2 s
    #endif

    // 3. efface le terminal
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    }


    // Affichage game over
    if (patient_furieux >= 3) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf(RED "\n\n=== GAME OVER ===\n" RESET);
        printf("3 patients sont partis furieux !\n");
        printf("Score final : %d$\n\n", P1.money);
    }


    GameState gs;
    gs.P1             = P1;
    gs.patientList    = patientList;
    gs.patient_furieux  = patient_furieux;
    gs.timer_prochain   = timer_prochain;
    gs.prochain_patient = prochain_patient;
    for (int i = 0; i < 4; i++) {
        gs.chaise_patient[i] = chaise_patient[i];
        gs.chaise_soignee[i] = chaise_soignee[i];
        gs.plateaux[i]       = plateaux[i];
    }
    saveGame(gs);


}