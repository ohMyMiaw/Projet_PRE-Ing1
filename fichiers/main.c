#include <stdio.h>
#include <stdlib.h> 


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
        QUIT
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
            printf("3. Quit\n");
        }
        else if(position == SAVE){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save <-\n");
            printf("3. Quit\n");
        }
        else if(position == QUIT){
            printf("=== Menu ===\n");
            printf("1. Start\n");
            printf("2. Save\n");
            printf("3. Quit <-\n");
        }

        choix = getch();

        if(choix == 's' && position < QUIT){
            position++;
        }
        else if(choix == 'z' && position > START){
            position--;
        }
        else if(choix == '\r' || choix == '\n'){
            break;
        }
    }

    if(position == START){
        printf("Start selected\n");
    }
    else if(position == SAVE){
        printf("Save selected\n");
    }
    else if(position == QUIT){
        return 0;
    }

    return 0;
}