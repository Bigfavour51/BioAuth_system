// #ifndef __BIOAUTHAWS_API___
// #define __BIOAUTHAWS_API___

// #include <Arduino.h>
// #include <WiFiClientSecure.h>
// #include <MQTTClient.h>
// #include <ArduinoJson.h>
// #include "secretkeys.h"
// #include "fingerprint_handler.h"
// #include "status_indicator.h"
// #include "oled_states.h"
// #include "wifi_handler.h"
// #include <pgmspace.h>
// #include <PubSubClient.h>



// /* Configuration For AWS Thing using generated topics, certificates and keys*/
// #define AWS_PUBLISH_TOPIC "bimotericAuth/pub"
// #define AWS_SUBSCRIBE_TOPIC "bimotericAuth/sub"

// WiFiClientSecure net;
// PubSubClient client(net);           // Use the secure client with PubSubClient

// //Timer for publishing message to AWS
// unsigned long previousPublishMillis = 0;
// unsigned long currentPublishMillis = 0;
// const long publishInterval = 5000; //Publish to AWS every 5 seconds

// bool awsConfigStatus = false; // to check if AWS is connected or not
// //AWS FUNCTIONS


// void setupNTP() { //Set time using internet   -->important for AWS IoT Core
  
//   Serial.print("Setting time using SNTP...");
//   configTime(TIME_ZONE*3600, 0, "pool.ntp.org", "time.nist.gov");
  
//   while (!time(nullptr)) {
//     Serial.print(".");
//     delay(500);
//   }
//   Serial.println("done!");

//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     Serial.println("Failed to obtain time info");
//     return;
//   }
//   Serial.print("Current time: ");
//   Serial.println(asctime(&timeinfo));
// }

// void connectAWS() {
//   setupNTP();

//     net.setCACert(cacert);              // Set the root CA certificate
//     net.setCertificate(client_cert);    // Set client certificate (PEM format)
//     net.setPrivateKey(privkey);         // Set private key (PEM format) 
//     client.setServer(MQTT_HOST, 8883);
// //   client.setCallback(messageReceived); //to receive message from aws
//   Serial.print("Connecting to AWS IoT...");
//   while (!client.connect(THINGNAME)) {
//     Serial.print(".");
//     delay(1000);
//   }

//   if (!client.connected()) {
//     Serial.println("AWS IoT Timeout!");
//     return;
//   }

//   // Subscribe to a topic
//   client.subscribe(AWS_SUBSCRIBE_TOPIC);
//   Serial.println("AWS IoT Connected!");
// }

// void publishMessage(int &fid) {  //Publish message to AWS (AWS will subsribe to this topic to get the message)
//   StaticJsonDocument<200> doc;
//   doc["fingerid"] = fid;
//   char jsonBuffer[512];
//   serializeJson(doc, jsonBuffer);
  
//   if (client.publish(AWS_PUBLISH_TOPIC, jsonBuffer)) {
//     Serial.println("Published to AWS IoT.");
//   } else {
//     Serial.println("Failed to publish to AWS IoT.");
//   }

//   Client.onMessage( messageHandler())
// }


// void AWS_Loop(int &fidupdate) {
//   if (!client.connected()) { // ensure AWS is connected before publishing
//     awsConfigStatus = false;
//     connectAWS();
//   } else {
//     client.loop(); // maintain MQTT connection
//     currentPublishMillis = millis();
//     if (currentPublishMillis - previousPublishMillis >= publishInterval) {
//       publishMessage(fidupdate);
//       previousPublishMillis = currentPublishMillis;
//     }
//     awsConfigStatus = true; // AWS is connected
//   }
// }


// void messageHandler(String &topic, String &payload) {
//   StaticJsonDocument<128> doc;
//   DeserializationError error = deserializeJson(doc, payload);

//   if (error) {
//     Serial.println("JSON parse failed");
//     return;
//   }
//   const char* command = doc["command"];
//     if (strcmp(command, "ENROLL") == 0) {
//        delay(200);
//         Serial.println("ENROLL command recognized");  // Debug
//         enrollFingerprint();
//         publishMessage(p);
        
//     }
//     else if(strcmp(command, "AUTH") == 0)
//     {
//       delay(200);
//         Serial.println("AUTH command recognized");  // Debug
//         authenticateFingerprint();
//         publishMessage(p);
//     }
//     else 
//     {
//       Serial.println("Invalid message received");
//     }

// }





// #endif // __BIOAUTHAWS_API___



#ifndef __BIOAUTHAWS_API___
#define __BIOAUTHAWS_API___

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secretkeys.h"
#include "fingerprint_handler.h"
#include "status_indicator.h"
#include "oled_states.h"
#include "wifi_handler.h"
#include <pgmspace.h>

/* Configuration For AWS Thing using generated topics, certificates and keys */
#define AWS_PUBLISH_TOPIC "bimotericAuth/pub"
#define AWS_SUBSCRIBE_TOPIC "bimotericAuth/sub"

WiFiClientSecure net;
PubSubClient client(net);

unsigned long previousPublishMillis = 0;
const long publishInterval = 5000;

void publishMessage(int fid);

bool awsConfigStatus = false;

// --------- NTP Time Setup (needed for AWS cert handshake) ----------
void setupNTP() {
  Serial.print("Setting time using SNTP...");
  configTime(TIME_ZONE * 3600, 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" done!");
}

// --------- AWS Message Handler ----------
void messageHandler(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Incoming message on topic [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.println("JSON parse failed");
    return;
  }

  const char* command = doc["command"];
  if (strcmp(command, "ENROLL") == 0) {
    Serial.println("ENROLL command recognized");
    int fid = enrollFingerprint();  // Should return the enrolled ID
    publishMessage(fid);
  } else if (strcmp(command, "AUTH") == 0) {
    Serial.println("AUTH command recognized");
    int fid = authenticateFingerprint();  // Should return matched ID or -1
    publishMessage(fid);
  } else {
    Serial.println("Invalid message received");
  }
}

// --------- Publish Fingerprint ID to AWS ----------
void publishMessage(int fid) {
  StaticJsonDocument<200> doc;
  doc["fingerid"] = fid;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  if (client.publish(AWS_PUBLISH_TOPIC, jsonBuffer)) {
    Serial.println("Published to AWS IoT.");
  } else {
    Serial.println("Failed to publish to AWS IoT.");
  }
}

// --------- AWS Connection ----------
void connectAWS() {
  setupNTP();

  net.setCACert(cacert);
  net.setCertificate(client_cert);
  net.setPrivateKey(privkey);

  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageHandler);

  Serial.print("Connecting to AWS IoT...");
  while (!client.connected()) {
    if (client.connect(THINGNAME)) {
      Serial.println(" connected!");
    } else {
      Serial.print(".");
      delay(1000);
    }
  }

  client.subscribe(AWS_SUBSCRIBE_TOPIC);
  Serial.println("Subscribed to AWS topic.");
}

// --------- AWS Loop ----------
void AWS_Loop() {
  if (!client.connected()) {
    awsConfigStatus = false;
    connectAWS();
  }
  client.loop();  // Always needed for MQTT callbacks

  // Optional timed publishing (if needed)
  // current millis logic here if you want to publish periodically
}

#endif // __BIOAUTHAWS_API___
