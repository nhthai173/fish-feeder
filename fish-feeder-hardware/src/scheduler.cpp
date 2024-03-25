#include "scheduler.h"

Scheduler::Scheduler()
{
    LittleFS.begin();
    load();
    timeClient.begin();
}

Scheduler::~Scheduler()
{
    tasks.clear();
    closeFile();
    LittleFS.end();
}

void Scheduler::openFile()
{
    if (!this->file)
    {
        this->file = &LittleFS.open(filePath, "a");
    }
}

void Scheduler::closeFile()
{
    if (this->file)
    {
        this->file->close();
        this->file = nullptr;
    }
}

bool Scheduler::writeTaskToFile(schedule_task_t *task)
{
    openFile();
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return false;
    }

    file->seek(file->size());
    file->printf("%d|%d|%d|%d%d%d%d%d%d|%d|%d|%d\n",
                 task->id,
                 task->time.hour,
                 task->time.minute,
                 task->repeat.monday,
                 task->repeat.tuesday,
                 task->repeat.wednesday,
                 task->repeat.thursday,
                 task->repeat.friday,
                 task->repeat.saturday,
                 task->repeat.sunday,
                 task->amount,
                 task->enabled,
                 task->executed);

    return true;
}

schedule_task_t Scheduler::parseTask(String task)
{
    schedule_task_t result;
    int index = 0;
    String id = "";
    while (task[index] != '|')
    {
        id += task[index];
        index++;
    }
    result.id = id.toInt();
    index++;
    String hour = "";
    while (task[index] != '|')
    {
        hour += task[index];
        index++;
    }
    result.time.hour = hour.toInt();
    index++;
    String minute = "";
    while (task[index] != '|')
    {
        minute += task[index];
        index++;
    }
    result.time.minute = minute.toInt();
    index++;
    String repeat = "";
    while (task[index] != '|')
    {
        repeat += task[index];
        index++;
    }
    result.repeat.monday = repeat[0] == '1';
    result.repeat.tuesday = repeat[1] == '1';
    result.repeat.wednesday = repeat[2] == '1';
    result.repeat.thursday = repeat[3] == '1';
    result.repeat.friday = repeat[4] == '1';
    result.repeat.saturday = repeat[5] == '1';
    result.repeat.sunday = repeat[6] == '1';
    index++;
    String amount = "";
    while (task[index] != '|')
    {
        amount += task[index];
        index++;
    }
    result.amount = amount.toInt();
    index++;
    String enabled = "";
    while (task[index] != '|')
    {
        enabled += task[index];
        index++;
    }
    result.enabled = enabled.toInt();
    index++;
    String executed = "";
    while (task[index] != '\n'  || task[index] != '\0' || index < task.length())
    {
        executed += task[index];
        index++;
    }
    result.executed = executed.toInt();
    return result;
}

void Scheduler::setCallback(std::function<void(uint8_t)> callback)
{
    this->callback = callback;
}

bool Scheduler::load()
{
    openFile();
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return false;
    }

    // Clear tasks
    tasks.clear();

    // Read file
    String line;
    while (file->available())
    {
        line = file->readStringUntil('\n');
        if (line.length() > 0)
        {
            schedule_task_t task = parseTask(line);
            tasks.push_back(task);
        }
    }

    closeFile();
    return true;
}

bool Scheduler::save()
{
    for (uint8_t i = 0; i < tasks.size(); i++)
    {
        if (!writeTaskToFile(&tasks[i]))
            return false;
    }
    closeFile();
    return true;
}

bool Scheduler::addTask(schedule_task_t task)
{
    if (tasks.size() >= MAX_TASKS)
    {
        Serial.println("Max tasks reached");
        return false;
    }

    tasks.push_back(task);
    bool ret = writeTaskToFile(&task);
    closeFile();
    return ret;
}

bool Scheduler::removeTask(uint8_t id)
{
    int8_t index = -1;
    for (uint8_t i = 0; i < tasks.size(); i++)
    {
        if (tasks[i].id == id)
        {
            index = i;
            break;
        }
    }

    if (index < 0)
    {
        Serial.println("Task not found");
        return false;
    }

    tasks.erase(tasks.begin() + index);
    return save();
}

bool Scheduler::updateTask(uint8_t id, schedule_task_t task)
{
    for (uint8_t i = 0; i < tasks.size(); i++)
    {
        if (tasks[i].id == id)
        {
            tasks[i] = task;
            return save();
        }
    }
    Serial.println("Task not found");
    return false;
}

void Scheduler::updateTime()
{
    timeClient.update();
}

void Scheduler::run()
{
    uint8_t h = timeClient.getHours();
    uint8_t m = timeClient.getMinutes();
    uint8_t dow = timeClient.getDay(); // 0 is sunday
    bool anychange = false;

    for (uint8_t i = 0; i < tasks.size(); i++)
    {
        if (tasks[i].enabled)
        {
            if (tasks[i].repeat.monday && dow == 1  ||
                tasks[i].repeat.tuesday && dow == 2 ||
                tasks[i].repeat.wednesday && dow == 3 ||
                tasks[i].repeat.thursday && dow == 4  ||
                tasks[i].repeat.friday && dow == 5  ||
                tasks[i].repeat.saturday && dow == 6||
                tasks[i].repeat.sunday && dow == 0)
            {
                if (tasks[i].time.hour == h && tasks[i].time.minute == m && !tasks[i].executed)
                {
                    callback(tasks[i].id);
                    tasks[i].executed = true;
                    anychange = true;
                }
            }
        }
    }

    if (anychange)
    {
        save();
    }
}

uint8_t Scheduler::getTaskCount()
{
    return tasks.size();
}
