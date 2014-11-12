#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/wdt.h>

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.print (x)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#define DEBUG_WRITE(x)  Serial.write (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_WRITE(x)
#endif

// Code based on tutorial from http://www.raywenderlich.com/38841/arduino-tutorial-temperature-sensor

#define TEMPERATURE_SLEEPING_AREA 5
#define TEMPERATURE_CAGE 6

const int ledPin = 7;

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0xFB, 0x6C };

OneWire oneWireSleepingArea(TEMPERATURE_SLEEPING_AREA);
DallasTemperature sensorSleepingArea(&oneWireSleepingArea);

OneWire oneWireCage(TEMPERATURE_CAGE);
DallasTemperature sensorCage(&oneWireCage);

EthernetServer server(80);

void setup(void) {
  pinMode(ledPin, OUTPUT);

#ifdef DEBUG
  Serial.begin(9600);
#endif

  DEBUG_PRINTLN("LED Pin setup for output on pin");

  digitalWrite(ledPin, HIGH);

  sensorSleepingArea.begin();
  sensorCage.begin();

  // start the Ethernet connection and the server:
  DEBUG_PRINTLN("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    DEBUG_PRINTLN("Failed to configure Ethernet using DHCP");
    while (true) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }
  }

  // start listening for clients
  DEBUG_PRINT("server is at ");
  DEBUG_PRINTLN(Ethernet.localIP());

  digitalWrite(ledPin, LOW);
}


void loop(void) {

  float temperatureSleepingArea;
  float temperatureCage;

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    DEBUG_PRINTLN("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        DEBUG_WRITE(c);
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

          temperatureSleepingArea = readTemperatureCelsius(oneWireSleepingArea, sensorSleepingArea);
          temperatureCage = readTemperatureCelsius(oneWireCage, sensorCage);

          client.print("cage.value ");
          client.print(temperatureCage);
          client.println();
          client.print("bedroom.value ");
          client.print(temperatureSleepingArea);
          client.println();
          
          // Handle error situation after returning -127. This will both prevent the munin code from getting a
          // timeout and also log a -127 there so we can see how often it happens.
          if (temperatureSleepingArea < -100 || temperatureCage < -100) { // -127 is disconnected
            DEBUG_PRINTLN("Disconnected sensor detected - restarting");
    
            // Start a watchdog and then timeout to force a reset
            wdt_enable(WDTO_15MS);
            while(1)
            {
            }
          }

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
    DEBUG_PRINTLN("client disonnected");
    blinkLED();
  }

}

float readTemperatureCelsius(OneWire oneWire, DallasTemperature sensor) {
  sensor.requestTemperatures();
  float temperature = sensor.getTempCByIndex(0);

  DEBUG_PRINT("Celsius Temperature for device is: ");
  DEBUG_PRINTLN(temperature);  //zero is first sensor if we had multiple on bus
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

