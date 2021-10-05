//
// Created by Ethan Wesley on 17/11/2020.
//
#include "Parameters.h"
// TODO: create struct (typedef struct) and declare as extern -

bool DEBUG = FALSE;
bool DRYRUN = TRUE;

// Run parameters
unsigned int OVENRUNSPEED = 21;                 // mm/min - raising speed of sled during run
unsigned int OVENDECENTSPEED = 550;             // mm/min - lowering speed of sled after run
unsigned int OVENHOMESPEEDCOARSE = 300;         // coarse home speed
unsigned int OVENHOMESPEEDFINE = 40;            // fine home speed
unsigned int OVENMANSPEED = 100;                // manual manoeuvre speed
unsigned int OVENTORPOS = 1500;                 // mm position for top of run (complete position)
unsigned int GLASSROTSPEED = 4;                 // rev/min - cane rotation speed
unsigned int OVENSETPOINT = 700;                // degrees C - oven run temperature
unsigned int OVENRAMPRATE = 35;                 // degrees C - oven heating ramp rate
unsigned int OVENREADYPOS = 120;                // mm - position above bottom stop before run start
unsigned int UNDERTEMPTHRESH = 5;               // degrees under set point to raise fault or re-enter "heating";

// Global constants
int CLK_PRESCALER = 64;                         // With 16Mhz ref clock -> 250kHz timer frequency
int UM_PER_HALF_PULSE = 15;                     //1.5 um(*10) per half pulse based on 6mm pitch lead screw (2000PPR, 6mmPR)

float ADC_LOW_CURR = 190;                        // defined as floats to match interp routine and force float operations
float ADC_HIGH_CURR = 1023;                     // 205 = 1V (tweaked for measured voltage with MM), 1023 = 5V (minimum drop over 250R at 4mA -> 1V)
float ADC_LOW = 0;
float ADC_HIGH = 1023;

float MAX_T_MANDREL = 1000;                     // degrees C
float MIN_T_MANDREL = 250;
float MAX_T_GLASS = 800;
float MIN_T_GLASS = 10;
float MAX_P_VAC = 3447.38;                      // mbar
float MIN_P_VAC = 0;

// Global Variables
unsigned int currOvenSpeed = 0;                 // mm/min
unsigned int currGlassSpeed = 0;                // rpm
unsigned int currOvenDir = 1;
unsigned int currGlassDir = 1;

int8_t fDir = 1;                                // Direction flag. Set this to keep track of location
int32_t Location = 0;                           // nanometers (m*10^-9) scaled by 10^-6 to avoid FP operations in interrupt
float Locationmm = Location;

bool bHomeHalt = false;                          // Flag to stop homing sequence
bool bFineHomeStop = false;                      // Flag to start homing sequence

char transmitBuffer[128];

// State masks
const uint16_t mHomed      = 0x0001;
const uint16_t mMoving     = 0x0002;
const uint16_t mStopped    = 0x0004;
const uint16_t mHeating    = 0x0008;
const uint16_t mAtTemp     = 0x0010;
const uint16_t mBotOfRun   = 0x0020;
const uint16_t mTopOfRun   = 0x0040;
const uint16_t mUpLimit    = 0x0080;
const uint16_t mLowLimit   = 0x0100;
const uint16_t mFaultState = 0x8000;

// State flag
uint16_t fState;

// Error masks
    // Warnings (continue operation)
const uint16_t mErrOvenTempMinor  = 0x0001;
    // Faults (stop operation)
const uint16_t mErrOvenComms      = 0x0100;
const uint16_t mErrOvenTemp       = 0x0200;
const uint16_t mErrLowVac         = 0x0400;
const uint16_t mErrEndStop        = 0x0800;
    // Fault mask
const uint16_t mFault = 0xFF00;

// Error flag
uint16_t fError;

volatile bool btnGreen = false;
volatile bool btnRed = false;

// Global timers (in ms unless otherwise specified)
volatile long homeTimer = 0;
volatile long homeTimerEnd = 0;

volatile long sensorFailTimer = 0;                          // ms - timer for detecting bottom stop sensor failure during home sequence
unsigned long sensorFailDelay = 5000;

volatile long btnTimerG = 0;
volatile long btnTimerR = 0;
volatile long btnLastPressG = 0;
volatile long btnLastPressR = 0;

volatile long ltLastFlash = 0;
volatile long ltFlashInt = 1000;

volatile long lastTransmit = 0;
volatile long lastSample = 0;
unsigned int txInterval = 1000;                             // ms - transmit interval (must be greater than Modbus timeout in ModbusMaster.h
unsigned int spInterval = 10;                               // ms - sensor sampling frequency

unsigned int delayTimer = 0;
unsigned int longDelay = 5000;

unsigned int currentState;
unsigned int nextState;

unsigned int mbTimeoutCnt = 0;
unsigned int mbTimeout = 5;                             //Timeout as a multiple of logging frequency (1/s)
