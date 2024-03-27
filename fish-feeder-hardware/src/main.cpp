#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SimpleTimer.h>

#include <WebSocketsServer.h>

#include "FeedLog.h"
#include "Scheduler.h"
#include "Feeder.h"

#include "mainpage.h"


#define SERVO_PIN D1
#define SENSOR_PIN D2
#define BUTTON_PIN D0


WiFiUDP ntpUDP;
NTPClient timeClient = NTPClient(ntpUDP, "pool.ntp.org", 7 * 3600);
ESP8266WebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);

SimpleTimer timer;
FeedLog feedLog(&timeClient);
Scheduler scheduler(&timeClient);
Feeder feeder(SERVO_PIN);

void _webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void mainLoop();
void feed(uint8_t amount, std::function<void()> callback);

bool buttonState;
bool lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial)
    delay(10);

  /* ====== Wifi begin manualy ====== */
  // Serial.print("Connecting to WiFi...");
  // WiFi.begin("The Coffee House", "thecoffeehouse");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(10);
  //   Serial.print(".");
  // }
  // Serial.println("\nConnected to WiFi");

  /* ====== Wait for wifi connection ====== */
  WiFiManager wifiManager;

  // // Reset wifimanager
  // wifiManager.resetSettings();
  String hotspotName = "Fish Feeder_" + String(ESP.getChipId(), HEX);
  wifiManager.autoConnect(hotspotName.c_str());

  // Print local IP
  Serial.println(WiFi.localIP());

  /* ====== Web server listener ====== */

  server.on("/", []()
            { server.send(200, "text/html", MAINPAGE); });

  server.on("/logs", []()
            { server.send(200, "text/plain", feedLog.gets()); });

  server.on("/schedules", []()
            { server.send(200, "text/plain", scheduler.getString()); });

  server.on("/addtask", []()
            {
    schedule_task_t task;
    task.id = rand() % 100;
    task.time.hour = 21;
    task.time.minute = 0;
    task.repeat.monday = true;
    task.repeat.tuesday = true;
    task.repeat.wednesday = true;
    task.repeat.thursday = true;
    task.repeat.friday = true;
    task.repeat.saturday = true;
    task.repeat.sunday = true;
    task.amount = 3;
    task.enabled = true;
    task.executed = false;
    bool result = scheduler.addTask(task);

    if (!result)
    {
      server.send(500, "text/plain", "Failed to add task");
      return;
    }

    timer.setTimeout(2000, [](){
      scheduler.printToSerial(Serial);
    });

    server.send(200, "text/plain", "Task added " + String(task.id)); });

  server.onNotFound([]()
                    { server.send(404, "text/plain", "404: Not found"); });

  /* ====== Start services ====== */

  server.begin();
  timeClient.begin();

  webSocket.begin();
  webSocket.onEvent(_webSocketEvent);

  timer.setTimeout(1000, []()
                   {
    Serial.println("======== Feed log ========");
    Serial.println(feedLog.gets().c_str());
    Serial.println();
    Serial.println();
    Serial.println("======== Scheduler ========");
    scheduler.printToSerial(Serial); });

  timer.setInterval(300, mainLoop);
}

void loop()
{
  timer.run();
  server.handleClient();
  webSocket.loop();
  feeder.run();

  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        feed(30, [](){
          Serial.println("Feeded");
        });
      }
    }
  }


  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void _webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
    // send message to client
    webSocket.sendTXT(num, "Connected");
  }
  break;
  case WStype_TEXT:
    Serial.printf("[%u] get Text: %s\n", num, payload);
    String message = String((char *)(payload));
    if (message.startsWith("#FEED"))
    {
      // #FEED <n>
      webSocket.broadcastTXT("FEEDING");
      feed(
          message.substring(6).toInt(),
          []()
          {
            webSocket.broadcastTXT("FEEDED");
          });
    }
    if (message.startsWith("#TASK"))
    {
      // #TASK <id>|<hour>|<minute>|<repeat>|<amount>|<enabled>
      schedule_task_t task = scheduler.parseTask(message.substring(6));
      if (task.id == 0)
      {
        webSocket.sendTXT(num, "Invalid task format");
        break;
      }
      bool result = scheduler.addTask(task);
      if (!result)
      {
        webSocket.sendTXT(num, "Failed to add task");
        break;
      }
      uint8_t* ret = (uint8_t*)("Task added " + String(task.id)).c_str();
      webSocket.sendTXT(num, ret, sizeof(ret));
    }
    break;
  }
}

void feed(uint8_t amount, std::function<void()> callback)
{
  feeder.feed(amount, callback);
  feedLog.add(amount);
}

void mainLoop()
{
  scheduler.run();
}