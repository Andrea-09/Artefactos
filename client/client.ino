/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-client-server-wi-fi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;


#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Andrea09"
#define AIO_KEY         "aio_raTe16en52hkOHnfFQ2KBpdZPzun"


#define ssid "Pollo Campero"
#define password "Andrita-09"

//Your IP address or domain name with URL path
const char* serverNameTemp = "http://192.168.127.176/temperature";
const char* serverNameHum = "http://192.168.127.176/humidity";
const char* serverNameId = "http://192.168.127.176/id";

String temperature;
String humidity;
String id;

unsigned long previousMillis = 0;
const long interval = 5000; 

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

// Setup a feed called 'photocell' for publishing.
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pruebaA");
// Setup a feed called 'photocell' for publishing.
Adafruit_MQTT_Publish humedad = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pruebaH");
Adafruit_MQTT_Publish id = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/rfid");

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // Address 0x3C for 128x64, you might need to change this value (use an I2C scanner)
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
}

void loop() {
  //MQTT_connect();
 // mqtt.processPackets(10000);
  //if(! mqtt.ping()) {   // ping the server to keep the mqtt connection alive
   // mqtt.disconnect();}
  
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
     // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      temperature = httpGETRequest(serverNameTemp);
      humidity = httpGETRequest(serverNameHum);
      id = httpGETRequest(serverNameHumId);
      Serial.println("Temperature: " + temperature);
      Serial.println("Humedad: " + humidity);
      Serial.println("Humedad: " + id);
      
      temp.publish(temperature.c_str());
      humedad.publish(humidity.c_str());
      id.publish(id.c_str());

      
      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;}
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {       // basically die and wait for WDT to reset me
         while (1);}}
  Serial.println("MQTT Connected!");
}
