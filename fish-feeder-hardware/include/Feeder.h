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
     /**
      * @brief Feed immediately
      * 
      * @param amount amount of food to feed
      * @param callback callback function to be called when feeding is done
      */
    void feed(uint8_t amount, std::function<void()> callback = NULL);
    
    /**
     * @brief Put this function in loop() to run the feeder
     * 
     */
    void run();

    /**
     * @brief Stop feeding immediately
     * 
     */
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