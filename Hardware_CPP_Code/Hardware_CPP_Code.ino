#include <ESP8266WiFi.h>
#include <TimeLib.h>
 #include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
String apiKey = "12345";
const char* ssid =  "Airtel_8454899945";     // Enter your WiFi Network's SSID
const char* pass =  "air47214"; // Enter your WiFi Network's Password
const char* server = "api.thingspeak.com";
 
int analogInPin  = A0;    // Analog input pin
int sensorValue;          // Analog Output of Sensor
float calibration = 4.2; // Check Battery voltage using multimeter & add/subtract the value
int bat_percentage;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
 
WiFiClient client;
 
void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
 
void loop()
{ 
  sensorValue = analogRead(analogInPin);
   float voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor
 
  bat_percentage = mapfloat(voltage, 2.2, 5.10, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
 
  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
  Serial.print("Analog Value = ");
  Serial.print(sensorValue);
  Serial.print("\t Output Voltage = ");
  Serial.print(voltage);
  Serial.print("\t Battery Percentage = ");
  Serial.println(bat_percentage);
  time_t utcTime = now();
  char timestamp[20];
  snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02dZ",
           year(utcTime), month(utcTime), day(utcTime),
           hour(utcTime), minute(utcTime), second(utcTime));
 StaticJsonDocument<500> doc;
 doc["timestamp"] = timestamp;
  doc["AnalogValue"] = sensorValue;
  doc["OutputVoltage"] = voltage;
  doc["Battery_Percentage"] = bat_percentage;
  WiFiClient client;
      HTTPClient http;
      http.begin(client, "http://192.168.1.13:3000/insert");
      http.addHeader("Content-Type", "application/json");
     // http.addHeader("Authorization", "Basic YXBpdXNlcjpFQlNpbnRlZ3JhdGVAMjAyMw==");
      char out[200];
      Serial.println( serializeJson(doc, out));
      String httpRequestData =   out   ;        
      int httpResponseCode = http.POST(httpRequestData);
      Serial.println(httpRequestData);
      Serial.print("HTTP Response code is: ");
      Serial.println(httpResponseCode);
      http.end();
      delay(2000); 
  
}

 
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}