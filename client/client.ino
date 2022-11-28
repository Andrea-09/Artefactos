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
#include <SPI.h>      
#include <MFRC522.h>
#include "DHT.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;


#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Bryan31542"
#define AIO_KEY         "aio_sodp02QtjKMcH0krgjpDH3GHnZIz"


#define ssid "Rodriguez"
#define password "pimienta8"

//Your IP address or domain name with URL path
const char* serverNameTemp = "http://192.168.1.27/temperature";
const char* serverNameHum = "http://192.168.1.22/humidity";
const char* serverNameId = "http://192.168.1.22/id";
const char* serverNameS1 = "http://192.168.1.22/soilOne";
const char* serverNameS2 = "http://192.168.1.22/soilTwo";

String temperature;
String humidity;
String id;
String soilOne;
String soilTwo;

unsigned long previousMillis = 0;
const long interval = 5000;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");
//Adafruit_MQTT_Publish humedad = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pruebaH");
//Adafruit_MQTT_Publish ide = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/rfid");
//Adafruit_MQTT_Publish s1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil1");
//Adafruit_MQTT_Publish s2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil2");

#define RST_PIN  D1      // constante para referenciar pin de reset
#define SS_PIN  D2      // constante para referenciar pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte LecturaUID[4];         // crea array para almacenar el UID leido
byte Usuario1[4]= {0x2A, 0xB9, 0xC9, 0x25} ;    // UID de tarjeta leido en programa 1
byte Usuario2[4]= {0xA2, 0x25, 0xE6, 0x1C} ;    // UID de llavero leido en programa 1

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  Serial.println();
  SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init();     // inicializa modulo lector
  Serial.println("Listo");
 
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
  MQTT_connect();
  mqtt.processPackets(10000);
  if(! mqtt.ping()) {   // ping the server to keep the mqtt connection alive
  mqtt.disconnect();}
 
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
     // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      read_RFID();
      temperature = httpGETRequest(serverNameTemp);
      humidity = httpGETRequest(serverNameHum);
      id = httpGETRequest(serverNameId);
      soilOne = httpGETRequest(serverNameS1);
      soilTwo = httpGETRequest(serverNameS2);
     
      Serial.println("Temperature: " + temperature);
      Serial.println("Humedad: " + humidity);
      Serial.println("ID: " + id);
      Serial.println("Maceta 1: " + soilOne);
      Serial.println("Maceta 2: " + soilTwo);
     
      temp.publish(temperature.c_str());
      //humedad.publish(humidity.c_str());
      //ide .publish(id.c_str());
      //s1.publish(soilOne.c_str());
      //s2.publish(soilTwo.c_str());

     
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
    return;
   }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(3000);  // wait 10 seconds
       retries--;
       if (retries == 0) {       // basically die and wait for WDT to reset me
         while (1);}}
  Serial.println("MQTT Connected!");
}

void read_RFID() {
  if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
    return;           // retorna al loop esperando por una tarjeta
 
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
    return;           // retorna al loop esperando por otra tarjeta
   
    Serial.print("UID:");       // muestra texto UID:
    for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID
      if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
        Serial.print(" 0");       // imprime espacio en blanco y numero cero
        }
        else{           // sino
          Serial.print(" ");        // imprime un espacio en blanco
          }
          Serial.print(mfrc522.uid.uidByte[i], HEX);    // imprime el byte del UID leido en hexadecimal
          LecturaUID[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del UID leido      
          }
         
          Serial.print("\t");         // imprime un espacio de tabulacion            
                   
          if(comparaUID(LecturaUID, Usuario1))    // llama a funcion comparaUID con Usuario1
            Serial.println("Bienvenido Usuario 1"); // si retorna verdadero muestra texto bienvenida
          else if(comparaUID(LecturaUID, Usuario2)) // llama a funcion comparaUID con Usuario2
            Serial.println("Bienvenido Usuario 2"); // si retorna verdadero muestra texto bienvenida
           else           // si retorna falso
            Serial.println("No te conozco");    // muestra texto equivalente a acceso denegado          
                 
                  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta                
}

boolean comparaUID(byte lectura[],byte usuario[]) // funcion comparaUID
{
  for (byte i=0; i < mfrc522.uid.size; i++){    // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
    return(false);          // retorna falso
  }
  return(true);           // si los 4 bytes coinciden retorna verdadero
}
