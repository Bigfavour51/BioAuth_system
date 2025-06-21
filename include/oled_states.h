#ifndef OLED_STATES_H
#define OLED_STATES_H

#include <U8g2lib.h>

// Centered string drawer
void drawCenteredText(U8G2 &u8g2, const char* text, uint8_t y) {
  uint8_t w = u8g2.getStrWidth(text);
  u8g2.drawStr((128 - w) / 2, y, text);
}

void drawWiFiStarting(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredText(u8g2, "Wi-Fi Starting...", 20);
  u8g2.drawRFrame(20, 35, 88, 10, 3); // Rounded frame
  u8g2.drawBox(20, 35, (millis() / 100) % 89, 10);
  u8g2.sendBuffer();
}

void drawWiFiConnected(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredText(u8g2, "Wi-Fi Connected!", 20);

  // Wi-Fi symbol
  uint8_t cx = 64, cy = 48;
  u8g2.drawCircle(cx, cy, 4, U8G2_DRAW_ALL);
  u8g2.drawCircle(cx, cy, 8, U8G2_DRAW_ALL);
  u8g2.drawCircle(cx, cy, 12, U8G2_DRAW_ALL);
  u8g2.drawDisc(cx, cy, 2); // dot center

  u8g2.sendBuffer();
}

void drawFingerprintMatch(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredText(u8g2, "Fingerprint Match", 20);

  u8g2.drawCircle(64, 44, 20, U8G2_DRAW_ALL);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  drawCenteredText(u8g2, "✓", 52);

  u8g2.sendBuffer();
}

void drawFingerprintMismatch(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredText(u8g2, "Fingerprint Mismatch", 20);

  // Large X symbol inside circle
  u8g2.drawCircle(64, 44, 20, U8G2_DRAW_ALL);
  u8g2.drawLine(54, 34, 74, 54);
  u8g2.drawLine(54, 54, 74, 34);

  u8g2.sendBuffer();
}

void drawAccessGranted(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredText(u8g2, "ACCESS GRANTED", 23);

  // Bold rectangle with tick
  u8g2.drawRBox(30, 36, 68, 20, 4);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  drawCenteredText(u8g2, "✓", 50);

  u8g2.sendBuffer();
}

void drawAccessDenied(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB10_tr);
  drawCenteredText(u8g2, "ACCESS DENIED", 23);

  // Box with cross
  u8g2.drawRFrame(30, 36, 68, 20, 4);
  u8g2.drawLine(40, 40, 88, 52);
  u8g2.drawLine(40, 52, 88, 40);

  u8g2.sendBuffer();
}

void drawHomeScreen(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredText(u8g2, "CBT AUTH SYSTEM", 16);
  drawCenteredText(u8g2, "Waiting for Command", 36);
  u8g2.sendBuffer();
}

void drawEnrollmentSuccess(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredText(u8g2, "Enrollment Complete", 20);

  // Box + Tick
  u8g2.drawRBox(30, 36, 68, 20, 4);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  drawCenteredText(u8g2, "✓", 50);

  // Footer note
  u8g2.setFont(u8g2_font_5x7_tr);
  drawCenteredText(u8g2, "ID sent to web", 62);

  u8g2.sendBuffer();
}

void drawAuthSuccess(U8G2 &u8g2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tr);
  drawCenteredText(u8g2, "Authentication OK", 20);

  // Bold box + Tick
  u8g2.drawRBox(30, 36, 68, 20, 4);
  u8g2.setFont(u8g2_font_ncenB14_tr);
  drawCenteredText(u8g2, "✓", 50);

  // Footer note
  u8g2.setFont(u8g2_font_5x7_tr);
  drawCenteredText(u8g2, "ID transmitted", 62);

  u8g2.sendBuffer();
}

#endif // OLED_STATES_H
