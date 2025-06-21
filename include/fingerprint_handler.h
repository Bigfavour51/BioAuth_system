#ifndef FINGERPRINT_HANDLER_H
#define FINGERPRINT_HANDLER_H

#include <Adafruit_Fingerprint.h>
#include "status_indicator.h"
#include "oled_states.h"

#define FINGER_RX 16
#define FINGER_TX 17
int p = -1;

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

HardwareSerial fingerSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

uint8_t fingerID = 1;

void setupFingerprintSensor() {
  fingerSerial.begin(56900, SERIAL_8N1, FINGER_RX, FINGER_TX);
  finger.begin(56900);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected");
  } else {
    Serial.println("Fingerprint sensor NOT found");
  }
}

bool enrollFingerprint() {
  indicateProcessing(); // Indicate processing with LED
  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Place finger", 20); u8g2.sendBuffer();

  Serial.println("Waiting for finger...");


  // Wait for finger to be placed
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(100);
      continue;
    } else if (p != FINGERPRINT_OK) {
      Serial.print("Error getting image: "); Serial.println(p);
      return false;
    }
  }

  Serial.println("Image taken");
  indicateTempSuccess(); // Indicate temporary success with LED

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.print("Image to template 1 failed: "); Serial.println(p);
    return false;
  }

  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Remove finger", 20); u8g2.sendBuffer();
  indicateProcessing(); // Indicate processing with LED
  delay(2000);

  // Wait for finger to be removed
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    delay(100);
  }
  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Place same finger", 20); u8g2.sendBuffer();

  // Wait for same finger again
  while ((p = finger.getImage()) != FINGERPRINT_OK) {
    delay(100);
  }

  Serial.println("Second image taken");
  indicateTempSuccess(); // Indicate temporary success with LED

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.print("Image to template 2 failed: "); Serial.println(p);
    return false;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.print("Create model failed: "); Serial.println(p);
    return false;
  }

  p = finger.storeModel(fingerID);
  if (p == FINGERPRINT_OK) {
    Serial.print("ENROLLED_ID: "); Serial.println(fingerID);
    fingerID++;
    indicateSuccess(); // Indicate success with buzzer/LED
    drawEnrollmentSuccess(u8g2);
    delay(4000);
    return true;
  }

  Serial.print("Store failed: "); Serial.println(p);
  indicateFailure(); // Indicate failure with buzzer/LED
  // u8g2.clearBuffer();
  // drawAccessDenied(u8g2);
  // u8g2.sendBuffer();

  return false;
}

bool authenticateFingerprint() {
  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Place Finger", 20); 
  u8g2.sendBuffer();
  indicateProcessing(); // Indicate processing with LED
  Serial.println("Waiting for finger...");

  // Wait for valid finger placement
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();

    if (p == FINGERPRINT_NOFINGER) {
      delay(100);
      continue;
    } else if (p != FINGERPRINT_OK) {
      Serial.print("Error getting image: "); Serial.println(p);
      return false;
    }
  }
  Serial.println("Image taken");

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.print("Image to template failed: "); Serial.println(p);
    return false;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("AUTH_ID: "); Serial.println(finger.fingerID);
    u8g2.clearBuffer();
    drawFingerprintMatch(u8g2);
    indicateSuccess(); // Indicate success with buzzer/LED
     u8g2.sendBuffer();
    drawAuthSuccess(u8g2);
    delay(4000);
    

    // delay(1000);
    // u8g2.clearBuffer();
    // drawAccessGranted(u8g2);
    // u8g2.sendBuffer();
     return true;
  }

  Serial.println("Fingerprint not recognized");
  u8g2.clearBuffer();
  drawFingerprintMismatch(u8g2);
  indicateFailure(); // Indicate failure with buzzer/LED
  u8g2.sendBuffer();
  // delay(1000);
  // u8g2.clearBuffer();
  // drawAccessDenied(u8g2);
  // u8g2.sendBuffer();
  return false;
}


#endif // FINGERPRINT_HANDLER_H