#ifndef FEEDER_H
#define FEEDER_H

#include "Arduino.h"
#include "Servo.h"

class Feeder
{
public:
    bool isFeeding = false;

    Feeder(uint8_t servo_pin);
    ~Feeder();

    void feed(uint8_t amount, std::function<void()> callback = NULL);
    void run();
    void stop();
private:
    Servo servo;
    uint8_t amount;
    std::function<void()> callback;
    unsigned long startTime;
    unsigned long duration;
    void _feed();
};

#endif // FEEDER_H