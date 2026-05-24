#include <stdio.h>
#include <stdbool.h> // avoir des bool
#include <stdlib.h>
#include <time.h>   // pour la fonction rand() et srand()
#include "map.h"
#include "display.h"
#include "items.h"
#include "patients.h"
#include "save.h"
#include "dentist.h"


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
// ------------------------------------------------------------ LES STRUCTURES D'AFFICHAGE ------------------------------------------------------------


typedef struct {
    int row;
    int col;
} DisplayBase;


const char* getDentistToolSymbol(Player P1) {
    
    static char buffer[64];
    if (P1.objectId == 0) return "-";

    const char* name = getObjectSymbol(P1.objectId);

    if (P1.objectInfected)
        sprintf(buffer, RED "%s infected" RESET, name);
    else
        sprintf(buffer, GREEN "%s" RESET, name);

    return buffer;
}








// ------------------------------------------------------------ PROCEDURE PRINCIPALE ------------------------------------------------------------
void display(Player P1, PatientList patientList, int next_patient, int chair_patient[4], int next_timer, bool neat_chair[4], Tray tray[4], int furious_patient, int unsatisfied_patient, int satisfied_patient) {
    int c = 0; // pour stocker les entrées du joueur
    /* ici c était un char car x arrête le programme, mais pour les touches directionnelles, 
    on a besoin d'un int pour contenir les codes spéciaux (ex: 1000 pour KEY_UP), d'où le changement de type de c en int
    */
        // Agrandir le terminal automatiquement ( sur Windows)   AYA te le met sur MAC
    #ifdef _WIN32
        system("mode con: cols=200 lines=50");
#endif



// ------------------------------------------------------------ POSITIONNEMENT DE LA GRILLE ------------------------------------------------------------
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

    grid[1][8].obj = TRAY; // Placer le plateau du dentiste à la position (1,8)
    grid[2][8].obj = TRAY; // Placer le plateau du dentiste à la position (2,8)
    grid[3][8].obj = TRAY; // Placer le plateau du dentiste à la position (3,8)
    grid[4][8].obj = TRAY; // Placer le plateau du dentiste à la position (4,8)






// ------------------------------------------------------------ BOUCLE PRINCIPALE ------------------------------------------------------------
    while (c != 'x') {
        for (int i = 0; i < 4; i++) {
            if (chair_patient[i] != 0) {
                int num = chair_patient[i] - 1;
                if (!patientList.patients[num].isTreated && patientList.patients[num].patienceLeft > 0){
                    patientList.patients[num].patienceLeft--;
                }
                if (!patientList.patients[num].isTreated && patientList.patients[num].patienceLeft == 0) {
                    treat_patient(i, chair_patient, neat_chair); // patient part furieux, sans paiement
                    furious_patient++;
                }
            }
        }


    /* 1. input (non bloquant) Z Q S D pour déplacer le joueur
    Le ifdef ne sert pas à grand chose ici, mais il est là pour montrer que la fonction kbhit() est différente selon le système d'exploitation
    */
    if (kbhit()) {
        
    c = getch_portable();
    if (furious_patient >= 3) {
            c = 'x'; // force la sortie du while
        }
    switch (c) {
    case 'z': if (P1.y > 0)            P1.y--; break;
    case 's': if (P1.y < M_HEIGHT - 1) P1.y++; break;
    case 'q': if (P1.x > 0)            P1.x--; break;
    case 'd': if (P1.x < M_WIDTH - 1)  P1.x++; break;
    case ' ':
        action_pick_up(&P1, grid, tray, chair_patient, &patientList);
        break;

    case 'e': {
    action_interact(&P1, grid, tray, chair_patient, &patientList,
                    neat_chair, &furious_patient, &unsatisfied_patient, &satisfied_patient);
        break;
    }
    }
    }

    
    // ------------------------------------------------------------ AFFICHAGE ------------------------------------------------------------


    update_patients_waiting_room(&next_patient, chair_patient, &next_timer, tray, &patientList);
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
    printf("----------------------------------------------\n");
    printf("|   |   |" GREEN "Gl" "\033[0m" " |" GREEN "Pr" "\033[0m" " |" GREEN "Co" "\033[0m" " |" GREEN "Mi" "\033[0m" " |   |   |   |   |   |\n");
    for (int i = 0; i < M_HEIGHT; i++) {
        printf("|%s |", (i == 1) ? GREEN "Su" RESET : (i == 2) ? GREEN "Sy" RESET : (i == 3) ? GREEN "Cl" RESET : (i == 4) ? GREEN "Dr" RESET : "  ");
        for (int j = 0; j < M_WIDTH; j++) {
            if (P1.x == j && P1.y == i) {
                printf("😷 |");
            } 
            else if (grid[i][j].obj == TRAY) {
                printf("🫙  |");
            }
            else {
                printf("   |");
            }

        }
        const char* patient_names[] = {"   ", "🤕", "🤒", "🤧", "🥶"};
        int chair_idx = i - 1;
        if (chair_idx >= 0 && chair_idx <= 3) {
            int p = chair_patient[chair_idx];
            if (p == 0)
                printf(" " YELLOW "-" RESET " |");
            else
             printf("" YELLOW "%s" RESET " |", patient_names[p]);
        } else {
    printf("   |");
        }

        printf("\n");

    }
    printf("|   |   |   |T1 |   |T2 |   |   |   |   |   |\n");
    printf("----------------------------------------------\n");

// ------------------------------------------------------------ AFFICHAGE DES INFOS ------------------------------------------------------------

printf("\e7");
DisplayBase display1 = {1, 50};
printf("\e[%d;%dH", display1.row++, display1.col);
printf("===== Office =====");

printf("\e[%d;%dH", display1.row++, display1.col);
printf("Gloves       : %s", P1.hasGloves == BAREHANDS ? RED "NO" RESET : GREEN "YES" RESET);

printf("\e[%d;%dH", display1.row++, display1.col);
printf("Gloves Used  : %s", P1.hasGloves == GLOVES_USED ? GREEN "YES" RESET : RED "NO" RESET);

printf("\e[%d;%dH", display1.row++, display1.col);
printf("Dentist      : %s", getDentistToolSymbol(P1));

printf("\e[%d;%dH", display1.row++, display1.col);
printf("Money        : %d", P1.money);

printf("\e[%d;%dH", display1.row++, display1.col);
printf("--- TRAIL ---");

for (int i = 0; i < 4; i++) {
    printf("\e[%d;%dH", display1.row++, display1.col);
    printf("Tray %d (%s):", i + 1,
        tray[i].isDirty ? RED "DIRTY" RESET : GREEN "CLEAN" RESET);
    if (tray[i].count == 0) {
        printf(" (empty)");
    } else {
        for (int t = 0; t < tray[i].count; t++) {
            printf(" %s", toolName(tray[i].tools[t]));
        }
    }
}

printf("\e8");

// ------------------------------------------------------------ AFFICHAGE DES PATIENTS ET SYMPTOMES ------------------------------------------------------------
DisplayBase display_patients = {15, 2};
printf("\e7");

printf("\e[%d;%dH", display_patients.row++, display_patients.col);
printf("===== Patients & Symptoms =====");

for (int i = 0; i < 4; i++) {
    if (chair_patient[i] == 0) continue;

    int num_patient = chair_patient[i] - 1;
    Patient *pat = &patientList.patients[num_patient];

    // Couleur selon la patience restante
    float ratio = (float)pat->patienceLeft / (float)pat->patienceMax;
    const char *color_patience = ratio > 0.6f ? GREEN : ratio > 0.3f ? YELLOW : RED;

    // Nom + patience + plateau
    printf("\e[%d;%dH", display_patients.row++, display_patients.col);
    printf(YELLOW "[ F%d ] %s" RESET " | Patience: %s%d/%d" RESET " | Tray: %s",
           i + 1, pat->name,
           color_patience, pat->patienceLeft, pat->patienceMax,
           tray[i].isDirty ? RED "DIRTY" RESET : GREEN "CLEAN" RESET);

    // Outils sur le plateau
    if (tray[i].count == 0) {
        printf(" (empty)");
    } else {
        for (int t = 0; t < tray[i].count; t++) {
            printf(" " GREEN "%s" RESET, toolName(tray[i].tools[t]));
        }
    }

    // Symptomes
    for (int s = 0; s < pat->symptomCount; s++) {
        Symptom *sym = &pat->symptoms[s];

        printf("\e[%d;%dH", display_patients.row++, display_patients.col);

        if (sym->neat)
            printf("  " GREEN "[OK] %s" RESET, sym->name);
        else
            printf("  " RED "[ ] %s" RESET, sym->name);

        printf(" -> ");

        for (int t = 0; t < sym->toolCount; t++) {
            ToolType tool = sym->tools[t];
            bool on_platform = false;
            for (int p = 0; p < tray[i].count; p++) {
                if (tray[i].tools[p] == tool) {
                    on_platform = true;
                    break;
                }
            }
            if (on_platform)
                printf(GREEN "%s " RESET, toolName(tool));
            else
                printf("%s ", toolName(tool));
        }
    }

    display_patients.row++;
}

printf("\e8");



// ------------------------------------------------------------ INFOS OUTILS ------------------------------------------------------------
DisplayBase display2 = {1, 110};
printf("\e7");

printf("\e[%d;%dH", display2.row++, display2.col);
printf("+============ TOOLS PRICES ============+");

// Tableau des tools et leurs prix
const char* tool_names[]  = {"Gloves ", "Probe ", "Mirror ", "Suction ", "Syringe ", "Clamp ", "Drill ", "Cotton "};
const int   tool_prices[]  = {2,       10,      15,       15,        20,        10,      10,      30}; // prix des tools dans l'ordre
// (remplace les prix par les vraies valeurs)

for (int z = 0; z < 8; z++) {
    printf("\e[%d;%dH", display2.row++, display2.col);
    printf("| %-12s :  " GREEN "%3d$" RESET "                   |", tool_names[z], tool_prices[z]);
}

printf("\e[%d;%dH", display2.row++, display2.col);
printf("+========================================+");

// ------------------------------------------------------------ SCORE ------------------------------------------------------------
printf("\e8");

DisplayBase display3 = {15, 110}; // a changer pour ne pas cacher les symptomes des patients dans la salle d'attente
printf("\e7");
printf("\e[%d;%dH", display3.row++, display3.col);
printf("+============ SCORE ============+");
printf("\e[%d;%dH", display3.row++, display3.col);
printf("| Money spent : " GREEN "%5d$" RESET "           |", P1.moneySpent);
printf("\e[%d;%dH", display3.row++, display3.col);
printf("| Patients furious : " RED "%5d" RESET "       |", furious_patient);
printf("\e[%d;%dH", display3.row++, display3.col);
printf("| Dissatisfied Patients : " GREEN "%5d" RESET "  |", unsatisfied_patient);
printf("\e[%d;%dH", display3.row++, display3.col);
printf("| Satisfied Patients : " GREEN "%5d" RESET "     |", satisfied_patient);
printf("\e[%d;%dH", display3.row++, display3.col);
printf("+===============================+");

printf("\e8");


    printf("\e7");
    DisplayBase display4 = {30, 110};
    printf("\e[%d;%dH", display4.row++, display4.col);
    printf("+============ QUIT ============+");
    printf("\e[%d;%dH", display4.row++, display4.col);
    printf("|       Press x to quit        |");
    printf("\e[%d;%dH", display4.row++, display4.col);
    printf("+==============================+");
    printf("\e8");
    


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


// ------------------------------------------------------------ GAME OVER ------------------------------------------------------------
    if (furious_patient >= 3) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf(RED "\n\n=== GAME OVER ===\n" RESET);
        printf("3 patients left furious!\n");
        printf("Final score: %d$\n\n", P1.moneySpent);
    }

// ------------------------------------------------------------ INFORMATION DU JEU ------------------------------------------------------------
    GameState gs;
    gs.P1             = P1;
    gs.patientList    = patientList;
    gs.furious_patient  = furious_patient;
    gs.unsatisfied_patient = unsatisfied_patient;
    gs.satisfied_patient = satisfied_patient;
    gs.next_timer   = next_timer;
    gs.next_patient = next_patient;
    for (int i = 0; i < 4; i++) {
        gs.chair_patient[i] = chair_patient[i];
        gs.neat_chair[i] = neat_chair[i];
        gs.tray[i]       = tray[i];
    }
    saveGame(gs);


}