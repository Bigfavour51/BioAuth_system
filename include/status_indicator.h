#ifndef STATUS_INDICATOR_H
#define STATUS_INDICATOR_H

#include <Adafruit_NeoPixel.h>

#define BUZZER_PIN     18
#define NEOPIXEL_PIN   4
#define NUM_PIXELS     3  // Updated for 3 LEDs

Adafruit_NeoPixel pixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


void setupIndicator();
void setAllPixels(uint32_t color);
void indicateSuccess();
void indicateFailure();
void indicateProcessing();
void indicateIdle();
void indicateTempSuccess();
void clearIndicator();


void setupIndicator() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Fix: Initialize LEDC channel for tone()
  ledcSetup(0, 5000, 8);              // Channel 0, 5 kHz, 8-bit resolution
  ledcAttachPin(BUZZER_PIN, 0);       // Attach channel 0 to the buzzer pin

  pixel.begin();
  pixel.setBrightness(255); // Max brightness
  pixel.clear();
  pixel.show();
}


void setAllPixels(uint32_t color) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixel.setPixelColor(i, color);
  }
  pixel.show();
}

void indicateSuccess() {
  clearIndicator(); // Clear previous state 
  for(int i = 0; i < 3; i++) {
    setAllPixels(pixel.Color(0, 255, 0)); // Green
    tone(BUZZER_PIN, 2000, 200);         // Loud tone
    delay(200);
    setAllPixels(pixel.Color(0, 0, 0));  // Off
    delay(100);
  }
}

void indicateFailure() {
  clearIndicator(); // Clear previous state
  for (int i = 0; i < 2; i++) {
    setAllPixels(pixel.Color(255, 0, 0)); // Red
    tone(BUZZER_PIN, 4000, 200);         // Loud tone
    delay(250);
    setAllPixels(pixel.Color(0, 0, 0));  // Off
    delay(150);
  }
  noTone(BUZZER_PIN);
}

void indicateProcessing() {
  setAllPixels(pixel.Color(255, 100, 0)); // Amber/yellow-orange
}
void indicateIdle() {
  setAllPixels(pixel.Color(0, 0, 255)); // Blue
}

void indicateTempSuccess() {
  clearIndicator(); // Clear previous state 
  for(int i = 0; i < 2; i++) {
    setAllPixels(pixel.Color(0, 255, 0)); // Green
    setAllPixels(pixel.Color(0, 0, 0));  // Off
    delay(600);
  }
}

void clearIndicator() {
  pixel.clear();
  pixel.show();
}

#endif
