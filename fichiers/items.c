#include "items.h"

const char* getObjectSymbol(enum Object type) {
    switch (type) {
        case GLOVES:  return "[GL]";
        case SYRINGE: return "[SY]";
        case COTTON:  return "[CO]";
        case CLAMP:   return "[CL]";
        case DRILL:   return "[DR]";
        case MIRROR:  return "[MI]";
        case PROBE:   return "[PR]";
        case SUCTION: return "[SU]";
        default:      return "    ";
    }
}