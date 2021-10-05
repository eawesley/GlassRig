//
// Created by Ethan Wesley on 17/11/2020.
//
#include "Motors.h"
#include "pins.h"

// Private
void motor::OvenTimer(CONTROL command, int ovenOCRV = 0){
    unsigned char sreg;
    switch (command){
        case START:
            sreg = SREG;                                        // save global interrupt flag
            cli();                                              // disable interrupt for shared register access
            OCR3A = ovenOCRV;                                   // write output compare register value
            TCCR3B |= (_BV(CS31) | _BV(CS30));                  // enable timer with prescaler = 64
            SREG = sreg;                                        // restore gloabl interrupt flag
            fState &= ~mStopped;
            fState |= mMoving;                                // verbosity allows stopped to be set elsewhere
            break;

        case STOP:
            TCCR3B &= ~(_BV(CS32) | _BV(CS31) | _BV(CS30));
            fState &= ~mMoving;
            fState |= mStopped;
            break;
    }
}

void motor::GlassTimer(CONTROL command, int glassOCRV = 0) {
    unsigned char sreg;
    switch (command){
        case START:
            sreg = SREG;                                            // save global interrupt flag
            cli();                                                  // disable interrupt for shared register access
            OCR4A = glassOCRV;                                      // write output compare register value
            TCCR4B |= (_BV(CS41) | _BV(CS40));                      // enable timer with prescaler = 64
            SREG = sreg;                                            // restore gloabl interrupt flag
            break;

        case STOP:
            TCCR4B &= ~(_BV(CS42) | _BV(CS41) | _BV(CS40));         // Clear timer registers -> disables timer
            break;
    }
}

// Public

void motor::OvenControl(CONTROL command, DIRECTION dir, int speed){
    unsigned int PPSO;
    switch (command){
        case START:
            if ((dir == UP) && !(fState & mUpLimit)) {
                digitalWrite(OVEN_DIR_PIN, HIGH);
                fDir = UP;
            } else if ((dir == DOWN) && !(fState & mUpLimit)) {
                digitalWrite(OVEN_DIR_PIN, LOW);
                fDir = DOWN;
            }
            PPSO = (speed * 55) / 100;                               // 5.55 multiplier scaled to fit into 16 bit int (loss of precision)
            OvenTimer(START, (12500 / PPSO));
            currOvenSpeed = speed;
            currOvenDir = dir;
            break;
        case STOP:
            OvenTimer(STOP);
            break;
    }
}

void motor::GlassControl(CONTROL command, DIRECTION dir, int speed){
    unsigned int PPSG;
    switch (command){
        case START:
            if (dir == UP){
                digitalWrite(GLASS_DIR_PIN, HIGH);
            } else if (dir == DOWN){
                digitalWrite(GLASS_DIR_PIN, LOW);
            }
            PPSG = speed * 3;
            GlassTimer(START, (25000/(PPSG*2)-1));
            currGlassSpeed = speed;
            currGlassDir = dir;
            break;

        case STOP:
            GlassTimer(STOP);
            break;
    }

}
