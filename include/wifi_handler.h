#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <WiFi.h>
#include "oled_states.h"
#include "display_ref.h"
#include "status_indicator.h"

const char* ssid = "smarttracker";
const char* password = "12345678";

bool wifiConnected = false;

void connectToWiFiWithUI(){
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 15000; // 15 seconds

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    drawWiFiStarting(u8g2);  // animate loading on OLED
    delay(100);              // slight delay for visual update and connection
  }

  if (WiFi.status() == WL_CONNECTED) {
    indicateSuccess(); // Indicate success with LED
    wifiConnected = true;
    Serial.println("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    indicateFailure(); // Indicate failure with LED
    Serial.println("\nWiFi connection failed.");
  }
}

#endif // WIFI_HANDLER_H

