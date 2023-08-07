#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <string>
#include <TimeLib.h>

const int inputPin = 5;
unsigned long prevTime = 0;
volatile unsigned long pulseCount = 0;
volatile unsigned int rpm = 0;


// GPIO where the Sensor is connected to
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Const

const char* ssid = "HotSinglesInYourArea";
const char* password = "XA3FHAJEEFA6KKUGJE";
const char* serverUrl = "http://epo.pettinaro.xyz/api/sensorvalues?populate=*";
const char* token = "d283e6cce65b72ce0b486d224898fc8060c9052c9cc5c3c0619c4d7be47b2f6b7336431c1f3ab817b65c3849c234ea1e5c6bdc02c500a43dd46493ec6ef02890fb85c9f985d49b46334047cf9592337927b73c7eb6ec04e54673e8210d2858e444f4bb9d1e19fc2b91073f752c6352bab7fbc70f56ea62924a5701112f46f8c4";

WiFiClient client;
HTTPClient http;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void ICACHE_RAM_ATTR countPulse() {
  pulseCount++;  // Increment the pulse count whenever an interrupt is triggered
}

void setup() {
  timeClient.begin();
  Serial.begin(115200);
  prevTime = millis();
  pinMode(inputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPin), countPulse, RISING);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}


void myHttpRequest(char timeStamp[25], float value, int sensorId) {
 if (WiFi.status() == WL_CONNECTED) {
    http.begin(client, serverUrl);

    // Set headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(token));
    
    Serial.println(timeClient.getEpochTime());
    /*
    * ID: 2 = Strömung
    * ID: 5 = Temperatur
    */
    // Set request body
    String requestBody1 = "{\"data\":{\"value\":\"";
    String requestBody2 = "\",\"date\":\"";
    String requestBody3 = "\",\"sensor\":"+String(sensorId)+"}}";
    String requestBody = requestBody1 + String(value) + requestBody2 + String(timeStamp) + requestBody3;



    Serial.println(requestBody);
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}


void loop() {

  // ******** Temperature ******** //
  // Get Temperature
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  // ******** RMP ******** //
  noInterrupts();
  float rpm = (pulseCount * 60000) / (millis() - prevTime);
  interrupts();
  
  pulseCount = 0;
  prevTime = millis();

  // Get Timestamp
  timeClient.update();

 // Get the current time
  time_t currentTime = timeClient.getEpochTime();


  // Format the time as ISO 8601
  char isoTimestamp[25];
  snprintf(isoTimestamp, sizeof(isoTimestamp), "%04d-%02d-%02dT%02d:%02d:%02dZ",
           year(currentTime), month(currentTime), day(currentTime),
           hour(currentTime), minute(currentTime), second(currentTime));   


  myHttpRequest(isoTimestamp, tempC, 5);
  myHttpRequest(isoTimestamp, rpm, 2);
 

  delay(15000); // Send the request every 15 seconds
}





int main () {
  
}