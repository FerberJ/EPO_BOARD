#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <string>
#include <TimeLib.h>

// GPIO where the Sensor is connected to
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

const char* ssid = "HotSinglesInYourArea";
const char* password = "XA3FHAJEEFA6KKUGJE";
const char* serverUrl = "http://epo.pettinaro.xyz/api/sensorvalues?populate=*";
const char* token = "d283e6cce65b72ce0b486d224898fc8060c9052c9cc5c3c0619c4d7be47b2f6b7336431c1f3ab817b65c3849c234ea1e5c6bdc02c500a43dd46493ec6ef02890fb85c9f985d49b46334047cf9592337927b73c7eb6ec04e54673e8210d2858e444f4bb9d1e19fc2b91073f752c6352bab7fbc70f56ea62924a5701112f46f8c4";

WiFiClient client;
HTTPClient http;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


void setup() {
  timeClient.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {

  // Get Temperature
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  // Get Timestamp
  timeClient.update();

 // Get the current time
  time_t currentTime = timeClient.getEpochTime();

  // Format the time as ISO 8601
  char isoTimestamp[25]; // ISO 8601 timestamp can be up to 24 characters
  snprintf(isoTimestamp, sizeof(isoTimestamp), "%04d-%02d-%02dT%02d:%02d:%02dZ",
           year(currentTime), month(currentTime), day(currentTime),
           hour(currentTime), minute(currentTime), second(currentTime));   



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
    String requestBody3 = "\",\"sensor\":5}}";
    String requestBody = requestBody1 + String(tempC) + requestBody2 + String(isoTimestamp) + requestBody3;



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

  delay(15000); // Send the request every 15 seconds
}

int main () {}