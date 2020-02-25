// Include Arduino.h for ESP32 to quieten annoying VS Code squiggles
#ifdef ARDUINO_ARCH_ESP32
    #include <Arduino.h>
#endif
#include "InfiniCommandSender.h"
#include "InfiniResponseParser.h"

using INFI::InfiniCommandSender;
using INFI::InfiniResponseParser;

#define RXD2 16
#define TXD2 17

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD    115200

const int CMD_DELAY = 2000;

InfiniCommandSender cmdSender(Serial2, &Serial);
InfiniResponseParser respParser;

void setup() {
    Serial.begin(SERIAL_DEBUG_BAUD);
    #ifdef ARDUINO_ARCH_ESP32
        Serial2.begin(INFI::SERIAL_BAUD, SERIAL_8N1, RXD2, TXD2);
    #else
        Serial2.begin(INFI::SERIAL_BAUD, SERIAL_8N1);
    #endif

    while(!Serial || !Serial2) {
        delay(1000);
    }
}

void loop() {
    delay(CMD_DELAY);
    cmdSender.sendCommand(INFI::CURRENT_TIME, {});

    if (cmdSender.response.actualLen == 0) {
        Serial.println("Inverter not responding. Perhaps the inverter is disconnected.\n");
        return;
    }
    Serial.println();
    Serial.println("Inverter responded.\n");

    // Parse current time to get the current day
    Serial.println("Parsing current time to get current day");
    size_t parsedSz = respParser.fromILCurrentTimeToILCurrentDay(cmdSender.response.val, cmdSender.response.actualLen);
    if (parsedSz == 0) {
    Serial.println("Malformed response. Please restart device.\n");
    return;
    }
    Serial.print("Current day: "); Serial.println(respParser.parsed);
    Serial.println();
}