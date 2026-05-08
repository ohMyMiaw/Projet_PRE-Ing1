#include "display.h"

#include <stdio.h>
#include <stdbool.h> // avoir des bool
#include "map.h"



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



int main() {
    char c = 0;
    /* ici c était un char car x arrête le programme, mais pour les touches directionnelles, 
    on a besoin d'un int pour contenir les codes spéciaux (ex: 1000 pour KEY_UP), d'où le changement de type de c en int
    */
   
    
    enum Object{
        GANTS,
        PENSEMENT,
        COTTON,
    };

    typedef struct {
        int x;
        int y;
        bool hasGants;
        bool GantsUsed;
        enum Object objetId;
    } Player;

    typedef struct {
        enum Object obj;
    } Cells;


    // Initialisation de la grille et du joueur
    Cells grid[M_HEIGHT][M_WIDTH]; // Exemple de grille 9x9
    grid[0][1].obj = GANTS; // Placer des gants à la position (0,1)
    grid[0][2].obj = PENSEMENT; // Placer un pensement à la position (0,2)
    grid[0][3].obj = COTTON; // Placer un coton à la position (0,3)

    Player P1 = {4, 4, false, false, 0}; // Position initiale du joueur


    printf("Programme en cours...\n");
    printf("Appuie sur 'x' pour quitter.\n");

    while (c != 'x') {

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
        case ' ': if (grid[P1.y][P1.x].obj == GANTS && !P1.hasGants) P1.hasGants = true, P1.GantsUsed = false; 
        else if (grid[P1.y][P1.x].obj == PENSEMENT && !P1.GantsUsed && P1.hasGants && P1.objetId == 0) P1.objetId = PENSEMENT;
        else if (grid[P1.y][P1.x].obj == COTTON && !P1.GantsUsed && P1.hasGants && P1.objetId == 0) P1.objetId = COTTON;
        
        break;
    }
    }


    
    // 2. affichage
    // 2.1 affichage grille
    printf("----------------------------------------------\n");
    printf("|   |   | G | P | C |   |   |   |   |   |   |\n");
    for (int i = 0; i < M_HEIGHT; i++) {
        printf("|   |");
        for (int j = 0; j < M_WIDTH; j++) {
            if (P1.x == j && P1.y == i) {
                printf(" P |");
            } 
            else {
            printf("   |");
            }
        }
        printf("   |");
        printf("\n");
    }
    printf("|   |   |   |   |   |   |   |   |   |   |   |\n");
    printf("----------------------------------------------\n");

    //2.2 affichage infos joueur
    printf("----------------------------------------------\n");
    printf("| Gants: %s | Utilisés: %s | Pensements: %d | Coton: %d |\n",
         P1.hasGants ? "Oui" : "Non", P1.GantsUsed ? "Oui" : "Non", P1.objetId == PENSEMENT ? 1 : 0, P1.objetId == COTTON ? 1 : 0);
    printf("----------------------------------------------\n");

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

    printf("Fin du programme.\n");

    return 0;
}