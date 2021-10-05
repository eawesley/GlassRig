//
// Created by Ethan Wesley on 30/11/2020.
//

#include <Arduino.h>
#include "pins.h"
#include "Parameters.h"
#include "OvenSerial.h"
#include "Sensors.h"

sensor senseData;

const int numReadings  = 40;

float readingsMandrel[numReadings];
float readingsGlass[numReadings];
float readingsVac[numReadings];

int readIndexMandrel;
int readIndexGlass;
int readIndexVac;

float totalMandrel;
float totalGlass;
float totalVac;

void sensorUpdate(){
    senseData.tMandrel = tMandrelRead();
    senseData.tGlass = tGlassRead();
    senseData.pVac = pVacRead();
}

void sensorRead(){
    tOvenRead();
    sensorUpdate();
}

float adcMapCurr(float val, float toLow, float toHigh){
    return (toLow + ((toHigh - toLow) * ((val - ADC_LOW_CURR) / (ADC_HIGH_CURR - ADC_LOW_CURR))));
}

float adcMap(float val, float toLow, float toHigh){
    return (toLow + ((toHigh - toLow) * ((val - ADC_LOW) / (ADC_HIGH - ADC_LOW))));
}

void tOvenRead(){
    OvenSerial::UpdateTemperature(Oven);
}
float tMandrelRead(){
    float average;

    totalMandrel = totalMandrel - readingsMandrel[readIndexMandrel];
    readingsMandrel[readIndexMandrel] = adcMapCurr(analogRead(MANDREL_TEMP_PIN), MIN_T_MANDREL, MAX_T_MANDREL);
    totalMandrel = totalMandrel + readingsMandrel[readIndexMandrel];
    readIndexMandrel = readIndexMandrel + 1;
    if (readIndexMandrel >= numReadings) {
        readIndexMandrel = 0;
    }
    average = totalMandrel / numReadings;

    return average;
}
float tGlassRead(){
    float average;

    totalGlass = totalGlass - readingsGlass[readIndexGlass];
    readingsGlass[readIndexGlass] = adcMapCurr(analogRead(GLASS_TEMP_PIN), MIN_T_GLASS, MAX_T_GLASS);
    totalGlass = totalGlass + readingsGlass[readIndexGlass];
    readIndexGlass = readIndexGlass + 1;
    if (readIndexGlass >= numReadings) {
        readIndexGlass = 0;
    }
    average = totalGlass / numReadings;

    return average;
}
float pVacRead(){
    float average;

    totalVac = totalVac - readingsVac[readIndexVac];
    readingsVac[readIndexVac] = adcMap(analogRead(VAC_PRES_PIN), MIN_P_VAC, MAX_P_VAC);
    totalVac = totalVac + readingsVac[readIndexVac];
    readIndexVac = readIndexVac + 1;
    if (readIndexVac >= numReadings) {
        readIndexVac = 0;
    }
    average = totalVac / numReadings;

    return average;
}