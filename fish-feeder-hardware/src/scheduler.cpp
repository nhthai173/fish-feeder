#include "scheduler.h"

Scheduler::Scheduler(NTPClient *timeClient)
{
    this->timeClient = timeClient;
    LittleFS.begin();

    // Load tasks from file
    load();
}

Scheduler::~Scheduler()
{
    tasks.clear();
    closeFile();
    LittleFS.end();
}

void Scheduler::openFile()
{
    if (this->file.isFile())
    {
        this->file.close();
    }
    this->file = LittleFS.open(filePath, "a+");
}

void Scheduler::closeFile()
{
    if (this->file)
    {
        this->file.close();
    }
}

bool Scheduler::writeTaskToFile(schedule_task_t *task)
{
    openFile();
    if (!file || !file.isFile())
    {
        Serial.println("Failed to open file for writing");
        return false;
    }
    
    file.printf("%d|%d|%d|%d%d%d%d%d%d%d|%d|%d|%d\n",
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
    result.id = 0;
    uint8_t count = 0;
    int last_pos = 0;
    std::vector<uint8_t> values;
    String repeat;

    while((unsigned int)last_pos < task.length())
    {
        int pos = task.indexOf("|", last_pos);
        if (pos < 0) pos = task.length();
        uint8_t num = (uint8_t)task.substring(last_pos, pos).toInt();
        values.push_back(num);
        // Assign string to repeat
        if (count == 3)
            repeat = task.substring(last_pos, pos);
        last_pos = pos + 1;
        ++count;
    }

    if (values.size() != 7) // id-hour-minute-repeat-amount-enabled-executed
    {
        Serial.println("Invalid task format");
        return result;
    }

    // Parse repeat days
    result.repeat.monday = repeat.charAt(0) == '1';
    result.repeat.tuesday = repeat.charAt(1) == '1';
    result.repeat.wednesday = repeat.charAt(2) == '1';
    result.repeat.thursday = repeat.charAt(3) == '1';
    result.repeat.friday = repeat.charAt(4) == '1';
    result.repeat.saturday = repeat.charAt(5) == '1';
    result.repeat.sunday = repeat.charAt(6) == '1';

    // Assign values
    result.id = values[0];
    result.time.hour = values[1];
    result.time.minute = values[2];
    result.amount = values[4];
    result.enabled = values[5];
    result.executed = values[6];

    return result;
}

void Scheduler::setCallback(std::function<void(uint8_t)> callback)
{
    this->callback = callback;
}

bool Scheduler::load()
{
    openFile();
    if (!file || !file.isFile())
    {
        Serial.println("Failed to open file for reading");
        return false;
    }

    // Clear tasks
    tasks.clear();

    // Read file
    if (file.size() == 0)
    {
        closeFile();
        return true;
    }

    String line;
    while (file.available())
    {
        line = file.readStringUntil('\n');
        if (line.length() > 0)
        {
            schedule_task_t task = parseTask(line);
            if (task.id)
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
    timeClient->update();
}

void Scheduler::run()
{
    uint8_t h = timeClient->getHours();
    uint8_t m = timeClient->getMinutes();
    uint8_t dow = timeClient->getDay(); // 0 is sunday
    bool anychange = false;

    for (uint8_t i = 0; i < tasks.size(); i++)
    {
        if (tasks[i].enabled)
        {
            if ((tasks[i].repeat.monday && dow == 1)    ||
                (tasks[i].repeat.tuesday && dow == 2)   ||
                (tasks[i].repeat.wednesday && dow == 3) ||
                (tasks[i].repeat.thursday && dow == 4)  ||
                (tasks[i].repeat.friday && dow == 5)    ||
                (tasks[i].repeat.saturday && dow == 6)  ||
                (tasks[i].repeat.sunday && dow == 0))
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

String Scheduler::getString()
{
    openFile();
    String result = "";
    result = file.readString();
    closeFile();
    return result;
}

void Scheduler::printToSerial(Stream &stream)
{
    stream.printf("Task count: %d\n\n", tasks.size());
    for (uint8_t i = 0; i < tasks.size(); i++)
    {
        stream.printf("Task %d: %02d:%02d\nrepeat: %d|%d|%d|%d|%d|%d|%d\namount: %d\nenabled: %d\nexecuted: %d\n\n",
                      tasks[i].id,
                      tasks[i].time.hour,
                      tasks[i].time.minute,
                      tasks[i].repeat.monday,
                      tasks[i].repeat.tuesday,
                      tasks[i].repeat.wednesday,
                      tasks[i].repeat.thursday,
                      tasks[i].repeat.friday,
                      tasks[i].repeat.saturday,
                      tasks[i].repeat.sunday,
                      tasks[i].amount,
                      tasks[i].enabled,
                      tasks[i].executed);
    }

    Serial.println("======== Read from file ========");
    openFile();

    Serial.printf("File size: %d\n", file.size());
    while (file.available())
    {
        Serial.print("[Task] ");
        Serial.println(file.readStringUntil('\n'));
    }

    Serial.println("=========== End file ===========");
    closeFile();
}