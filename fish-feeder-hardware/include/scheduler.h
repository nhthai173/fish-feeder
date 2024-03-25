#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "stdint.h"
#include <vector>
#include "LittleFS.h"
#include "NTPClient.h"

struct schedule_time_t
{
    uint8_t hour;
    uint8_t minute;
};

struct schedule_repeat_t
{
    bool monday;
    bool tuesday;
    bool wednesday;
    bool thursday;
    bool friday;
    bool saturday;
    bool sunday;
};

struct schedule_task_t
{
    uint8_t id;
    schedule_time_t time;
    schedule_repeat_t repeat;
    uint8_t amount;
    bool enabled;
    bool executed;
};



class Scheduler
{

public:
    String filePath = "/schedules.txt";
    uint8_t MAX_TASKS = 10;
    
    Scheduler(NTPClient *timeClient);
    ~Scheduler();

    /**
     * @brief Add a new task to the schedule
     * 
     * @param task 
     * @return true 
     * @return false 
     */
    bool addTask(schedule_task_t task);
    
    /**
     * @brief Remove a task from the schedule by its id
     * 
     * @param id 
     * @return true 
     * @return false 
     */
    bool removeTask(uint8_t id);
    
    /**
     * @brief Update a task in the schedule by its id
     * 
     * @param id 
     * @param task new task to update
     * @return true 
     * @return false 
     */
    bool updateTask(uint8_t id, schedule_task_t task);
    
    /**
     * @brief Set the Callback object
     * 
     * @param callback 
     */
    void setCallback(std::function<void(uint8_t)> callback);
    
    
    /**
     * @brief Get number of tasks in the schedule
     * 
     * @return uint8_t 
     */
    uint8_t getTaskCount();

    /**
     * @brief Load tasks from file
     * 
     */
    bool load();

    /**
     * @brief Save tasks to file
     * 
     * @return true 
     * @return false 
     */
    bool save();

    /**
     * @brief Update time from NTP server
     * 
     */
    void updateTime();

    /**
     * @brief Run the scheduler
     * 
     */
    void run();



    /**
     * @brief Print all tasks to serial
     * 
     * @param serial 
     */
    void printToSerial(Stream &stream);


private:
    std::vector<schedule_task_t> tasks;
    std::function<void(uint8_t)> callback;
    File file;
    NTPClient *timeClient;

    void openFile();
    void closeFile();

    /**
     * @brief Append a task to file
     * 
     * @param file file object
     * @param task task object
     * @return true 
     * @return false 
     */
    bool writeTaskToFile(schedule_task_t *task);
    
    
    /**
     * @brief Parse a task from string
     * 
     * @param task 
     * @return schedule_task_t 
     */
    schedule_task_t parseTask(String task);
    
};



#endif