#include <stdio.h>
#include <stdlib.h> 
#include "save.h"
#include "main.h"

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

void saveDisplay() {
    enum Menu {
        SAVE1,
        SAVE2,
        SAVE3,
        RETURN,
    };

    int position = SAVE1;

    char choix;

    while (1) {

        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        if(position == SAVE1){
            printf("=== Menu ===\n");
            printf("1. Save 1 <-\n");
            printf("2. Save 2\n");
            printf("3. Save 3\n");
            printf("4. Return\n");
        }
        else if(position == SAVE2){
            printf("=== Menu ===\n");
            printf("1. Save 1\n");
            printf("2. Save 2 <-\n");
            printf("3. Save 3\n");
            printf("4. Return\n");
        }
        else if(position == SAVE3){
            printf("=== Menu ===\n");
            printf("1. Save 1\n");
            printf("2. Save 2\n");
            printf("3. Save 3 <-\n");
            printf("4. Return\n");
        }
        else if(position == RETURN){
            printf("=== Menu ===\n");
            printf("1. Save 1\n");
            printf("2. Save 2\n");
            printf("3. Save 3\n");
            printf("4. Return <-\n");
        }

        choix = getch();

        if(choix == 's' && position < RETURN){
            position++;
        }
        else if(choix == 'z' && position > SAVE1){
            position--;
        }
        else if(choix == ' '){
            break;
        }
    }

    if(position == SAVE1){
        printf("Save 1 selected\n");
    }
    else if(position == SAVE2){
        printf("Save 2 selected\n");
    }
    else if(position == SAVE3){
        printf("Save 3 selected\n");
    }
    else if(position == RETURN){
        main();
    }


}