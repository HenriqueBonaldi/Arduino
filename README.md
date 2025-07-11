# Arduino and ESP8266 chip 

Solution utilizing Arduino and ESP8266 chips to track coordinates in the applicable object in real time feeding data to Google Sheets

## 🛠️ Installation

```bash
#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <MPU6050.h>
#include "I2Cdev.h"
```

\## 🛠️ Registration

```
// Replace with your network credentials
const char\* ssid = "VIVOFIBRA-xxxx";
const char\* password = "xxxxxxx";

// Router mobile network
//const char\* ssid = "Galaxy A21sxxxx";
//const char\* password = "xxxxxx";

// Connect ESP
const char\* apSSID = "ESP8266";
const char\* apPassword = "xxxxxx";

// Your Google Sheets URL
const char\* scriptUrl = "https://script.google.com/macros/s/xxxxx/exec";
```

## 🛠️ Google Sheets code

```
function doPost(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var data = JSON.parse(e.postData.contents);

  var dataArray = data.data.split(",");
  var rowData = \[new Date(), dataArray\[0]];

  sheet.appendRow(rowData);
  return ContentService.createTextOutput("Data added successfully");
}
```

