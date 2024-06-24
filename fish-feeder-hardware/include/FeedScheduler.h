//
// Created by Thái Nguyễn on 23/6/24.
//

#ifndef FISH_FEEDER_HARDWARE_FEEDSCHEDULER_H
#define FISH_FEEDER_HARDWARE_FEEDSCHEDULER_H

#include "scheduler.hpp"

class FeedTaskArgs : public ScheduleTaskArgsBase
{
public:
    uint8_t amount = 0;

    void parse(String &args) override
    {
        amount = args.toInt();
    }

    String toString() override
    {
        return String(amount);
    }

};

template class Scheduler<FeedTaskArgs>;

#endif //FISH_FEEDER_HARDWARE_FEEDSCHEDULER_H
