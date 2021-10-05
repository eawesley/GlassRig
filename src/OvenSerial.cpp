//
// Created by Ethan Wesley on 18/11/2020.
//

#include "OvenSerial.h"
#include "ModbusMaster.h"
#include "Parameters.h"
#include "Sensors.h"


ModbusMaster Oven;

void OvenSerial::UpdateTemperature(ModbusMaster &target) {
    uint16_t data[1];
    uint8_t result;
    result = target.readHoldingRegisters(mbTEMPERATURE, 1);
    if (result == target.ku8MBSuccess){
        data[0] = target.getResponseBuffer(0);
    }
    senseData.tOven = data[0]/10.;
}

void OvenSerial::ControlOutput(ModbusMaster &target, PID command) {
    uint8_t result;
    switch (command){
        case PID_ENABLE:
            target.setTransmitBuffer(0, 0);
            result = target.writeMultipleRegisters(mbCONTROLOP, 1);
            break;
        case PID_DISABLE:
            target.setTransmitBuffer(0, 1);
            result = target.writeMultipleRegisters(mbCONTROLOP, 1);
            break;
    }
}

void OvenSerial::SetPoint(ModbusMaster &target, uint16_t temp) {
    uint8_t result;
    target.setTransmitBuffer(0, temp*10);
    result = target.writeMultipleRegisters(mbSETPOINT, 1);
}

void OvenSerial::SetRamp(ModbusMaster &target, uint16_t ramp) {
    uint8_t result;
    target.setTransmitBuffer(0, ramp*10);
    result = target.writeMultipleRegisters(mbRAMP, 1);
}

void OvenSerial::HeartBeat(ModbusMaster &target){
    uint8_t result;
    result = target.readHoldingRegisters(mbDEVICESTATUS, 1);
    if (result == target.ku8MBSuccess){
        mbTimeoutCnt = 0;
    } else {
        mbTimeoutCnt++;
        if (mbTimeoutCnt >= mbTimeout){
            fError |= mErrOvenComms;
        }
    }
}
