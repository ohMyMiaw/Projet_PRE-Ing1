#ifndef ITEMS_H
#define ITEMS_H

enum Object {
    EMPTY=0,
    GLOVES,
    DRILL,
    COTTON,
    MIRROR,
    PROBE,
    SUCTION,
    CLAMP,
    SYRINGE,
    TRASH1,
    TRASH2,
    TRASH3,
    TRAY,
};

typedef struct {
    enum Object obj;
} Cells;

const char* getObjectSymbol(enum Object type);

#endif