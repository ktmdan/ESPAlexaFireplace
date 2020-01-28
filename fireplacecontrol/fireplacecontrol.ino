
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

#define WIFI_SSID "" //Put your WIFI SSDI here
#define WIFI_PASS "" //Put your password here
#define RELAY 0 // relay connected to  GPIO0
#define device1 "fireplace"

fauxmoESP fauxmo;

void wifi()
{
  WiFi.mode(WIFI_STA);
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);


  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");

    delay(100);
  }
  Serial.print(".");
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}


void faux()
{
  fauxmo.createServer(true);
  fauxmo.setPort(80);

  fauxmo.enable(true);
  fauxmo.addDevice(device1);
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    // Callback when a command from Alexa is received.
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.

    Serial.printf("[MAIN](%s) state: %s\n", device_name, state ? "ON" : "OFF");
    if (strcmp(device_name, device1) == 0)
    {
      Serial.println("RELAY 1 switched by Alexa");

      if (state) {
        Serial.print("1");
        digitalWrite(RELAY, LOW);

      } else {
        Serial.print("2");
        digitalWrite(RELAY, HIGH);

      }
    }
  });



}

void setup()
{
  Serial.begin(115200);
  Serial.println("STARTING");
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  wifi();
  Serial.println("FAUX START");
  faux();
  Serial.println("SETUP DONE");
}

void loop()
{
  fauxmo.handle();
  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }
}
