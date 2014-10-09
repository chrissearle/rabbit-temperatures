#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Code based on tutorial from http://www.raywenderlich.com/38841/arduino-tutorial-temperature-sensor

#define TEMPERATURE_SLEEPING_AREA 5
#define TEMPERATURE_CAGE 6

const int ledPin = 7;

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xFB, 0x6C };

OneWire oneWireSleepingArea(TEMPERATURE_SLEEPING_AREA);
DallasTemperature sensorSleepingArea(&oneWireSleepingArea);

OneWire oneWireCage(TEMPERATURE_CAGE);
DallasTemperature sensorCage(&oneWireCage);

EthernetServer server(80);

void setup(void) {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("LED Pin setup for output on pin");
  
  digitalWrite(ledPin, HIGH);
  
  sensorSleepingArea.begin();
  sensorCage.begin();
  
  // start the Ethernet connection and the server:
  Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while (true) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }
  }
  Serial.println();
  // start listening for clients
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  digitalWrite(ledPin, LOW);
  
}


void loop(void) {

  float temperatureSleepingArea;
  float temperatureCage;
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
          
          temperatureSleepingArea = readTemperatureCelsius(sensorSleepingArea);
          temperatureCage = readTemperatureCelsius(sensorCage);
          
          client.print("cage.value: ");
          client.print(temperatureCage);
          client.println();
          client.print("bedroom.value: ");
          client.print(temperatureSleepingArea);
          client.println();
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
    blinkLED();
  }

}

float readTemperatureCelsius(DallasTemperature sensor) {
  sensor.requestTemperatures();
  float temperature = sensor.getTempCByIndex(0);
  //to read in Fahrenheit, we use Celsius for one form
  //float temperature = sensorsIndoor.getTempFByIndex(0);  
  Serial.print("Celsius Temperature for device is: ");
  Serial.println(temperature);  //zero is first sensor if we had multiple on bus
  return temperature;
}

void blinkLED(void)
{
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
  
  return;
}
