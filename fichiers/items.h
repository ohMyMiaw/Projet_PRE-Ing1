#ifndef ITEMS_H
#define ITEMS_H

enum Object {
    GLOVES,
    DRILL,
    COTTON,
    MIRROR,
    PROBE,
    SUCTION,
    CLAMP,
    SYRINGE,
};

typedef struct {
    enum Object obj;
} Cells;

const char* getObjectSymbol(enum Object type);

#endif