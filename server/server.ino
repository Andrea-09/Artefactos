// Import required libraries
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

#include <DHT.h>
 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN D3
int fanOne = D4;
int fanTwo = D5;
int soilOne = D6;
int soilTwo = D7;


// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Set your access point network credentials
#define ssid "Rodriguez"
#define password "pimienta8"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

float t, h;
int fans, s1, s2;

String readTemp() {
  return String(t);
  //return String(1.8 * bme.readTemperature() + 32);
}

String readFanOne() {
  return String(fans);
}

String readSoilOne() {
  return String(s1);
}

String readSoilTwo() {
  return String(s2);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();

  pinMode(fanOne, OUTPUT);
  pinMode(fanTwo, OUTPUT);
  pinMode(soilOne, INPUT);
  pinMode(soilTwo, INPUT);

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

  server.on("/fans", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readFanOne().c_str());
  });

   server.on("/soilOne", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readSoilOne().c_str());
  });

  server.on("/soilTwo", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readSoilTwo().c_str());
  });
  
  // Start server
  server.begin();
}
 
void loop(){

  t = dht.readTemperature();
  h = dht.readHumidity();

  if(t >= 30.00){
    digitalWrite(fanOne, LOW);
    digitalWrite(fanTwo, LOW);
    fans = 1;  
  }
  else {
    digitalWrite(fanOne, HIGH);
    digitalWrite(fanTwo, HIGH);
    fans = 0;
  }

  Serial.println("Temperatura: ");
  Serial.println(t);

  delay(2000);

  int soilWet = digitalRead(soilOne);
  int soilWet2 = digitalRead(soilTwo);

  if (soilWet == 1) {
    Serial.println("El suelo de la maceta 1 se encuentra seco");
    s1 = 0;
  }
  else {
    Serial.println("El suelo de la maceta 1 se encuentra humedo");
    s1 = 1;
  }

  delay(3000);


  if (soilWet2 == 1) {
    Serial.println("El suelo de la maceta 2 se encuentra seco");
    s2 = 0;
  }
  else {
    Serial.println("El suelo de la maceta 2 se encuentra humedo");
    s2 = 1;
  }

  delay(3000);
 
}
