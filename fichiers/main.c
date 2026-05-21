#include <stdio.h>
#include <stdlib.h> 
#include "display.h"
#include "save.h"

// les couleurs ANSI, pour rendre le terminal plus joli
#define ROUGE   "\033[31m" 
#define VERT    "\033[32m"
#define JAUNE   "\033[33m"
#define BLEU    "\033[34m"
#define RESET   "\033[0m"

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
void Help(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    printf(VERT "\n  +============== HELP - CAVITY-TASKFORCE ==============+\n\n" RESET);

    printf("  " JAUNE "OBJECTIVE:" RESET "\n");
    printf("    Manage a dental office. Treat patients\n");
    printf("    as fast as possible to maximize your earnings!\n\n");

    printf("  " JAUNE "HOW TO PLAY:" RESET "\n");
    printf("    1. A patient arrives and sits in a chair\n");
    printf("    2. Go get sterile gloves (Gl)\n");
    printf("    3. Pick up the required tools one by one\n");
    printf("    4. Drop them on the tray next to the patient\n");
    printf("    5. Once the tray is complete, treat the patient\n");
    printf("    6. Pick up the dirty tray and empty it in Ob (biohazard)\n");
    printf("    7. Your gloves are discarded at the same time\n\n");

    printf("  " JAUNE "IMPORTANT RULES:" RESET "\n");
    printf("    - Without gloves, the tool is CONTAMINATED -> recycling (Rc)\n");
    printf("    - With dirty gloves, the patient panics and leaves furious\n");
    printf("    - If all chairs are full and a patient leaves furious -> GAME OVER\n");
    printf("    - Each tool picked up costs money\n");
    printf("    - Satisfied patient: full payment\n");
    printf("    - Unhappy patient (long wait): half payment\n");
    printf("    - Furious patient: no payment\n");
    printf("    - If a patient arrives with a dirty tray: will NOT pay\n\n");

    printf("  " JAUNE "CONTROLS:" RESET "\n");
    printf("    Z, Q, S, D    -> Move\n");
    printf("    SPACE or E    -> Contextual action\n");
    printf("    X             -> Quit\n\n");

    printf("Press any key to return to the menu...\n");
    getch();
}

int main() {

    enum Menu {
        START,
        SAVE,
        LEADERBOARD,
        HELP,
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
            printf("4. Help\n");
            printf("5. Quit\n");
        }
        else if(position == SAVE){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save <-\n");
            printf("3. Leaderboard\n");
            printf("4. Help\n");
            printf("5. Quit\n");
        }
        else if(position == LEADERBOARD){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save\n");
            printf("3. Leaderboard <-\n");
            printf("4. Help\n");
            printf("5. Quit\n");
        }
        else if(position == HELP){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save\n");
            printf("3. Leaderboard\n");
            printf("4. Help <-\n");
            printf("5. Quit\n");
        }
        else if(position == QUIT){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save\n");
            printf("3. Leaderboard\n");
            printf("4. Help\n");
            printf("5. Quit <-\n");
        }

        choix = getch();

        if(choix == 's' && position < QUIT){
            position++;
        }
        else if(choix == 'z' && position > START){
            position--;
        }
        else if(choix == ' '){
            if(position == HELP){
                Help();
            } else {
                break;
            }
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