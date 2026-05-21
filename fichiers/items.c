#include "items.h"

const char* getObjectSymbol(enum Object type) {
    switch (type) {
        case GLOVES:  return "[GLOVES]";
        case SYRINGE: return "[SYRINGE]";
        case COTTON:  return "[COTTON]";
        case CLAMP:   return "[CLAMP]";
        case DRILL:   return "[DRILL]";
        case MIRROR:  return "[MIRROR]";
        case PROBE:   return "[PROBE]";
        case SUCTION: return "[SUCTION]";
        default:      return "    ";
    }
}