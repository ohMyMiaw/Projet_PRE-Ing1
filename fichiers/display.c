#include "display.h"
#include "items.h"
#include "patients.h"
#include "save.h"
#include <stdio.h>
#include <stdbool.h> // avoir des bool
#include <stdlib.h>
#include <time.h>   // pour la fonction rand() et srand()
#include "map.h"

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
        struct timeval tv = {0L, 0L};
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
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



// Remplace les déclarations AVANT le while par :
static int chaise_patient[4] = {0, 0, 0, 0};  // 0=vide, 1=P1, 2=P2, 3=P3, 4=P4
static int prochain_patient = 1;               // prochain patient à faire arriver (1 à 4)
static int timer_prochain   = 50;              // secondes avant l'arrivée du prochain patient
static bool chaise_soignee[4] = {false, false, false, false}; // true = dentiste a soigné

void update_patients_salle_attente() {
    // Si tous les patients sont arrivés, on arrête
    if (prochain_patient > 4) return;

    timer_prochain--;

    if (timer_prochain <= 0) {
        int chaise_idx = prochain_patient - 1; // P1→chaise 0, P2→chaise 1, etc.

        // Le patient s'assoit sur SA chaise
        chaise_patient[chaise_idx] = prochain_patient;

        prochain_patient++;

        // Attendre au moins 15 secondes avant le suivant (+ aléatoire)
        timer_prochain = 50 + rand() % 10;
    }
}


void display(Player P1) {
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

    grid[8][3].obj = TRASH1; // Placer une poubelle à la position (8,3)
    grid[8][5].obj = TRASH2; // Placer une autre poubelle à la position (8,5)



    while (c != 'x') {
        PatientList patientList; // initilisation de la liste de patients
        initPatients(&patientList);
        updatePatience(&patientList);  // chaque seconde
        displayPatience(&patientList.patients[0], 15, 1);  // ligne 15, col 1


    /* 1. input (non bloquant) Z Q S D pour déplacer le joueur
    Le ifdef ne sert pas à grand chose ici, mais il est là pour montrer que la fonction kbhit() est différente selon le système d'exploitation
    */
    if (kbhit()) {
    c = getch_portable();
    switch (c) {
        case 'z': if (P1.y > 0)            P1.y--; break;
        case 's': if (P1.y < M_HEIGHT - 1) P1.y++; break;
        case 'q': if (P1.x > 0)            P1.x--; break;
        case 'd': if (P1.x < M_WIDTH - 1)  P1.x++; break;
        case ' ': if (grid[P1.y][P1.x].obj == GLOVES && !P1.hasGloves) P1.hasGloves = true, P1.GlovesUsed = false, P1.money -= 10; 
        else if (grid[P1.y][P1.x].obj == PROBE && P1.objetId == 0) {P1.objetId = PROBE;P1.money -= 10;
            if(!P1.GlovesUsed && !P1.hasGloves)P1.objetInfected = true;}
        else if (grid[P1.y][P1.x].obj == CLAMP && P1.objetId == 0) {P1.objetId = CLAMP;P1.money -= 10;
            if(!P1.GlovesUsed && !P1.hasGloves)P1.objetInfected = true;}
        else if (grid[P1.y][P1.x].obj == SYRINGE && P1.objetId == 0) {P1.objetId = SYRINGE;P1.money -= 10;
            if(!P1.GlovesUsed && !P1.hasGloves)P1.objetInfected = true;}
        else if (grid[P1.y][P1.x].obj == MIRROR && P1.objetId == 0) {P1.objetId = MIRROR;P1.money -= 10;
            if(!P1.GlovesUsed && !P1.hasGloves)P1.objetInfected = true;}
        else if (grid[P1.y][P1.x].obj == SUCTION && P1.objetId == 0) {P1.objetId = SUCTION;P1.money -= 10;
            if(!P1.GlovesUsed && !P1.hasGloves)P1.objetInfected = true;}
        else if (grid[P1.y][P1.x].obj == DRILL && P1.objetId == 0) {P1.objetId = DRILL;P1.money -= 10;
            if(!P1.GlovesUsed && !P1.hasGloves)P1.objetInfected = true;}
        else if (grid[P1.y][P1.x].obj == COTTON && P1.objetId == 0) {P1.objetId = COTTON;P1.money -= 10;
            if(!P1.GlovesUsed && !P1.hasGloves)P1.objetInfected = true;}
        else if (grid[P1.y][P1.x].obj == TRASH1) P1.hasGloves = false, P1.GlovesUsed = false, P1.objetId = 0,P1.objetInfected = false;
        else if (grid[P1.y][P1.x].obj == TRASH2) P1.hasGloves = false, P1.GlovesUsed = false, P1.objetId = 0;

        
        break;
    }
    }


    
    // 2. affichage
    update_patients_salle_attente();

    // 2.1 affichage grille
    printf("----------------------------------------------\n");
    printf("|   |   | " GREEN "G" "\033[0m" " | " GREEN "P" "\033[0m" " | " GREEN "C" "\033[0m" " | " GREEN "M" "\033[0m" " |   |   |   |   |   |\n");
    for (int i = 0; i < M_HEIGHT; i++) {
        printf("| %s |", (i == 1) ? GREEN "S" RESET : (i == 2) ? GREEN "S" RESET : (i == 3) ? GREEN "C" RESET : (i == 4) ? GREEN "D" RESET : " ");
        for (int j = 0; j < M_WIDTH; j++) {
            if (P1.x == j && P1.y == i) {
                printf(" P |");
            } 
            else {
            printf("   |");
            }

        }
        const char* noms_patients[] = {"   ", "P1", "P2", "P3", "P4"};
        int chaise_idx = i - 1;
        if (chaise_idx >= 0 && chaise_idx <= 3) {
            int p = chaise_patient[chaise_idx];
            if (p == 0)
                printf(" " YELLOW "_" RESET " |");
            else
             printf(" " YELLOW "%s" RESET "|", noms_patients[p]);
        } else {
    printf("   |");
        }

        printf("\n");

    }
    printf("|   |   |   |   | T |   | T |   |   |   |   |\n");
    printf("----------------------------------------------\n");

    //2.2 affichage infos des outils (à droite de la grille)
    DisplayBase display1 = {2, 70};
printf("\e7");

// Titre de l'affichage
printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("===== Office =====");

// Liste des outils verticalement avec leur état (possédé ou non, utilisé ou non)
printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Gloves       : %s", P1.hasGloves ? GREEN "YES" RESET : RED "NO" RESET);

printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Gloves Used  : %s", P1.GlovesUsed ? GREEN "YES" RESET : RED "NO" RESET);

printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Dentist      : %s", getObjetSymbol(P1));

printf("\e[%d;%dH", display1.ligne++, display1.col);
printf("Money        : %d", P1.money);

printf("\e8");
// 2.3 affichage dans la cellule (à droite de la grille)


// -----------------------------------------------------------------------------------------------------------------
    DisplayBase display2 = {1, 110}; // position de base pour l'affichage dans la cellule
    printf("\e7"); // sauvegarde la position du curseur
    printf("\e[%d;%dH", display2.ligne++, display2.col);; // déplace le curseur à la position de la cellule

    printf("----------------------------------------------\n");
    printf("\e[%d;%dH", display2.ligne++, display2.col);;

    printf("|                                           |\n");
    printf("\e[%d;%dH", display2.ligne++, display2.col);

    for (int z = 0; z < M_HEIGHT; z++) {
        printf("|                                           |\n");
        printf("\e[%d;%dH", display2.ligne++, display2.col);
    }
    printf("|                                           |\n");
    printf("\e[%d;%dH", display2.ligne++, display2.col);
    printf("----------------------------------------------\n");
    printf("\e[%d;%dH", display2.ligne++, display2.col);

    printf("\e8"); // restaure la position du curseur
    

 

    printf("\n \n Pour quitter, appuiez sur X \n");

    


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



    saveGame(P1);

}
