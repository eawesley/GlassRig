#include "pins.h"
#include <Arduino.h>
#include "init.h"
#include "param.h"
#include "ovenserial.h"
#include "table.h"
#include "transition.h"
#include "logging.h"
#include "sensors.h"
#include "motors.h"
#include <SPI.h>
#include <Ethernet.h>


/*
 *
 * Last edited by Ethan 22/03/2021 Arduino 1.8.13
 *
 *
 */

//============================================================================

// ISR
void GreenPressed();
void RedPressed();

// Initialisation
void Initialise();
void EtherConnect();

// Main processes
void ButtonPoll();
void StateMachine();
void ManualOvenControl();
void SensorSample();
void TransmitData();
void FaultCheck();


void setup() {

    Initialise();
    //EtherConnect();

}

//============================================================================

void loop() {

    // DEBUGGING
    ///*

    //*/
    //------------------------------------------
    ///*
    while(TRUE){

        ButtonPoll();
        StateMachine();
        ManualOvenControl();

        SensorSample();
        TransmitData();
        FaultCheck();

    }
//*/
}


//================================================================================


ISR(TIMER3_COMPA_vect) {
    if(fState & mMoving) Location += (UM_PER_HALF_PULSE * fDir);  //1.5um per pulse Updates Location (based on 333.3 Pulses/mm)
    Locationmm = Location/10000.;
}

ISR(TIMER4_COMPA_vect) {
//needs definition for NVIC interrupt
}

// Button interrupts:
void GreenPressed() {
    btnTimerG = millis();
    //check to see if GreenPressed() was called in the last 250 milliseconds
    if (btnTimerG - btnLastPressG > 250){
        btnGreen = true;
    }
    btnLastPressG = btnTimerG;
}

void RedPressed(){
    btnTimerR = millis();
    //check to see if RedPressed() was called in the last 250 milliseconds
    if (btnTimerR - btnLastPressR > 250){
        btnRed = true;
    }
    btnLastPressR = btnTimerR;
}

void Initialise(){
    //TODO: move all below to init.cpp (should only call init::Begin from setup)

    // Set interrupt pins
    attachInterrupt(digitalPinToInterrupt(BTNGREEN), GreenPressed, RISING);
    attachInterrupt(digitalPinToInterrupt(BTNRED), RedPressed, FALLING);

    // For debugging: output to serial console (USB)
    Serial.begin(9600);

    // Run pin initialisation
    init::Begin();
    delay(2000); // delay for oven startup before sending commands

    // Initialize serial connection to oven
    Serial2.begin(9600);
    Oven.begin(2, Serial2);

    // Initilise oven to safe startup state
    ovenserial::ControlOutput(Oven, PID_DISABLE);
    ovenserial::SetPoint(Oven, 0);
    ovenserial::SetRamp(Oven, 0);

    currentState = ZERO;
    nextState = ZERO;
}

void EtherConnect(){
    byte mac[] = {0xE8, 0x2A, 0xEA, 0x4B, 0x1F, 0xC3};
    IPAddress localip(192, 168, 1, 201);
    IPAddress serverip(192, 168, 1, 202);

    EthernetClient client;
    Ethernet.begin(mac, localip);
}

void ButtonPoll(){
    if (btnGreen) {
        switch (currentState) {
            case ZERO:
                nextState = SET;
                break;
            case SET:
                nextState = HEATING;
                break;
            case READY:
                nextState = RUNNING;
                break;
            case COLD:
                nextState = SET;
                break;
            default:
                break;
        }
        digitalWrite(LTGREEN, LOW);
        digitalWrite(LTRED, LOW);
        btnGreen = false;
    }

    if (btnRed) {
        switch (currentState) {
            case HEATING:
                nextState = COLD;
                break;
            case READY:
                nextState = COLD;
                break;
            case RUNNING:
                nextState = DROP;
                break;
            default:
                break;
        }
        digitalWrite(LTGREEN, LOW);
        digitalWrite(LTRED, HIGH);
        btnRed = false;
    }
}

void StateMachine(){
    uint16_t transitionCriteria;
    char transitionID;
    Table_Entry const * pEntry = tableBegin();
    Table_Entry const * const pTableEnd = tableEnd();
    bool entryFound = false;

    while ((!entryFound) && (pEntry != pTableEnd)) {
        if(pEntry->currentStateID == currentState){
            if(pEntry->nextStateID == nextState){
                //Serial.println("Found state");
                entryFound = true;
                transitionID = pEntry->transitionID;
                transitionCriteria = pEntry->transitionCriteria;
                if (fState == transitionCriteria){
                    currentState = pEntry->nextStateID;
                    switch (currentState){ //auto advance states
                        case HEATING:
                            nextState = READY;
                            break;
                        case RUNNING:
                            nextState = COMPLETE;
                            break;
                        case COMPLETE:
                            nextState = DROP;
                            break;
                        case DROP:
                            nextState = COLD;
                            break;
                        default:
                            break;
                    }
                } else {
                    switch (transitionID) {
                        case 'A':
                            transA();
                            break;
                        case 'B':
                            transB();
                            break;
                        case 'C':
                            transC();
                            break;
                        case 'D':
                            transD();
                            break;
                        case 'E':
                            transE();
                            break;
                        case 'F':
                            transF();
                            break;
                        case 'G':
                            transG();
                            break;
                        case 'H':
                            transH();
                            break;
                        case 'I':
                            transI();
                            break;
                        case 'J':
                            transJ();
                            break;
                        case 'K':
                            transK();
                            break;
                        case 'L':
                            transL();
                            break;
                    }
                }
            }
        }
        ++pEntry;
    }

    if (!entryFound) {
        if(millis()-lastTransmit>txInterval) {
            //Serial.println("Transition not found.");
        }
        if (nextState == FAULT){
            transFault();
        }
    }
}

void ManualOvenControl(){
    //Manual control for stack entry
    if (currentState == nextState) { //if no transition defined (holding state)
        switch (nextState) {
            case ZERO:
            case READY:
                if (digitalRead(BTNUP)) {
                    motor::OvenControl(START, UP, OVENMANSPEED);
                } else if (digitalRead(BTNDOWN)) {
                    motor::OvenControl(START, DOWN, OVENMANSPEED);
                } else {
                    motor::OvenControl(STOP);
                }
                break;
            case COLD:
                if (millis() - ltLastFlash > ltFlashInt) {
                    digitalWrite(LTRED, !digitalRead(LTRED));
                    ltLastFlash = millis();
                }
                digitalWrite(VAC_RELAY_PIN, LOW);
                break;
            default:
                break;
        }
    }
}

void SensorSample(){
    // sensor sampling
    if (millis()-lastSample>spInterval) { sensorUpdate(); lastSample = millis(); }
}

void TransmitData(){
    // transmit data over serial (TODO: transmit data to web server)
    if (millis()-lastTransmit>txInterval) { ovenserial::HeartBeat(Oven); transmit(); lastTransmit = millis(); }
}

void FaultCheck(){
    // check for faults that will interrupt operation
    if (fError & mFault){ nextState = FAULT; }
}