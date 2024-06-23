#pragma once

#include "scheduler.hpp"

template<typename T>
Scheduler<T>::Scheduler(NTPClient *timeClient) {
    this->timeClient = timeClient;
    LittleFS.begin();

    // Load tasks from file
    load();
}

template<typename T>
Scheduler<T>::~Scheduler() {
    tasks.clear();
    closeFile();
    LittleFS.end();
}

template<typename T>
void Scheduler<T>::openFile(bool overwrite) {
    if (this->file && this->file.isFile()) {
        if (overwrite) {
            this->file.close();
            this->file = LittleFS.open(filePath, "w+");
        } else {
            return; // File already opened
        }
    }
    if (overwrite)
        this->file = LittleFS.open(filePath, "w+");
    else
        this->file = LittleFS.open(filePath, "a+");
}

template<typename T>
void Scheduler<T>::closeFile() {
    if (this->file) {
        this->file.close();
    }
}

template<typename T>
bool Scheduler<T>::writeTaskToFile(schedule_task_t<T> *task) {
    if (!file || !file.isFile()) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    file.seek(file.size());
    file.printf("%d|%d|%d|%d%d%d%d%d%d%d|%s|%d|%d\n",
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
                task->args->toString().c_str(),
                task->enabled,
                task->executed);

    return true;
}

template<typename T>
schedule_task_t<T> Scheduler<T>::parseTask(const String &task) {
    schedule_task_t<T> result{};
    result.id = 0;
    result.args = new T();
    uint8_t count = 0;
    int last_pos = 0;
    std::vector<uint8_t> values;
    String repeat = "";
    String args = "";

    // Task format: id|hour|minute|repeat|args|enabled|executed

    while ((unsigned int) last_pos < task.length()) {
        int pos = task.indexOf("|", last_pos);
        if (pos < 0) pos = task.length();

        // repeat part
        if (count == 3) {
            repeat = task.substring(last_pos, pos);
        }
        // args part
        else if (count == 4) {
            args = task.substring(last_pos, pos);
        }
        // other parts
        else {
            uint8_t num = (uint8_t) task.substring(last_pos, pos).toInt();
            values.push_back(num);
        }
        last_pos = pos + 1;
        ++count;
    }

    if (values.size() != 5 || repeat == "" || args == "")
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

    // Parse args
    result.args->parse(args);

    // Assign values
    result.id = values[0];
    result.time.hour = values[1];
    result.time.minute = values[2];
    result.enabled = values[3];
    result.executed = values[4];

    return result;
}

template<typename T>
void Scheduler<T>::setCallback(std::function<void(schedule_task_t<T>)> callback) {
    this->callback = std::move(callback);
}

template<typename T>
bool Scheduler<T>::load() {
    openFile();
    if (!file || !file.isFile()) {
        Serial.println("Failed to open file for reading");
        return false;
    }

    // Clear tasks
    tasks.clear();

    // Read file
    if (file.size() == 0) {
        closeFile();
        return true;
    }

    String line;
    while (file.available()) {
        line = file.readStringUntil('\n');
        if (line.length() > 0) {
            schedule_task_t task = parseTask(line);
            if (task.id)
                tasks.push_back(task);
        }
    }

    closeFile();
    return true;
}

template<typename T>
bool Scheduler<T>::save() {
    openFile(true);
    if (!file || !file.isFile()) {
        Serial.println("Failed to open file for writing");
        return false;
    }

    for (schedule_task_t task: tasks) {
        writeTaskToFile(&task);
    }

    closeFile();
    return true;
}

template<typename T>
bool Scheduler<T>::addTask(schedule_task_t<T> task) {
    if (tasks.size() >= MAX_TASKS) {
        Serial.println("Max tasks reached");
        return false;
    }

    tasks.push_back(task);
    openFile();
    bool ret = writeTaskToFile(&task);
    closeFile();
    return ret;
}

template<typename T>
bool Scheduler<T>::removeTask(uint8_t id) {
    int8_t index = -1;
    for (uint8_t i = 0; i < tasks.size(); i++) {
        if (tasks[i].id == id) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        Serial.println("Task not found");
        return false;
    }

    tasks.erase(tasks.begin() + index);
    return save();
}

template<typename T>
bool Scheduler<T>::updateTask(uint8_t id, schedule_task_t<T> task) {
    for (schedule_task_t t: tasks) {
        if (t.id == id) {
            t = task;
            return save();
        }
    }
    Serial.println("Task not found");
    return false;
}

template<typename T>
void Scheduler<T>::run() {
    uint8_t h = timeClient->getHours();
    uint8_t m = timeClient->getMinutes();
    uint8_t dow = timeClient->getDay(); // 0 is sunday
    bool anychange = false;

    for (auto &task: tasks) {
        if (task.enabled) {
            if ((task.repeat.monday && dow == 1) ||
                (task.repeat.tuesday && dow == 2) ||
                (task.repeat.wednesday && dow == 3) ||
                (task.repeat.thursday && dow == 4) ||
                (task.repeat.friday && dow == 5) ||
                (task.repeat.saturday && dow == 6) ||
                (task.repeat.sunday && dow == 0)) {
                if (task.time.hour == h && task.time.minute == m) {
                    if (!task.executed) {
                        task.executed = true;
                        anychange = true;
                        if (callback)
                            callback(task);
                    }
                } else if (task.executed) {
                    task.executed = false;
                    anychange = true;
                }
            }
        }
    }

    if (anychange) {
        save();
    }
}

template<typename T>
schedule_task_t<T> Scheduler<T>::getTaskById(uint8_t id) {
    for (schedule_task_t task: tasks) {
        if (task.id == id) {
            return task;
        }
    }
    return {};
}

template<typename T>
uint8_t Scheduler<T>::getTaskCount() {
    return tasks.size();
}

template<typename T>
String Scheduler<T>::getString() {
    openFile();
    String result = "";
    result = file.readString();
    closeFile();
    return result;
}

template<typename T>
void Scheduler<T>::printToSerial(Stream &stream) {
    stream.printf("Task count: %d\n\n", tasks.size());
    for (auto &task: tasks) {
        stream.printf("Task %d: %02d:%02d\nrepeat: %d|%d|%d|%d|%d|%d|%d\nargs: %s\nenabled: %d\nexecuted: %d\n\n",
                      task.id,
                      task.time.hour,
                      task.time.minute,
                      task.repeat.monday,
                      task.repeat.tuesday,
                      task.repeat.wednesday,
                      task.repeat.thursday,
                      task.repeat.friday,
                      task.repeat.saturday,
                      task.repeat.sunday,
                      task.args->toString().c_str(),
                      task.enabled,
                      task.executed);
    }

    Serial.println("======== Read from file ========");
    openFile();

    Serial.printf("File size: %d\n", file.size());
    while (file.available()) {
        Serial.print("[Task] ");
        Serial.println(file.readStringUntil('\n'));
    }

    Serial.println("=========== End file ===========");
    closeFile();
}