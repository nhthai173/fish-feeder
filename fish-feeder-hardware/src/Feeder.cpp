#include "Feeder.h"

Feeder::Feeder(uint8_t servo_pin)
{
    pinMode(servo_pin, OUTPUT);
    servo.attach(servo_pin);
}

Feeder::~Feeder()
{
    servo.detach();
}


void Feeder::feed(uint8_t amount, std::function<void()> callback)
{
    if (isFeeding)
        return;

    this->amount = amount;
    this->callback = callback;
    startTime = millis();
    duration = amount * 1000 / 100;

    _feed();
}


void Feeder::run()
{
    if (isFeeding && millis() - startTime >= duration)
    {
        isFeeding = false;
        servo.write(0);
        if (callback)
            callback();
    }
}



void Feeder::stop()
{
    isFeeding = false;
    servo.write(0);
}


void Feeder::_feed()
{
    isFeeding = true;
    servo.write(180);
}