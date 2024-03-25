#ifndef FEED_LOG_H
#define FEED_LOG_H

#include "stdint.h"
#include "LittleFS.h"
#include "NTPClient.h"


class FeedLog
{
public:
    String filePath = "/feed-log.txt";
    uint8_t MAX_LOG_TIME = 30; // in days

    FeedLog(NTPClient *timeClient);
    ~FeedLog();

    /**
     * @brief Add a new log to the log file
     * 
     * @param log 
     * @return true 
     * @return false 
     */
    bool add(uint8_t amount);

    /**
     * @brief Get all logs from the log file
     * 
     * @return String 
     */
    String gets();

    /**
     * @brief Clear logs over MAC_LOG_TIME days
     * 
     * @return true 
     * @return false 
     */
    bool clearOld();

    /**
     * @brief Clear all logs
     * 
     * @return true 
     * @return false 
     */
    bool clearAll();

    private:
    NTPClient *timeClient = NULL;
};


#endif