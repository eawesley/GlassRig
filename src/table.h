//
// Created by Ethan Wesley on 19/11/2020.
//

#ifndef TABLE_H
#define TABLE_H

#include "Arduino.h"

struct Table_Entry{
    unsigned int currentStateID;
    unsigned char transitionID;
    uint16_t transitionCriteria;
    unsigned int nextStateID;
};

Table_Entry const * tableBegin(void);
Table_Entry const * tableEnd(void);

#endif //TABLE_H
