
#include <Arduino.h>
#include "WiFi.h"
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include <arduino-timer.h>

// WiFi credentials.
// const char* WIFI_SSID = "<ssid>";
// const char* WIFI_PASS = "<password>";
#include "Credential.h"


#define LED_BUILTIN (2)
IPAddress server(192, 168, 1, 1);
WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char * powerTopic = "/server/power"; 
const char * fanTopic = "/server/fan";

class CommunicationClient{
    
protected:
    virtual void power_button(int) = 0;
    virtual void fan_speed(int) = 0;
    

public:
  CommunicationClient()
  {
      // setup mqtt client 
    client.setServer(server, 1883);
    client.setCallback([=](char* topic, byte* payload, unsigned int length){this->callback(topic, payload, length);});
      // Connect to Wifi.
    setupWiFi();
  }

  virtual void pub_temperature() = 0; 
  void loop()
  {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
   
  }

protected:
  void setupWiFi()
  {
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("Connecting...");

    while (WiFi.status() != WL_CONNECTED) {
      // Check to see if connecting failed.
      // This is due to incorrect credentials
      if (WiFi.status() == WL_CONNECT_FAILED) {
        Serial.println("Failed to connect to WIFI. Please verify credentials: ");
      }
      delay(5000);
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Hello World, I'm connected to the internets!!");
    // Allow the hardware to sort itself out
  }



  void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect("arduinoClient")) {
        Serial.println("Connected!!");
        // ... and resubscribe
        client.subscribe(powerTopic);
        client.subscribe(fanTopic);
      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }



  void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String data = "";
    for (int i = 0; i < length; i++) {
      data += (char)payload[i];
    }
    Serial.println(data);

    if(String(topic).equals(powerTopic))
      power_button(data.toInt());
    if(String(topic).equals(fanTopic))
      fan_speed(data.toDouble());    
  }


};


