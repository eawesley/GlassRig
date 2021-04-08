//
// Created by Ethan Wesley on 30/11/2020.
//

#ifndef SENSORS_H
#define SENSORS_H

// Sensor data struct
extern struct sensor{
    float tOven;
    float tMandrel;
    float tGlass;
    float pVac;
}senseData;

void sensorUpdate();
void sensorRead();
void tOvenRead();
float tMandrelRead();
float tGlassRead();
float pVacRead();
float adcMapCurr(float val, float toLow, float toHigh);
float adcMap(float val, float toLow, float toHigh);

#endif //GLASSRIGCONTROL_SENSORS_H
