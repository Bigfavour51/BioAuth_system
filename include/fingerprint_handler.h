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

int findNextAvailableID() {
  for (int id = 1; id <= 127; id++) {
    if (finger.loadModel(id) != FINGERPRINT_OK) {
      return id; // Slot is free
    }
  }
  return -1; // Sensor full
}

int enrollFingerprint() {
  indicateProcessing();
  int fid = findNextAvailableID();
  if (fid == -1) {
    Serial.println("Fingerprint sensor memory is full!");
    return -1;
  }

  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Place finger", 20);
  u8g2.sendBuffer();

  while (finger.getImage() != FINGERPRINT_OK);

  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Processing...", 20);
  u8g2.sendBuffer();

  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("Image conversion failed");
    indicateFailure(); // Indicate failure with buzzer/LED
    return -1;
  }
  delay(500);
  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Remove finger", 20);
  indicateTempSuccess(); // Indicate temporary success with buzzer/LED
  u8g2.sendBuffer();

  while (finger.getImage() != FINGERPRINT_NOFINGER);
  delay(1000);
  u8g2.clearBuffer();
  drawCenteredText(u8g2, "Place same finger again", 20);
  u8g2.sendBuffer();

  while (finger.getImage() != FINGERPRINT_OK);

  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("Second image conversion failed");
    return -1;
  }

  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("Model creation failed");
    indicateFailure(); // Indicate failure with buzzer/LED
    return -1;
  }

  if (finger.storeModel(fid) == FINGERPRINT_OK) {
    Serial.print("Stored fingerprint at ID: ");
    Serial.println(fid);
    indicateSuccess(); // Indicate success with buzzer/LED
    drawEnrollmentSuccess(u8g2);
    delay(4000);
    return fid;
  } else {
    Serial.println("Fingerprint storage failed");
    return -1;
  }
}

int authenticateFingerprint() {
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
    p = finger.fingerID; // Return the matched ID
    return p;
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