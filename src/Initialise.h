//
// Created by Ethan Wesley on 16/11/2020.
//

#ifndef INIT_H
#define INIT_H

class Init {
private:
    static void PinIO();
    static void PinSet();
    static void TimerSet();
public:
    static void Begin();
};

#endif //INIT_H
