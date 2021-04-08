//
// Created by Ethan Wesley on 19/11/2020.
//

#include "table.h"
#include "param.h"

static const Table_Entry stateTable[] = {
        //Current    Transition     Next
        //State ID    Criteria    StateID
        {   ZERO,    'A',  0x0105, SET},
        {   SET,     'B',  0x000C, HEATING},
        {   HEATING, 'C',  0x0034, READY},
        {   HEATING, 'D',  0x0004, COLD},
        {   READY,   'E',  0x0012, RUNNING},
        {   READY,   'F',  0x0004, COLD},
        {   READY,   'G',  0x000C, HEATING},
        {   RUNNING, 'H',  0x0054, COMPLETE},
        {   RUNNING, 'I',  0x0002, DROP},
        {   COMPLETE,'J',  0x0002, DROP},
        {   DROP,    'K',  0x0004, COLD},
        {   COLD,    'L',  0x0105, SET},
};

static const unsigned int TABLE_SIZE = sizeof(stateTable) / sizeof(stateTable[0]);

Table_Entry const * tableBegin(void){
    return &stateTable[0];
}

Table_Entry const * tableEnd(void){
    return &stateTable[TABLE_SIZE];
}