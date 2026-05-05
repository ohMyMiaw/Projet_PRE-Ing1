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

    enum Object{
        GANTS,
        PENSEMENT,
    };

    typedef struct {
        int x;
        int y;
    } Player;

    typedef struct {
        bool playerOn;
        enum Object obj;
    } Cells;

    Cells grid[M_HEIGHT][M_WIDTH]; // Exemple de grille 11x11
    Player P1 = {5, 5}; // Position initiale du joueur

    printf("Programme en cours...\n");
    printf("Appuie sur 'x' pour quitter.\n");

    while (c != 'x') {

    // 1. input (non bloquant)
    #ifdef _WIN32
        if (kbhit()) {
            c = getch();
        }
    #else
        if (kbhit()) {
            c = getch_portable();
        }
    #endif

    // 2. affichage
    printf("----------------------------------------------\n");
    for (int i = 0; i < M_HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < M_WIDTH; j++) {
            if (P1.x == j && P1.y == i) {
                printf(" P |");
            } 
            else {
            printf("   |");
            }
        }
        printf("\n");
    }
    printf("------------------------------------------\n");

    // 3. pause (contrôle vitesse)
    #ifdef _WIN32
        Sleep(1000);      // 1 s
    #else
        usleep(1000000);  // 1 s
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