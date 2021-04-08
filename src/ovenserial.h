//
// Created by Ethan Wesley on 18/11/2020.
//

#ifndef OVENSERIAL_H
#define OVENSERIAL_H

#include "ModbusMaster.h"
#include "param.h"

extern ModbusMaster Oven;

class ovenserial {
private:
    static uint16_t const mbDEVICESTATUS = 153;
    static uint16_t const mbTEMPERATURE = 601;
    static uint16_t const mbCONTROLOP = 611;
    static uint16_t const mbSETPOINT = 401;
    static uint16_t const mbRAMP = 402;
public:
    static void UpdateTemperature(ModbusMaster &target);
    static void ControlOutput(ModbusMaster &target, PID command);
    static void SetPoint(ModbusMaster &target, uint16_t temp);
    static void SetRamp(ModbusMaster &target, uint16_t ramp);
    static void HeartBeat(ModbusMaster &target);
};

#endif //GLASSRIGCONTROL_OVENSERIAL_H
