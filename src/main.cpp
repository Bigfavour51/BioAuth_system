#include <U8g2lib.h>
#include "oled_states.h"
#include "wifi_handler.h"
#include "fingerprint_handler.h"
#include "serial_command_handler.h"
#include "display_ref.h"
#include "status_indicator.h"
#include "BioAuthAWS_API.h"



U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


void setup() {
  Serial.begin(115200);
  u8g2.begin();

  drawWiFiStarting(u8g2);  // Initial frame (optional)
  setupFingerprintSensor();
  connectToWiFiWithUI();   // Updated function with retry & animation
  setupIndicator();
  connectAWS();

  if (wifiConnected) {
    drawWiFiConnected(u8g2);
    indicateSuccess();
  } else {
    u8g2.clearBuffer();
    drawCenteredText(u8g2, "Wi-Fi Failed", 20); 
    indicateFailure();
    u8g2.sendBuffer();
  }

  delay(2000);
}

 
void loop() {

  AWS_Loop();  // Keeps MQTT alive

 if (enrollRequested) {
    enrollRequested = false;
    int fid = enrollFingerprint();
    publishMessage(fid, incomingUserId);
  }
  if (authRequested) {
    authRequested = false;
    int fid = authenticateFingerprint();
    publishAuthMessage(fid, incomingUserId);
  }


  if (finger.getImage() == FINGERPRINT_OK) {
    if (finger.image2Tz() == FINGERPRINT_OK &&
      finger.fingerSearch() == FINGERPRINT_OK) {

      int fid = finger.fingerID;
      Serial.print("Finger mapped,fingerprint ID: ");
      Serial.println(fid);
      publishAuthMessage(fid, "local"); // Send to AWS IoT
      indicateSuccess();
      u8g2.clearBuffer();
      drawCenteredText(u8g2, "match found", 20);
      u8g2.sendBuffer();
      delay(1000);
      
    }
     else{
      indicateFailure();
      u8g2.clearBuffer();
      drawCenteredText(u8g2, "No match found", 20);
      u8g2.sendBuffer();
      delay(2000);
  }

  }
 
  drawHomeScreen(u8g2);

  if (WiFi.status() != WL_CONNECTED && !wifiConnected) {
    connectToWiFiWithUI();
  }

}




