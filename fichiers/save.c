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
        state.chair_patient[i] = 0;  // Corrigé pour correspondre à display.c
        state.neat_chair[i] = 0;     // Corrigé pour correspondre à display.c
        state.tray[i] = (Tray){.count=0, .isDirty=false, .patientIdx=-1}; // Corrigé (tray au lieu de trays)
    }
    state.furious_patient  = 0;
    state.unsatisfied_patient = 0;
    state.satisfied_patient = 0;
    state.next_timer   = 50;
    state.next_patient = 1;          // CORRIGÉ ! Tu avais mis state.next_timer = 1; ici !

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

    char choice;

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

        choice = getch();

        if(choice == 's' && position < RETURN){
            position++;
        }
        else if(choice == 'z' && position > SAVE1){
            position--;
        }
        else if(choice == ' '){
            break;
        }
    }

    if (position == SAVE1 || position == SAVE2 || position == SAVE3) {
        int slot = position + 1; // SAVE1=0 → slot 1, etc.
        GameState s = loadGame(slot);
        
        // CORRIGÉ : L'ordre exact et les bons arguments de la fonction display !
        display(s.P1, s.patientList, s.next_patient, s.chair_patient, s.next_timer, s.neat_chair,
                s.tray, s.furious_patient, s.unsatisfied_patient, s.satisfied_patient);
                
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