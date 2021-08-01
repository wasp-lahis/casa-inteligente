/*
   Filename:    smart-washing-thingspeak.ino
   Created by:  Lahis Almeida
   Release:     1.0
   Date:        01/08/2021
   
   Description: This programa implement a Smart Washing Notifier. It's responsabile to:
    - TO DO hide credentials
    - TO DO read LDR sensor value
    - TO DO calibrate not operate mode (light off) value
    - connect to WiFi
    - connect to MQTT Cloud Thingspeak
    - TO DO send LDR value and Washing Status
*/

#include <string.h> 
#include <PubSubClient.h>
#include <WiFi.h>
#include "credentials.h"

/* Light Sensor */
// defines pins numbers
const int analogPIN = 4;  // 0 to 4095.


/* Wi-Fi Connection*/
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASSWORD;

/* MQTT Protocol */
const char* mqtt_user = MQTT_USERNAME;
const char* mqtt_server = MQTT_SERVER;
const int   mqtt_port = MQTT_PORT;

// char* topic= "channels/<channelID/publish/<channelAPI>
char mqtt_topic[ 
  sizeof("channels/") + 
  sizeof(TSPEAK_CHANNEL_ID) + 
  sizeof("/publish/") + 
  sizeof(TSPEAK_CHANNEL_WRITE_API_KEY)] = "";

  

WiFiClient espClient;
PubSubClient client(espClient);


/**
   Generate MQTT topic string.
   @param none
   @return
   @see http://www.cplusplus.com/reference/cstring/strcat/
*/
void generateMqttTopics(){
  strcpy(mqtt_topic, "channels/");
  strcat(mqtt_topic, TSPEAK_CHANNEL_ID);
  strcat(mqtt_topic, "/publish/");
  strcat(mqtt_topic, TSPEAK_CHANNEL_WRITE_API_KEY);
}


/**
   Send any MQTT message to the broker. The Thingspeak message content can constain 8 fields.
   @param none
   @return
   @see https://www.youtube.com/watch?v=yRtc_UKuJuU
*/
void sendMessageToBroker() {

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_user)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  // if you get here you have connected to the WiFi
  String payload = "field1=";
  payload += micros();
  //  payload+="&field2=";
  //  payload+=0;
  payload+="&status=MQTTPUBLISH";

  generateMqttTopics();
  
  Serial.print("TOPIC: ");
  Serial.println(mqtt_topic);
  Serial.print("MSG: ");
  Serial.println(payload);
  
  if (client.publish(mqtt_topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
  }
  else {
      Serial.println("Publish failed");
  }
  
  client.loop();    
}

/**
   This function freezes the program until wifi is connected.
   WiFi.begin() needs to be called before this function.
   @param none
   @return
   @see http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
*/
void connectWifi() {
 //int countDelay = 0;
  Serial.print("Your are connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  Serial.print("Trying to connect");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Device connected.");
}


void setup() {

  // Serial Initialization
  Serial.begin(115200); // Starts the serial communication
  delay(2000);
  
  Serial.print("\n----------------TESTE----------------\n");
  Serial.println(TSPEAK_CHANNEL_ID);
}

void loop() {
  connectWifi();
  
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Send data to IoT Platform...");
  sendMessageToBroker();
    
  delay(10000);
//  Serial.println("Done, let's sleep...");
  
}
