#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "<WLAN>";
const char* password = "<PASSWORT>";
const char* serverUrl = "http://192.168.0.4:1337/api/sensors";
const char* token = "9e03409b927b496033ad3421b8957314a1bc378a1a6b351ad4e7796b35297142103cdd85eb079e0f261c9cc583bb3b9427cd7b94a6318b1ee64be0dad0e0945ee874526999c01a53b75af459589ead8fceb4c1c667e02210752bb1b6ecd948dc7f58b89c0aba82fd798cf9c7b1312392b007d0e9102a834f6047db8898a765c3";

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    http.begin(client, serverUrl); // Use ::begin(WiFiClient, url) instead

    // Set headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(token));

    // Set request body
    String requestBody = "{\"data\":{\"Value\":\"99.99\",\"Name\":\"value2\"}}";
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
  //ESP.deepSleep(15e6); // 15 sekunden
}

int main () {}