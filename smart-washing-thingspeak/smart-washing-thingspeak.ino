/*
   Filename:    smart-washing-thingspeak.ino
   Created by:  Lahis Almeida
   Release:     1.0
   Date:        01/08/2021
   Description: This programa implement a Smart Washing Notifier. It's responsabile to:
    - read LDR sensor value
    - calibrate not operate mode (light off) value
    - connect to WiFi
    - connect to MQTT Cloud Thingspeak
    - send Washing Status
*/

#include <string.h> 
#include <PubSubClient.h>
#include <WiFi.h>

/* Light Sensor */
// defines pins numbers
const int analogPIN = 4;  //D2


/* Wi-Fi Connection*/
const char* ssid     = "Zizi";
const char* password = "casadazizi";

/* MQTT Protocol */
const char* mqtt_user = "smart-washing";
const char* mqtt_server = "mqtt.thingspeak.com";
const int mqtt_port = 1883;
// char* topic= "channels/<channelID/publish/<channelAPI>
const char* mqtt_topic = "channels/1461278/publish/DV9RINDKHP8RCJMF";


WiFiClient espClient;
PubSubClient client(espClient);

/**
   Send any MQTT message to the broker. The Thingspeak message content can constain 8 fields.
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
  
  Serial.print("PUB: ");
  Serial.println(payload);
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
  
}

void loop() {
  connectWifi();
  
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Send data to IoT Platform...");
  sendMessageToBroker();
    
  delay(10000);
//  Serial.println("Done, let's sleep...");
  
}
