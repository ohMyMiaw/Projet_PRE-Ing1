#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdbool.h>
#include "items.h"

enum GLOVES_STATUS {
    BAREHANDS,
    GLOVES_CLEAN,
    GLOVES_USED,
};

enum TRAIL{
    NONE,
    TRAIL_CLEAN,
    TRAIL_DIRTY,
};



typedef struct {
    int x;
    int y;
    enum GLOVES_STATUS hasGloves;
    enum Object objetId;
    enum TRAIL trail;
    bool objetInfected;
    int money;
} Player;


void display(Player P1);


#endif
