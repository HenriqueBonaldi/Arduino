# Arduino and ESP8266 chip 

Solution utilizing Arduino IDE and ESP8266 chip to track x,y,z coordinates in real time feeding data to Google Sheets

## 🛠️ Registration

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

