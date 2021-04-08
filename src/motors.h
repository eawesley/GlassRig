//
// Created by Ethan Wesley on 17/11/2020.
//

#ifndef MOTORS_H
#define MOTORS_H

#include "param.h"
#include <Arduino.h>

class motor{
private:

    static void OvenTimer(CONTROL command, int ovenOCRV);
    static void GlassTimer(CONTROL command, int glassOCRV);

public:
    static void OvenControl(CONTROL command, DIRECTION dir = DIRECTION(currOvenDir), int speed = currOvenSpeed);
    static void GlassControl(CONTROL command, DIRECTION dir = DIRECTION(currGlassDir), int speed = currGlassSpeed);
};

#endif //MOTORS_H
