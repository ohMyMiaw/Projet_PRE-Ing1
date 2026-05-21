#include <stdio.h>
#include <stdlib.h> 
#include "display.h"
#include "save.h"

#ifdef _WIN32

    #include <conio.h>

#else

    #include <termios.h>
    #include <unistd.h>

    char getch() {
        struct termios oldt, newt;
        char c;

        tcgetattr(STDIN_FILENO, &oldt);

        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        c = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return c;
    }

#endif

int main() {

    enum Menu {
        START,
        SAVE,
        LEADERBOARD,
        QUIT,
    };

    int position = START;

    char choix;

    while (1) {

        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        if(position == START){
            printf("=== Menu ===\n");
            printf("1. Start <-\n");
            printf("2. Save\n");
            printf("3. Leaderboard\n");
            printf("4. Quit\n");
        }
        else if(position == SAVE){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save <-\n");
            printf("3. Leaderboard\n");
            printf("4. Quit\n");
        }
        else if(position == LEADERBOARD){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save\n");
            printf("3. Leaderboard <-\n");
            printf("4. Quit\n");
        }
        else if(position == QUIT){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save\n");
            printf("3. Leaderboard\n");
            printf("4. Quit <-\n");
        }

        choix = getch();

        if(choix == 's' && position < QUIT){
            position++;
        }
        else if(choix == 'z' && position > START){
            position--;
        }
        else if(choix == ' '){
            break;
        }
    }

    if(position == START){
        Player P1 = {4, 4, false, false, 0, false, 500}; // Position initiale du joueur
        display(P1);
    }
    else if(position == SAVE){
        saveDisplay();
    }
    else if(position == LEADERBOARD){
        printf("Leaderboard selected\n");
    }
    else if(position == QUIT){
        return 0;
    }

    return 0;
}