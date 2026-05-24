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
    TRAY,
}; // les différents objets que le joueur peut ramasser ou utiliser, à placer sur la map pour les différencier des cases vides

typedef struct {
    enum Object obj;
} Cells; // les cellules de la grille

const char* getObjectSymbol(enum Object type);

#endif