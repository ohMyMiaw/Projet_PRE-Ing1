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

void saving(GameState gameState, int saveSlot) {
    char filename[20];
    sprintf(filename, "save%d.bin", saveSlot);
    FILE* f = fopen(filename, "wb");
    if (f == NULL) { printf("Error saving game\n"); return; }
    fwrite(&gameState, sizeof(GameState), 1, f);
    fclose(f);
    printf("Game saved successfully.\n");
}

GameState loadGame(int saveSlot) {
    GameState state;
    // valeurs par défaut
    PatientList pl;
    initPatients(&pl);
    state.P1 = (Player){4, 4, BAREHANDS, 0, NONE, false, 500, 0};
    state.patientList = pl;
    for (int i = 0; i < 4; i++) {
        state.chaise_patient[i] = 0;
        state.chaise_soignee[i] = 0;
        state.plateaux[i] = (Plateau){.count=0, .estSale=false, .patientIdx=-1};
    }
    state.patient_furieux  = 0;
    state.timer_prochain   = 50;
    state.prochain_patient = 1;

    char filename[20];
    sprintf(filename, "save%d.bin", saveSlot);
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        printf("No save found, starting new game.\n");
        #ifdef _WIN32
            Sleep(1000);
        #endif
        return state;
    }
    fread(&state, sizeof(GameState), 1, f);
    fclose(f);
    printf("Game loaded successfully.\n");
    return state;
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

    if (position == SAVE1 || position == SAVE2 || position == SAVE3) {
        int slot = position + 1; // SAVE1=0 → slot 1, etc.
        GameState s = loadGame(slot);
        display(s.P1,s.patientList, s.prochain_patient, s.chaise_patient, s.timer_prochain, s.chaise_soignee,
                s.plateaux, s.patient_furieux);
    } else if (position == RETURN) {
        main();
    }
}

void saveGame(GameState gameState) {
    // 1. gestion des entrées clavier
    int c;
    printf("Do you want to save the game? (1/2/3/x)\n");
    c = getch();
    if(c == '1'||c == '2'||c == '3') saving(gameState,c - '0');
    else{
        printf("Game save cancelled.\n");
    }


}