#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
//#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <SimpleTimer.h>

#include <WebSocketsServer.h>

#include "FeedLog.h"
#include "FeedScheduler.h"
#include "Feeder.h"

#include "mainpage.h"

#define SERVO_PIN D1
#define SENSOR_PIN D2
#define BUTTON_PIN D3

#define AUTH_TOKEN "123456789"

WiFiUDP ntpUDP;
NTPClient timeClient = NTPClient(ntpUDP, "pool.ntp.org", 7 * 3600);
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

SimpleTimer timer;
FeedLog feedLog(&timeClient);
Scheduler<FeedTaskArgs> scheduler(&timeClient);
Feeder feeder(SERVO_PIN);

bool auth_process(ESP8266WebServer *sv);
void wsEventHandler(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void feed(uint8_t amount, std::function<void()> callback);
void feedAutomatically(schedule_task_t<FeedTaskArgs> task);

bool buttonState = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

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

  // Reset wifimanager
  // WiFi.disconnect();
  // wifiManager.resetSettings();
  // ESP.eraseConfig();
  // ESP.reset();
  // ESP.restart();

  wifiManager.autoConnect(String("Fish Feeder_" + String(ESP.getChipId(), HEX)).c_str());

  // Print local IP
  Serial.println(WiFi.localIP());

  /* ====== Web server listener ====== */

  server.on("/", []()
            { server.send(200, "text/html", MAINPAGE); });

  server.on("/logs", []()
            { server.send(200, "text/plain", feedLog.gets()); });

  server.on("/schedules", []()
            { server.send(200, "text/plain", scheduler.getString()); });

  server.on("/testauth", [](){
      if (!auth_process(&server))
          return;
        server.send(200, "text/plain", "Authenticated");
  });

  server.onNotFound([]()
                    { server.send(404, "text/plain", "404: Not found"); });

//  MDNS.begin("feeder");
//  MDNS.addService("http", "tcp", 80);
//  MDNS.addService("ws", "tcp", 81);

  /* ====== Start services ====== */

  server.begin();
  timeClient.begin();

  webSocket.begin();
  webSocket.onEvent(wsEventHandler);

  scheduler.setCallback(feedAutomatically);

  timer.setTimeout(1000, []()
                   {
    Serial.println("======== Feed log ========");
    Serial.println(feedLog.gets().c_str());
    Serial.println();
    Serial.println();
    Serial.println("======== Scheduler ========");
    scheduler.printToSerial(Serial); });

  timer.setInterval(1000, []()
                    {
                      timeClient.update();
                      scheduler.run(); });
}

void loop()
{
  timer.run();
  server.handleClient();
  webSocket.loop();
//  MDNS.update();
  feeder.run();

  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;
      Serial.printf("Button state: %d\n", buttonState);
      if (buttonState == LOW)
      {
        feed(2, []()
             { Serial.println("Feeded via button"); });
      }
    }
  }
  lastButtonState = reading;
}

bool auth_process(ESP8266WebServer *sv)
{
    String authStr = sv->header("Authorization");
    if (authStr.length() == 0)
    {
        sv->send(401, "text/plain", "Authentication required");
        return false;
    }

    if (authStr == "Bearer " + String(AUTH_TOKEN))
        return true;

    sv->send(401, "text/plain", "Unauthorized");
    return false;
}

void wsEventHandler(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {

  /* === Client disconnected === */
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;

  /* === New client connected === */
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
    webSocket.sendTXT(num, "Connected");
    break;
  }

  case WStype_TEXT:
    Serial.printf("[%u] get Text: %s\n", num, payload);
    String message = String((char *)(payload));

    /* === Feed command === */
    if (message.startsWith("#FEED"))
    {
      // #FEED <amount>
      // Respond: FEEDING or FEEDED
      webSocket.broadcastTXT("FEEDING");
      feed(
          message.substring(6).toInt(),
          []()
          {
            webSocket.broadcastTXT("FEEDED");
          });
    }

    /* === Add task command === */
    if (message.startsWith("#TASK"))
    {
      // #TASK <id>|<hour>|<minute>|<repeat>|<args>|<enabled>|<excuted>
      // Respond: TASK_ADDED_<id> or TASK_FAILED
      auto task = Scheduler<FeedTaskArgs>::parseTask(message.substring(6));
      if (task.id == 0)
      {
        webSocket.sendTXT(num, "TASK_FAILED invalid format");
        break;
      }
      bool result = scheduler.addTask(task);
      if (!result)
      {
        webSocket.sendTXT(num, "TASK_FAILED failed to add task");
        break;
      }
      webSocket.broadcastTXT(String("TASK_ADDED_" + String(task.id)).c_str());
    }

    /* === Remove task command === */
    if (message.startsWith("#RMTASK"))
    {
      // #RMTASK <id>
      // Respond: TASK_REMOVED_<id> or RMTASK_FAILED
      uint8_t id = message.substring(8).toInt();
      if (id <= 0)
      {
        webSocket.sendTXT(num, "RMTASK_FAILED invalid id");
        break;
      }
      bool result = scheduler.removeTask(id);
      if (!result)
      {
        webSocket.sendTXT(num, "RMTASK_FAILED failed to remove task");
        break;
      }
      webSocket.broadcastTXT(String("TASK_REMOVED_" + String(id)).c_str());
    }

    break;
  }
}

void feed(uint8_t amount, std::function<void()> callback)
{
  feeder.feed(amount, callback);
  feedLog.add(amount);
}

void feedAutomatically(schedule_task_t<FeedTaskArgs> task)
{
  feed(task.args->amount, [task]()
       { Serial.printf("Feeded automatically %dg\n", task.args->amount); });
}