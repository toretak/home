#include <ESP8266WiFi.h>
#include <OneWire.h> 
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

Adafruit_AM2320 am2320 = Adafruit_AM2320(&Wire);


const char* hostGet = "192.168.2.2";
const String sensor_id = "1";
#define wifi_ssid "***"
#define wifi_password "***"


void setup() {
  Wire.pins(0, 2);
  Wire.begin(0, 2);
  // pinMode(2, INPUT_PULLUP);
  Serial.begin(115200);
  am2320.begin();
}


void postData(String temp, String hum) {

  WiFiClient clientGet;
  const int httpGetPort = 80;

  //the path and file to send the data to:
  String urlGet = "/api/sensor-data";
  urlGet += "?sensor=" + sensor_id + "&value=" + temp + "&humidity=" + hum;


  if (clientGet.connect(hostGet, httpGetPort)) {
    clientGet.println("GET " + urlGet + " HTTP/1.1");
    clientGet.print("Host: ");
    clientGet.println(hostGet);
    clientGet.println("User-Agent: ESP8266/1.0");
    clientGet.println("Connection: close\r\n\r\n");

    unsigned long timeoutP = millis();
    while (clientGet.available() == 0) {

      if (millis() - timeoutP > 10000) {
        clientGet.stop();
        return;
      }
    }

    //just checks the 1st line of the server response. Could be expanded if needed.
    while (clientGet.available()) {
      String retLine = clientGet.readStringUntil('\r');
      break;
    }

  } //end client connection if else
  clientGet.stop();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network

  WiFi.begin(wifi_ssid, wifi_password);
  uint counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    counter++;
    Serial.print(".");
    if (counter > 80) {
      ESP.deepSleep(SLEEP_TIME_SECONDS * 1000000);
    }
  }

}

void loop() {
  delay(1000);
    
  float teplota = am2320.readTemperature();
  float vlhkost = am2320.readHumidity();
  Serial.println("temp: " + String(teplota));
  if (!isnan(teplota)) {
    setup_wifi();
    postData(String(teplota), String(vlhkost));

    Serial.println("going to sleep....");
    ESP.deepSleep(SLEEP_TIME_SECONDS * 1000000);
  } else { 
    Serial.println("error");
  }
  delay(3000);
}
