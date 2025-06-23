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


#define AWS_PUBLISH_TOPIC "bimotericAuth/sub"
#define AWS_SUBSCRIBE_TOPIC "bimotericAuth/pub"
#define AWS_PUBSTATUS_TOPIC "bimotericAuth/login/pub"
#define AWS_SUBSTATUS_TOPIC "bimotericAuth/login/sub"

WiFiClientSecure net;
PubSubClient client(net);

unsigned long previousPublishMillis = 0;
const long publishInterval = 5000;
void publishAuthMessage(int fid);
void connectAWS();
bool awsConfigStatus = false;

volatile bool enrollRequested = false;
volatile bool authRequested = false;
char incomingUserId[40];  // Make room for a 36-char UUID + null terminator



void setupNTP() {
  Serial.print("Setting time using SNTP...");
  configTime(TIME_ZONE * 3600, 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" done!");
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Incoming message on topic [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.println("JSON parse failed");
    return;
  }

  // 🧠 Distinguish by topic
  if (strcmp(topic, "bimotericAuth/pub") == 0) {
    if (doc.containsKey("command")) {
      const char* command = doc["command"];

      if (strcmp(command, "ENROLL") == 0) {
        enrollRequested = true;

        if (doc.containsKey("userId")) {
          strncpy(incomingUserId, doc["userId"], sizeof(incomingUserId));
          incomingUserId[sizeof(incomingUserId) - 1] = '\0';
          Serial.print("Enrollment requested for user ID: ");
          Serial.println(incomingUserId);
        }
      }

      else if (strcmp(command, "AUTH") == 0) {
        authRequested = true;
        Serial.println("AUTH command manually triggered.");
      }

      else {
        Serial.println("Unknown command received.");
      }
    }
  }

  else if (strcmp(topic, "bimotericAuth/login/sub") == 0) {
  if (doc.containsKey("authenticate")) {
    bool isAuth = doc["authenticate"];
    const char* userId = doc["userId"] | "Unknown";
    const char* name = doc["name"] | "Unknown";
    const char* email = doc["email"] | "Unknown";

    Serial.println("Authentication response received:");
    Serial.print("User ID: "); Serial.println(userId);
    Serial.print("Name: "); Serial.println(name);
    Serial.print("Email: "); Serial.println(email);
    Serial.print("Auth status: "); Serial.println(isAuth ? "GRANTED" : "DENIED");

        if (isAuth) {
        indicateSuccess();

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr); // Nice readable font
        u8g2.drawStr(0, 12, "Access Granted!");

        u8g2.setCursor(0, 28);
        u8g2.print("Name: ");
        u8g2.print(name);

        u8g2.setCursor(0, 44);
        u8g2.print("Email: ");
        u8g2.print(email);

        u8g2.sendBuffer();
        delay(4000);
      } else {
        indicateFailure();

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 12, "Access Denied!");

        u8g2.setCursor(0, 28);
        u8g2.print("Name: ");
        u8g2.print(name);

        u8g2.setCursor(0, 44);
        u8g2.print("Email: ");
        u8g2.print(email);

        u8g2.sendBuffer();
        delay(4000);
      }

  } else {
    Serial.println("Invalid AUTH payload from web.");
  }
}


  else {
    Serial.println("Unrecognized topic.");
  }
}

void publishMessage(int fid, const char* userId) {
  StaticJsonDocument<256> doc;
  doc["event"] = "ENROLL_COMPLETE";
  doc["fingerprint"] = fid;
  doc["userId"] = userId;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  // 🚨 Ensure client is connected
  if (!client.connected()) {
    Serial.println("MQTT not connected. Attempting to reconnect...");
    connectAWS();  // Call your reconnect logic
    delay(500);    // Give time to connect
  }

  if (client.connected()) {
    if (client.publish(AWS_PUBLISH_TOPIC, jsonBuffer)) {
      Serial.println("Published enrollment result to AWS IoT:");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("Failed to publish to AWS IoT.");
    }
  } else {
    Serial.println("Still not connected. Publish aborted.");
  }
}

void publishAuthMessage(int fid) {
  StaticJsonDocument<128> doc;
  doc["fingerprint"] = fid;

  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer);

  // 🚨 Check if MQTT client is connected
  if (!client.connected()) {
    Serial.println("MQTT not connected. Attempting to reconnect...");
    connectAWS();  // Use your AWS reconnect function
    delay(500);
  }

  if (client.connected()) {
    if (client.publish(AWS_PUBSTATUS_TOPIC, jsonBuffer)) {
      Serial.println("Published AUTH to AWS IoT:");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("Failed to publish AUTH to AWS IoT.");
    }
  } else {
    Serial.println("Still not connected. Publish aborted.");
  }
}

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

 client.subscribe("bimotericAuth/pub");        // For incoming commands
 client.subscribe("bimotericAuth/login/sub");  // For AUTH result status
 Serial.println("Subscribed to AWS topics.");

}

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
