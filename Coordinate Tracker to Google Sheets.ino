
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <WiFiClientSecureBearSSL.h>

#include <Wire.h>
#include <MPU6050.h>

#include "I2Cdev.h"

// Replace with your network credentials
const char* ssid = "VIVOFIBRA-xxxx";
const char* password = "xxxxxxx";

// Router mobile network
//const char* ssid = "Galaxy A21sxxxx";
//const char* password = "xxxxxx";

// Connect ESP
const char* apSSID = "ESP8266";
const char* apPassword = "xxxxxx";

const char* scriptUrl = "https://script.google.com/macros/s/xxxxx/exec";

// Create an instance of the server
ESP8266WebServer server(80);

MPU6050 mpu;
bool run = false; // power control
bool off = false; // output control "programa desligado" 

int16_t ax,ay,az;         
int16_t gx,gy,gz;     

//StaticJsonDocument<64> payloadDoc;

#define INTERRUPT_PIN 15 // use pin 15 on ESP8266

void handleRoot() {
  server.send(200, "text/html",
    "<!DOCTYPE html>\
    <html>\
    <head>\
      <title>BEM-VINDO</title>\
    </head>\
    <body>\
      <h1>BEM-VINDO</h1>\
      <button onclick=\"funcStart()\">START</button>\
      <p id=\"response1\"></p>\
      <script>\
        function funcStart() {\
          var xhr = new XMLHttpRequest();\
          xhr.onreadystatechange = function() {\
            if (xhr.readyState === XMLHttpRequest.DONE) {\
              if (xhr.status === 200) {\
                document.getElementById(\"response1\").textContent = xhr.responseText;\
              } else {\
                document.getElementById(\"response1\").textContent = \"Error: \" + xhr.status;\
              }\
            }\
          };\
          xhr.open(\"GET\", \"/start\", true);\
          xhr.send();\
        }\
      </script>\
      <button onclick=\"funcStop()\">STOP</button>\
      <p id=\"response2\"></p>\
      <script>\
        function funcStop() {\
          var xhr = new XMLHttpRequest();\
          xhr.onreadystatechange = function() {\
            if (xhr.readyState === XMLHttpRequest.DONE) {\
              if (xhr.status === 200) {\
                document.getElementById(\"response2\").textContent = xhr.responseText;\
              } else {\
                document.getElementById(\"response2\").textContent = \"Error: \" + xhr.status;\
              }\
            }\
          };\
          xhr.open(\"GET\", \"/stop\", true);\
          xhr.send();\
        }\
      </script>\
    </body>\
    </html>");
}

void handleStart() {
  run = true;
  off = false;

  digitalWrite(INTERRUPT_PIN, run);
  server.send(200, "text/plain", "Adquirindo dados e plotando no excel");
}

void handleStop() {
  run = false;

  digitalWrite(INTERRUPT_PIN, run);
  server.send(200, "text/plain", "Parando a execução");
}

void mpu_setup(){
  // Starting MPU module
  Wire.begin();
  
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  //Serial.println("Testing device connections...");
  //Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
}

void setup() {
 // Start serial communication
  Serial.begin(9600);

  // Connect to existing Wi-Fi network
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print the connected network's IP address
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  // Configure and start the access point
  WiFi.softAP(apSSID, apPassword);

  // Print the access point's IP address
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(apIP);

  // Set up server handlers
  server.on("/", handleRoot);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);

  // Start the server
  server.begin();
  Serial.println("Server started");

  mpu_setup();
}

void loop() {
  // Handle client requests
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();

    //Serial.print("[HTTP] Connecting to script URL: ");
    //Serial.println(scriptUrl);
 
    if (http.begin(client, scriptUrl) && !off) {
      http.addHeader("Content-Type", "application/json");

      String payload;

      // Prepare data to send to Google Sheets
      if(run) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
       
        payload = "{\"data\": \"" + String(ax) + "," + String(ay) + "," + String(az) + "," + String(gx) + "," + String(gy) + "," + String(gz) + "\"}";
      } 
      else {
        payload = "{\"data\": \"Programa desligado\" }";
        off = true;
      }
      
      Serial.print("[HTTP] Sending data: ");
      Serial.println(payload);

      int httpResponseCode = http.POST(payload);
      
      if (httpResponseCode > 0) {
        Serial.print("[HTTP] POST request sent successfully. Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("[HTTP] POST request failed. Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } 
    else {
      //Serial.println("[HTTP] Waiting for start signal OR Unable to connect  ");
    }
  } 
  else {
    Serial.println("WiFi connection lost");
  }

  delay(100); // Wait before sending the next request
}
  