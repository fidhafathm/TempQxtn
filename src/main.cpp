#include <Arduino.h>
#include "Adafruit_SHTC3.h"
#include <SPI.h>
#include <Wifi.h>
#include <PubSubClient.h>
#include <stdbool.h>

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();


#define relay 4
#define pubIntrvl 1  // in sec
#define touchsensor 5

// Update these with values suitable for your network.
const char* ssid = "KabaniTech";         
const char* password = "makerspace";  
const char* mqtt_server = "test.mosquitto.org";  
const char* clientID = "1q2w3e4r5t"; 
bool relayState;
bool buttonState = false;

WiFiClient espClient;
PubSubClient client(espClient);

void connectWiFi();
void connectMqtt();
void onMessage(char* topic, byte* payload, unsigned int length);

double prevPubTime =0;

void setup() {
  pinMode(relay,OUTPUT);
  pinMode(touchsensor,INPUT);
  Serial.begin(115200);
  shtc3.begin();
  connectWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(onMessage);
  connectMqtt();
}

void loop() {
	double millisNow = millis();
	if(millisNow-prevPubTime > pubIntrvl*1000){
		sensors_event_t humidity, temp;
		shtc3.getEvent(&humidity, &temp);
		float tempval = temp.temperature;
		Serial.println(temp.temperature);
		char payload[20];
		sprintf(payload,"%.2f",temp.temperature);
		if(client.publish("temperature13567",payload)){
			Serial.println("published");
		}
		else{
			Serial.println("error");
		}
		prevPubTime = millisNow;
	}
  if(digitalRead(touchsensor)){
    delay(700);
    relayState = !relayState;
    char relayPayload[15];
    sprintf(relayPayload,"%.2f",relayState);
    if(client.publish("device1a",relayPayload)){
      Serial.println("switch state changed");
    }
  }
  digitalWrite(relay,relayState);
  client.loop();
}


void connectWiFi() {
  delay(10);
  // Start the Wi-Fi connection
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectMqtt() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      client.subscribe("device1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void onMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  relayState = false;

 if(payload[0] == '1'){
	relayState = true;
 }
  Serial.println();
}


