#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include "items.h"
#include "patients.h"
#include "dentist.h"






void display(Player P1, PatientList patientList, int next_patient, int chair_patient[4], int next_timer, bool neat_chair[4], Tray tray[4], int furious_patient, int unsatisfied_patient, int satisfied_patient);
#endif
