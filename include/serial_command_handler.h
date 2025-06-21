#ifndef SERIAL_COMMAND_HANDLER_H
#define SERIAL_COMMAND_HANDLER_H

#include "fingerprint_handler.h"
#include "oled_states.h"

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

String incomingCommand = "";
void handleSerialCommand() {
  while (Serial.available()) {
    char c = Serial.read();
    Serial.print(c);  // <- Debug: See if chars are coming in

    if (c == '\n' || c == '\r') {
      incomingCommand.trim();
      Serial.print("Command Received: "); Serial.println(incomingCommand);  // Debug

      if (incomingCommand.equalsIgnoreCase("ENROLL")) {
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

      } else if (incomingCommand.equalsIgnoreCase("AUTH")) {
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
      }

      incomingCommand = "";
    } else {
      incomingCommand += c;
    }
  }
}

#endif