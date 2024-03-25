#include "FeedLog.h"

FeedLog::FeedLog(NTPClient *timeClient)
{
    this->timeClient = timeClient;
    
    LittleFS.begin();
    if (!LittleFS.exists(filePath))
    {
        File file = LittleFS.open(filePath, "w");
        file.close();
    }

    if (!LittleFS.exists(maxTimeFilePath))
    {
        File file = LittleFS.open(maxTimeFilePath, "w");
        file.print(MAX_LOG_TIME);
        file.close();
    }
    else
    {
        MAX_LOG_TIME = getMaxLogTime();
    }
}

FeedLog::~FeedLog()
{
    LittleFS.end();
}

uint16_t FeedLog::getMaxLogTime()
{
    File file = LittleFS.open(maxTimeFilePath, "r");
    if (!file)
        return MAX_LOG_TIME;
    return file.parseInt();
}

bool FeedLog::setMaxLogTime(uint16_t days)
{
    File file = LittleFS.open(maxTimeFilePath, "w");
    if (!file)
        return false;
    file.print(days);
    file.close();
    return true;
}

bool FeedLog::add(uint8_t amount)
{
    File file = LittleFS.open(filePath, "a");
    if (!file)
    {
        return false;
    }

    timeClient->update();
    file.seek(file.size());
    file.printf("%ld %dg\n", timeClient->getEpochTime(), amount);
    file.close();
    return true;
}

String FeedLog::gets()
{
    File file = LittleFS.open(filePath, "r");
    if (!file)
        return "";
    return file.readString();
}

bool FeedLog::clearOld()
{
    File file = LittleFS.open(filePath, "r");
    if (!file)
        return false;

    timeClient->update();
    uint32_t currentTime = timeClient->getEpochTime();
    uint32_t maxLogTime = MAX_LOG_TIME * 24 * 60 * 60;
    File tempFile = LittleFS.open("/temp-log.txt", "w");
    if (!tempFile)
        return false;

    while (file.available())
    {
        String line = file.readStringUntil('\n');
        uint32_t logTime = line.substring(0, line.indexOf(' ')).toInt();
        if (currentTime - logTime < maxLogTime)
            tempFile.println(line);
    }

    file.close();
    tempFile.close();
    LittleFS.remove(filePath);
    LittleFS.rename("/temp-log.txt", filePath);
    return true;
}


bool FeedLog::clearAll()
{
    if (!LittleFS.remove(filePath))
        return false;
    File file = LittleFS.open(filePath, "w");
    file.close();
    return true;
}
