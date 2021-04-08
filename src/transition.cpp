//
// Created by Ethan Wesley on 19/11/2020.
//

#include "transition.h"
#include "pins.h"
#include "motors.h"
#include "param.h"
#include "ovenserial.h"
#include "sensors.h"
#include <Arduino.h>

void transA(){
    // ZERO->SET Homing routine to bottom limit switch, complete when at Location = 0;
    if (!bFineHomeStop) {
        if (digitalRead(LIMIT_BOT) && !bHomeStop) {                                                    // limit is active high
            motor::OvenControl(START, UP,OVENHOMESPEEDCOARSE);
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDCOARSE);
            bHomeStop = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            bFineHomeStop = true;
            bHomeStop = false;
        }
    } else if (bFineHomeStop){
        if (homeTimer == 0) {
            homeTimer = millis();
        }
        if ((millis() - homeTimer < 1000) && !bHomeStop) {                                                  // limit is active high
            motor::OvenControl(START, UP, OVENHOMESPEEDCOARSE);
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDFINE);
            bHomeStop = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            homeTimer = 0;
            bFineHomeStop = false;
            bHomeStop = false;
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
            ovenserial::SetPoint(Oven, OVENSETPOINT);
            ovenserial::SetRamp(Oven, OVENRAMPRATE);
            ovenserial::ControlOutput(Oven, PID_ENABLE);
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
    ovenserial::ControlOutput(Oven, PID_DISABLE);
    fState &= ~(mHeating | mAtTemp | mBotOfRun);
    if (Locationmm > OVENREADYPOS/2){
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
    } else {
        motor::OvenControl(STOP);
    }
}

void transE(){
    // READY->RUNNING Start glass rotation, delay, start oven at climb speed
    motor::GlassControl(START, UP, GLASSROTSPEED);
    digitalWrite(SOL3_PIN, HIGH); //TODO move to configuration definition
    digitalWrite(VAC_RELAY_PIN, HIGH);
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
    ovenserial::ControlOutput(Oven, PID_DISABLE);
    ovenserial::SetPoint(Oven, 0);
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
        ovenserial::ControlOutput(Oven, PID_DISABLE);
        ovenserial::SetPoint(Oven, 0);
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
        motor::GlassControl(STOP);
        fState &= ~(mAtTemp);
    }
}

void transJ(){
    // COMPLETE->DROP Oven is lowered after completed moulding run
    if(Locationmm >= OVENREADYPOS/2){
        ovenserial::ControlOutput(Oven, PID_DISABLE);
        ovenserial::SetPoint(Oven, 0);
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
        if (digitalRead(LIMIT_BOT) && !bHomeStop) {                                                    // limit is active high
            motor::OvenControl(START, UP,OVENHOMESPEEDCOARSE);
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDCOARSE);
            bHomeStop = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            bFineHomeStop = true;
            bHomeStop = false;
        }
    } else if (bFineHomeStop){
        if (homeTimer == 0) {
            homeTimer = millis();
        }
        if ((millis() - homeTimer < 1000) && !bHomeStop) {                                                  // limit is active high
            motor::OvenControl(START, UP, OVENHOMESPEEDCOARSE);
        } else if (!digitalRead(LIMIT_BOT)) {
            motor::OvenControl(START, DOWN, OVENHOMESPEEDFINE);
            bHomeStop = true;
        } else if (digitalRead(LIMIT_BOT)) {
            motor::OvenControl(STOP);
            Location = 0;
            homeTimer = 0;
            bFineHomeStop = false;
            bHomeStop = false;
            fState |= (mHomed | mLowLimit);
            digitalWrite(LTGREEN, HIGH);
        }
    }
}

void transFault() {
    fState = mFaultState;
    ovenserial::ControlOutput(Oven, PID_DISABLE);
    ovenserial::SetPoint(Oven, 0);
    motor::GlassControl(STOP);
    digitalWrite(VAC_RELAY_PIN, LOW);
    if (Locationmm > OVENREADYPOS+5){
        motor::OvenControl(START, DOWN, OVENDECENTSPEED);
    } else {
        motor::OvenControl(STOP);
    }
}