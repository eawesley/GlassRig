//
// Created by Ethan Wesley on 16/11/2020.
//

#include "pins.h"
#include "Initialise.h"
#include "Parameters.h"
//#include "OvenSerial.h"
#include <Arduino.h>

// Private
void Init::PinIO(){
    // Stepper motor controller outputs
    pinMode(OVEN_STEP_PIN, OUTPUT);
    pinMode(OVEN_DIR_PIN, OUTPUT);
    pinMode(GLASS_STEP_PIN, OUTPUT);
    pinMode(GLASS_DIR_PIN, OUTPUT);

    //Solenoid outputs
    pinMode(SOL1_PIN, OUTPUT);
    pinMode(SOL2_PIN, OUTPUT);
    pinMode(SOL3_PIN, OUTPUT);

    //LED indicators
    pinMode(LTGREEN, OUTPUT);
    pinMode(LTRED, OUTPUT);

    //Limit switches
    pinMode(LIMIT_BOT, INPUT);                      // INPUT definitions are not neccessary but good to be explicit
    pinMode(LIMIT_TOP, INPUT);

    //Analog inputs
    pinMode(GLASS_TEMP_PIN, INPUT);
    pinMode(MANDREL_TEMP_PIN, INPUT);
    pinMode(VAC_PRES_PIN, INPUT);

    //Button interrupts
    pinMode(BTNGREEN, INPUT);
    pinMode(BTNRED, INPUT);

    //Manual buttons
    pinMode(BTNUP, INPUT);
    pinMode(BTNDOWN, INPUT);

    //Power Relays
    pinMode(OVEN_RELAY_PIN, OUTPUT);
    pinMode(VAC_RELAY_PIN, OUTPUT);
}

void Init::PinSet(){
    digitalWrite(SOL1_PIN, LOW);
    digitalWrite(SOL2_PIN, LOW);
    digitalWrite(SOL3_PIN, LOW);

    digitalWrite(OVEN_RELAY_PIN, HIGH);

    digitalWrite(LTGREEN, LOW);
    digitalWrite(LTRED, LOW);
}

void Init::TimerSet(){
    // Timer setup 3 (oven) and 4 (glass)
    // TODO: dont set prescaler here - set in start routine for rotation and oven
    unsigned char sreg;
    sreg = SREG;
    cli();

    TCCR3A = _BV(COM3A0) | _BV(COM3B1) | _BV(WGM31) | _BV(WGM30);     // Set timer 3 to fast PWM mode and toggle OC3A on compare match
    TCCR3B = _BV(WGM33) | _BV(WGM32) | _BV(CS31) | _BV(CS30);         // Set prescaler to 64 (16Mhz/64 = 250kHz)
    OCR3A = 0;                                                        // (CLK/PRESCALER*PPSO*2)-1 -> datasheet pg146
    OCR3B = 0;                                                        // Not used in mode 15 but set anyway
    TIMSK3 = _BV(OCIE3A);                                             // Set for output compare interrupt
    TCCR3B &= ~(_BV(CS32) | _BV(CS31) | _BV(CS30));                   // Disable - ready for start

    TCCR4A = _BV(COM4A0) | _BV(COM4B1) | _BV(WGM41) | _BV(WGM40);     // Set timer 3 to fast PWM mode and toggle OC4A on compare match
    TCCR4B = _BV(WGM43) | _BV(WGM42) | _BV(CS41) | _BV(CS40);         // Set prescaler to 64 (16Mhz/64 = 250kHz)
    OCR4A = 0;                                                        // (CLK/PRESCALER*PPSO*2)-1 -> datasheet pg146
    OCR4B = 0;                                                        // Not used in mode 15 but set anyway
    TIMSK4 = _BV(OCIE4A);                                             // Set for output compare interrupt
    TCCR4B &= ~(_BV(CS42) | _BV(CS41) | _BV(CS40));                   // Disable - ready for start

    SREG = sreg;
    //sei();                                                            // Enables interrups. Use cli() to turn them off
}

// Public
void Init::Begin(){
    PinIO();
    PinSet();
    TimerSet();
}