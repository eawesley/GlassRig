//
// Created by Ethan Wesley on 30/11/2020.
//

#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>

extern struct content{
    int log_currentState;
    int log_nextState;
    uint16_t log_s_flags;
    uint16_t log_e_flags;
    char log_tOven[8];
    char log_tMandrel[8];
    char log_tGlass[8];
    char log_pVac[8];
    char log_location[16];
    char log_time[8];
}message;

void construct();
void transmit();

#endif //GLASSRIGCONTROL_LOGGING_H
