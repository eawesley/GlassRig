//
// Created by Ethan Wesley on 19/11/2020.
//

#include "Transition.h"
#include "pins.h"
#include "Motors.h"
#include "Parameters.h"
#include "OvenSerial.h"
#include "Sensors.h"
#include <Arduino.h>

void transA(){
    // ZERO->SET Homing routine to bottom limit switch, complete when at Location = 0;
    if (!bFineHomeStop) {
        if (digitalRead(LIMIT_BOT) && !bHomeHalt) {                                                    // limit is active high
            motor::OvenControl(START, UP,OVENHOMESPEEDCOARSE);
            if (sensorFailTimer == 0){
                sensorFailTimer = millis();
            }
            if (millis() - sensorFailTimer > sensorFailDelay){
                fError |= mErrEndStop;
                sensorFailTimer = 0;
            }
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDCOARSE);
            bHomeHalt = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            bFineHomeStop = true;
            bHomeHalt = false;
        }
    } else if (bFineHomeStop){
        if (homeTimer == 0) {
            homeTimer = millis();
        }
        if ((millis() - homeTimer < 1000) && !bHomeHalt) {                                                  // limit is active high
            motor::OvenControl(START, UP, OVENHOMESPEEDCOARSE);
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDFINE);
            bHomeHalt = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            homeTimer = 0;
            bFineHomeStop = false;
            bHomeHalt = false;
            fState |= (mHomed | mLowLimit);
            digitalWrite(LTGREEN, HIGH);
        }
    }
}

void transB(){
    // SET->HEATING Raise to "ready position", set oven temperature and ramp rates, begin heating
    if (Locationmm <= OVENREADYPOS){  // convert location to mm
        fState &= ~(mHomed | mLowLimit);
        motor::OvenControl(START, UP, OVENHOMESPEEDCOARSE);
    }else{
        motor::OvenControl(STOP);
        if(!DRYRUN || !DEBUG) {
            OvenSerial::SetPoint(Oven, OVENSETPOINT);
            OvenSerial::SetRamp(Oven, OVENRAMPRATE);
            OvenSerial::ControlOutput(Oven, PID_ENABLE);
        }
        fState |= (mHeating);
    }
}

void transC(){
    // HEATING->READY At temperature, set flags for ready state
    if(DRYRUN || DEBUG) {
        fState &= ~(mHeating);
        fState |= (mAtTemp | mBotOfRun);
            digitalWrite(LTGREEN, HIGH);
    } else {
        if (senseData.tOven >= OVENSETPOINT) {
            fState &= ~(mHeating);
            fState |= (mAtTemp | mBotOfRun);
            digitalWrite(LTGREEN, HIGH);
        } else if (senseData.tOven <= OVENSETPOINT - UNDERTEMPTHRESH) {
            fState &= ~(mAtTemp | mBotOfRun);
            fState |= (mHeating);
            digitalWrite(LTGREEN, LOW);
        }
    }
}

void transD(){
    // HEATING->COLD Stop button pressed, turn off oven, lower to ready position
    OvenSerial::ControlOutput(Oven, PID_DISABLE);
    fState &= ~(mHeating | mAtTemp | mBotOfRun);
    if (Locationmm > OVENREADYPOS/2){
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
    } else {
        motor::OvenControl(STOP);
    }
}

void transE(){
    // READY->RUNNING Start glass rotation, delay, start oven at climb speed

    if(!DRYRUN && !DEBUG) {
        motor::GlassControl(START, UP, GLASSROTSPEED);
        digitalWrite(SOL3_PIN, HIGH); //TODO move to configuration definition
        digitalWrite(VAC_RELAY_PIN, HIGH);
    }
    if(delayTimer == 0){
        delayTimer = millis();
    }
    if(millis()-delayTimer >= longDelay) {
        if (Locationmm <= OVENTORPOS) {
            motor::OvenControl(START, UP, OVENRUNSPEED);
            fState &= ~(mBotOfRun);
        }
        delayTimer = 0;
    }
}

void transF(){
    // READY->COLD Stop button pressed, turn off oven, lower to 150mm
    OvenSerial::ControlOutput(Oven, PID_DISABLE);
    OvenSerial::SetPoint(Oven, 0);
    fState &= ~(mAtTemp | mBotOfRun);
    if (Locationmm >= OVENREADYPOS/2){
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
    } else {
        motor::OvenControl(STOP);
    }
}

void transG(){
    // READY->HEATING Temperature change, return to heating state
}

void transH(){
    // RUNNING->COMPLETE Oven at top of run, set flags for complete
    if(Locationmm >= OVENTORPOS){
        motor::OvenControl(STOP);
        fState |= (mTopOfRun);
    }
}

void transI(){
    // RUNNING->DROP Stop button pressed, turn off oven, start lowing oven at drop speed
    if(Locationmm >= OVENREADYPOS/2){
        OvenSerial::ControlOutput(Oven, PID_DISABLE);
        OvenSerial::SetPoint(Oven, 0);
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
        motor::GlassControl(STOP);
        fState &= ~(mAtTemp);
    }
}

void transJ(){
    // COMPLETE->DROP Oven is lowered after completed moulding run
    if(Locationmm >= OVENREADYPOS/2){
        OvenSerial::ControlOutput(Oven, PID_DISABLE);
        OvenSerial::SetPoint(Oven, 0);
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
        fState &= ~(mTopOfRun | mAtTemp);
    }
}

void transK() {
    // DROP->COLD Oven is lowered to ready position
    if(Locationmm <= OVENREADYPOS/2) {
        motor::OvenControl(STOP);
        motor::GlassControl(STOP);
    }
}

void transL(){
    // COLD->SET Homing routine to bottom limit switch, complete when at Location = 0;
    if (!bFineHomeStop) {
        if (digitalRead(LIMIT_BOT) && !bHomeHalt) {                                                    // limit is active high
            motor::OvenControl(START, UP,OVENHOMESPEEDCOARSE);
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDCOARSE);
            bHomeHalt = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            bFineHomeStop = true;
            bHomeHalt = false;
        }
    } else if (bFineHomeStop){
        if (homeTimer == 0) {
            homeTimer = millis();
        }
        if ((millis() - homeTimer < 1000) && !bHomeHalt) {                                                  // limit is active high
            motor::OvenControl(START, UP, OVENHOMESPEEDCOARSE);
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDFINE);
            bHomeHalt = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            homeTimer = 0;
            bFineHomeStop = false;
            bHomeHalt = false;
            fState |= (mHomed | mLowLimit);
            digitalWrite(LTGREEN, HIGH);
        }
    }
}

void transFault() {
    fState = mFaultState;
    OvenSerial::ControlOutput(Oven, PID_DISABLE);
    OvenSerial::SetPoint(Oven, 0);
    motor::GlassControl(STOP);
    digitalWrite(VAC_RELAY_PIN, LOW);
    if (Locationmm > OVENREADYPOS+5){
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
    } else {
        motor::OvenControl(STOP);
    }
}