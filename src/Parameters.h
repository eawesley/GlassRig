//
// Created by Ethan Wesley on 17/11/2020.
//

#ifndef PARAM_H
#define PARAM_H

#include <stdint.h>

#define TRUE 1
#define FALSE 0

extern bool DEBUG;
extern bool DRYRUN;

// Run parameters
extern unsigned int OVENRUNSPEED;
extern unsigned int OVENDECENTSPEED;
extern unsigned int OVENHOMESPEEDCOARSE;
extern unsigned int OVENHOMESPEEDFINE;
extern unsigned int OVENMANSPEED;
extern unsigned int OVENTORPOS;
extern unsigned int GLASSROTSPEED;
extern unsigned int OVENSETPOINT;
extern unsigned int OVENRAMPRATE;
extern unsigned int OVENREADYPOS;
extern unsigned int UNDERTEMPTHRESH;


//Global constants
extern int CLK_PRESCALER;
extern int UM_PER_HALF_PULSE;

extern float ADC_LOW_CURR;
extern float ADC_HIGH_CURR;
extern float ADC_LOW;
extern float ADC_HIGH;

extern float MAX_T_MANDREL;
extern float MIN_T_MANDREL;
extern float MAX_T_GLASS;
extern float MIN_T_GLASS;
extern float MAX_P_VAC;
extern float MIN_P_VAC;

enum DIRECTION{
    UP = 1,
    DOWN = -1
};

enum CONTROL{
    START = 1,
    STOP = 0
};

enum PID{
    PID_ENABLE = 0,
    PID_DISABLE = 1
};

enum STATE{
    ZERO = 1,
    SET = 2,
    HEATING = 3,
    READY = 4,
    RUNNING = 5,
    COMPLETE = 6,
    DROP = 7,
    COLD = 8,
    FAULT = 9
};

// Global Variables
extern unsigned int currOvenSpeed;
extern unsigned int currGlassSpeed;
extern unsigned int currOvenDir;
extern unsigned int currGlassDir;

extern int8_t fDir;
extern int32_t Location;
extern float Locationmm;

extern bool bHomeHalt;
extern bool bFineHomeStop;

extern char transmitBuffer[128];

// Status flags
extern const uint16_t mHomed;
extern const uint16_t mMoving;
extern const uint16_t mStopped;
extern const uint16_t mHeating;
extern const uint16_t mAtTemp;
extern const uint16_t mBotOfRun;
extern const uint16_t mTopOfRun;
extern const uint16_t mUpLimit;
extern const uint16_t mLowLimit;
extern const uint16_t mFaultState;

// Error masks
extern const uint16_t mErrOvenTempMinor;

extern const uint16_t mErrOvenComms;
extern const uint16_t mErrOvenTemp;
extern const uint16_t mErrLowVac;
extern const uint16_t mErrEndStop;
extern const uint16_t mFault;

extern uint16_t fState;
extern uint16_t fError;

volatile extern bool btnGreen;
volatile extern bool btnRed;

// Global timers
volatile extern long homeTimer;
volatile extern long homeTimerEnd;
volatile extern long sensorFailTimer;
extern unsigned long sensorFailDelay;
volatile extern long btnTimerG;
volatile extern long btnTimerR;
volatile extern long btnLastPressG;
volatile extern long btnLastPressR;

volatile extern long ltLastFlash;
volatile extern long ltFlashInt;

volatile extern long lastTransmit;
volatile extern long lastSample;
extern unsigned int txInterval;
extern unsigned int spInterval;

extern unsigned int delayTimer;
extern unsigned int longDelay;

unsigned extern int currentState;
unsigned extern int nextState;

unsigned extern int mbTimeoutCnt;
unsigned extern int mbTimeout;


#endif //PARAM_H
