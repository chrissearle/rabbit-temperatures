#include <ESP8266WiFi.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "config.h"

unsigned long myChannelNumber = THINGSPEAK_CHANNEL;
String myWriteAPIKey = THINGSPEAK_API;

DeviceAddress CageProbe = CAGE_SENSOR; 
DeviceAddress BedroomProbe = BEDROOM_SENSOR;

OneWire oneWire(BUS_PIN);
DallasTemperature sensors(&oneWire);

void setup(void) {
  pinMode(STATUS_LED_PIN, OUTPUT);

  Serial.begin(115200);

  digitalWrite(STATUS_LED_PIN, HIGH);

  Serial.println();
  Serial.println("Connecting");

  WiFiManager wifiManager;
  wifiManager.autoConnect("RabbitsWemosAP");

  Serial.println();
  Serial.println("Connected");

  digitalWrite(STATUS_LED_PIN, LOW);

  sensors.begin();
  sensors.setResolution(CageProbe, 12);
  sensors.setResolution(BedroomProbe, 12);
}


void loop(void) {
  blinkLED();
  
  float temperatureSleepingArea;
  float temperatureCage;

  sensors.requestTemperatures();

  temperatureSleepingArea = readTemperatureCelsius(BedroomProbe);
  temperatureCage = readTemperatureCelsius(CageProbe);

  Serial.print("Sleep ");
  Serial.println(temperatureSleepingArea);
  Serial.print("Cage ");
  Serial.println(temperatureCage);

  sendTemperatureTS(temperatureSleepingArea, temperatureCage);

  delay(1000 * 60 * 5);
}

float readTemperatureCelsius(DeviceAddress probe) {
  float temperature = sensors.getTempC(probe);

  return temperature;
}

void blinkLED(void)
{
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(500);
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(500);

  return;
}

void sendTemperatureTS(float temp1, float temp2)
{
  WiFiClient client;
  
  if (client.connect("api.thingspeak.com", 80)) {
    Serial.println("WiFi Client connected ");
   
    String postStr = myWriteAPIKey;
    postStr += "&field1=";
    postStr += String(temp1);
    postStr += "&field2=";
    postStr += String(temp2);
    postStr += "\r\n\r\n";
   
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + myWriteAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
   }
   
   client.stop();
}
