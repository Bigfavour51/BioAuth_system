#ifndef __BIOAUTHAWS_API___
#define __BIOAUTHAWS_API___

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "secretkeys.h"
#include "fingerprint_handler.h"
#include "status_indicator.h"
#include "oled_states.h"
#include "wifi_handler.h"

WiFiClientSecure net;
MQTTClient client(256);

unsigned long lastPublishTime = 0;

// Initialize AWS IoT client
void connectAWS() {
  net.setCACert(aws_ca_cert);
  net.setCertificate(aws_device_cert);
  net.setPrivateKey(aws_private_key);

  client.begin(AWS_IOT_MQTT_HOST, 8883, net);

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nConnected to AWS IoT");
  client.subscribe(AWS_SUBSCRIBE_TOPIC);
}

// Function to send fingerprint ID to AWS
void SendFingerId_AWS(uint16_t id) {
  if (!client.connected()) connectAWS();

  StaticJsonDocument<128> doc;
  doc["finger_id"] = p;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  client.publish(AWS_PUBLISH_TOPIC, jsonBuffer);
}

// Callback for incoming AWS MQTT messages
void messageHandler(String &topic, String &payload) {
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.println("JSON parse failed");
    return;
  }
  const char* command = doc["command"];
       if (strcmp(command, "ENROLL") == 0) {
       delay(200);
        Serial.println("ENROLL command recognized");  // Debug
        u8g2.clearBuffer();
        drawCenteredText(u8g2, "Starting Enrollment", 20);
        u8g2.sendBuffer();

        bool result = enrollFingerprint();

        if (!result) {
          u8g2.clearBuffer();
          drawAccessDenied(u8g2);
          u8g2.sendBuffer();

        }
        } else if (strcmp(command, "AUTH") == 0) {
            delay(200);
        Serial.println("AUTH command recognized");  // Debug
        u8g2.clearBuffer();
        drawCenteredText(u8g2, "Authenticating...", 20);
        u8g2.sendBuffer();

        bool result = authenticateFingerprint();

        if (!result) {
            u8g2.clearBuffer();
          drawFingerprintMismatch(u8g2);
          u8g2.sendBuffer();
        }
    
    } else {
    Serial.println("Unknown command received");
  }
}

// Poll AWS messages
void GetStatus_AWS() {
  client.loop();
}

// Setup
void setupAWS() {
  connectAWS();
  client.onMessage(messageHandler);
}

#endif // __BIOAUTHAWS_API___
