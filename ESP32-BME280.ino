/*
  Weather Station
*/
#include <WiFi.h>
#include "time.h"
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFunBME280.h"

// WiFi Details
const char* ssid = "BELL864";
const char* password = "2C6223E6";
const char* ntpServer = "pool.ntp.org";

// MQTT Broker Details
const char* mqtt_broker = "mqtt.ably.io";
const char* topic = "data";
const char* mqtt_username = "Yu5U3g.E_wIUA";
const char* mqtt_password = "TmExsBHnhBNrD8VdQvzM5CzcWjyxmdPKDzeU7e2sM9k";
const int mqtt_port = 1883;

// Clients
BME280 mySensor;
WiFiClient espClient;
PubSubClient client(espClient);

// Variable to save current unix time
unsigned long unixTime;

void setup()
{
  Serial.begin(115200);
  InitWifi();
  InitMQTT();
  InitI2C();
}

void loop() 
{
  while(WiFi.status() != WL_CONNECTED) {
      InitWifi();
    }
  while (!client.connected()) {
      InitMQTT();
    }
  while (mySensor.beginI2C() != true) {
      InitI2C();
    }
    
  String m1 = String(getTime());
  String m2 = String(mySensor.readTempC());
  String m3 = String(mySensor.readFloatPressure());
  String m4 = String(mySensor.readFloatAltitudeMeters());
  String m5 = String(mySensor.readFloatHumidity());

  String message = "{Time:"+ m1 +",Temperature(deg C):"+ m2 +",Pressure(Pa):"+ m3 +",Altitude(m):"+ m4 +",RelativeHumidity(%)"+ m5 +"}";
  client.publish(topic, message.c_str());
  Serial.println("Message Published");
  delay(25000);
}

void InitWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  } 
}

void InitMQTT()
{
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.println("Connecting to MQTT...");
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
