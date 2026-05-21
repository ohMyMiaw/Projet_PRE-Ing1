#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdbool.h>
#include "items.h"


typedef struct {
     int x;
     int y;
    bool hasGloves;
    bool GlovesUsed;
    enum Object objetId;
    bool objetInfected;
    int money;
} Player;

void display(Player P1);

#endif