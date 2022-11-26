/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-client-server-wi-fi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

#include <DHT.h>
 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN D2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Set your access point network credentials
#define ssid "Pollo Campero"
#define password "Andrita-09"

/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

float t, h;

String readTemp() {
  return String(t);
  //return String(1.8 * bme.readTemperature() + 32);
}

String readH() {
  return String(h);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);

  dht.begin();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".....");
  }

  Serial.println(WiFi.localIP());

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemp().c_str());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readH().c_str());
  });
  
  // Start server
  server.begin();
}
 
void loop(){

  t = dht.readTemperature();
  h = dht.readHumidity();
  
}
