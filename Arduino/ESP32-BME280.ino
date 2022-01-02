/*
  Weather Station
*/
#include <WiFi.h>
#include "time.h"
#include <Wire.h>
#include <PubSubClient.h>
#include "SparkFunBME280.h"

// Deep Sleep Details
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  1500

// WiFi Details
const char* ssid = "";
const char* password = "";
const char* ntpServer = "pool.ntp.org";

// MQTT Broker Details
const char* mqtt_broker = "mqtt.ably.io";
const char* topic = "data";
const char* mqtt_username = "";
const char* mqtt_password = "";
const int mqtt_port = 1883;

// Clients
BME280 mySensor;
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Ready...");
  InitI2C();
  InitWifi();
  InitMQTT();
  configTime(0, 0, ntpServer);
  measureWeather();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop() 
{ 
  //  This is not going to be used 
}

void InitWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(2000);
  } 
}

void InitMQTT()
{
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
     Serial.println("Connecting to MQTT...");
     String client_id = "esp32-client";
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("MQTT connected");
     } else {
         Serial.print("failed with state: ");
         Serial.println(client.state());
         delay(2000);
     }
   }
}

void InitI2C() 
{
  Wire.begin();
  while (mySensor.beginI2C() != true)
  {
    delay(500);
    Serial.println("The sensor did not respond. Please check wiring.");
    Wire.begin();
  }
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return now;
}

void measureWeather() 
{
  String m1 = String(getTime());
  String m2 = String(mySensor.readTempC(), 2);
  String m3 = String(mySensor.readFloatPressure(), 3);
  String m4 = String(mySensor.readFloatAltitudeMeters(), 3);
  String m5 = String(mySensor.readFloatHumidity(), 3);
  String message = "{Time:"+ m1 +",Temperature(deg C):"+ m2 +",Pressure(Pa):"+ m3 +",Altitude(m):"+ m4 +",RelativeHumidity(%):"+ m5 +"}";
  Serial.println(message);
  client.publish(topic, message.c_str());
  Serial.println("Message Published");
}
