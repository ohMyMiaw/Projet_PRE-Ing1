#include <stdio.h>
#include <stdlib.h> 
#include "save.h"
#include "display.h"
#include "main.h"

#ifdef _WIN32

    #include <conio.h>
    #include <windows.h>

#else

    #include <termios.h>
    #include <unistd.h>

    //char getch() {
    //    struct termios oldt, newt;
    //    char c;

    //    tcgetattr(STDIN_FILENO, &oldt);

    //    newt = oldt;
    //    newt.c_lflag &= ~(ICANON | ECHO);

    //    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    //    c = getchar();

    //    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    //    return c;
    //} prcq c'est deja dans main.c

#endif

void saving(Player P1, int saveSlot) {
    char filename[20];
    sprintf(filename, "save%d.bin", saveSlot);
    FILE* f = fopen(filename, "wb");
    if (f == NULL) { printf("Error saving game\n"); return; }
    fwrite(&P1, sizeof(Player), 1, f);
    fclose(f);
    printf("Game saved successfully.\n");
}


Player loadGame(int saveSlot) {
    Player P1;
    char filename[20];
    sprintf(filename, "save%d.bin", saveSlot);
    FILE* f = fopen(filename, "rb");
    if (f == NULL) { 
        printf("No save found, starting new game.\n");
        #ifdef _WIN32
            Sleep(8000);
        #endif
        Player P1 = {4, 4, BAREHANDS, 0, NONE, false, 500};
        return P1; 
    }
    fread(&P1, sizeof(Player), 1, f);
    fclose(f);
    printf("Game loaded successfully.\n");
    return P1;
}


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
        display(loadGame(1));
    }
    else if(position == SAVE2){
        display(loadGame(2));
    }
    else if(position == SAVE3){
        display(loadGame(3));
    }
    else if(position == RETURN){
        main();
    }
}

void saveGame(Player P1) {
    // 1. gestion des entrées clavier
    int c;
    printf("Do you want to save the game? (1/2/3/x)\n");
    c = getch();
    if(c == '1'||c == '2'||c == '3') saving(P1,c - '0');
    else{
        printf("Game save cancelled.\n");
    }


}