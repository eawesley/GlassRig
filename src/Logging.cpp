//
// Created by Ethan Wesley on 30/11/2020.
//

#include "Logging.h"
#include "Sensors.h"
#include "Parameters.h"
#include <stdio.h>

content message;

void construct(){
    sensorRead(); //update values
    dtostrf(millis()/1000.,5,2,message.log_time);
    message.log_currentState = currentState;
    message.log_nextState = nextState;
    message.log_s_flags = fState;
    message.log_e_flags = fError;
    dtostrf(senseData.tOven,5, 2, message.log_tOven);
    dtostrf(senseData.tMandrel,5, 2, message.log_tMandrel);
    dtostrf(senseData.tGlass,5, 2, message.log_tGlass);
    dtostrf(senseData.pVac,5, 2, message.log_pVac);
    dtostrf(Location/10000., 3, 2, message.log_location);
}

void transmit(){
    construct();
    int b;
    b=sprintf (transmitBuffer, "%s,%i,%i,%i,%i,%s,%s,%s,%s,%s",
               message.log_time,
               message.log_currentState,
               message.log_nextState,
               message.log_s_flags,
               message.log_e_flags,
               message.log_tOven,
               message.log_tMandrel,
               message.log_tGlass,
               message.log_pVac,
               message.log_location);
    if(Serial){
        for(int l= 0; l<=b; l++) {
            Serial.print(transmitBuffer[l]);
        }
        Serial.print('\n');
    }
}