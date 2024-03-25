#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>

#include "mainpage.h"

ESP8266WebServer server(80);

void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);

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
  wifiManager.autoConnect();

  // Print local IP
  Serial.println(WiFi.localIP());

  /* ====== Web server listener ====== */

  server.on("/", []()
            { server.send(200, "text/html", MAINPAGE); });

  server.onNotFound([]()
                    { server.send(404, "text/plain", "404: Not found"); });

  /* ====== Start web server ====== */
  server.begin();
}



void loop()
{
  server.handleClient();
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}