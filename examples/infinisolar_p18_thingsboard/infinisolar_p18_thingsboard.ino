#include <WiFi.h>           // WiFi control for ESP32
#include <ThingsBoard.h>    // ThingsBoard SDK
#include "infinisolar_p18_thingsboard.h" // Defs for various secrets, keys
#include "InfiniCommandSender.h"
#include "InfiniResponseParser.h"

using INFI::InfiniCommandSender;
using INFI::InfiniResponseParser;

#define RXD2 16
#define TXD2 17

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
const size_t MAX_FIELDS_AMT = 16;
const size_t NUM_RPC_CALLBACKS = 9;
ThingsBoardSized<INFI::MAX_RESPONSE_SZ, MAX_FIELDS_AMT,
                 ThingsBoardDefaultLogger,
                 NUM_RPC_CALLBACKS> tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD    115200

InfiniCommandSender cmdSender(Serial2, &Serial);
InfiniResponseParser respParser;

// Main application loop delay
int quant = 20;
unsigned long quant_now = 0;

// Period of delay for Wifi (re)connection attempt.
int wifi_connect_delay = 500;

// Time passed after Wifi (re)connection attempted.
unsigned long wifi_connect_passed = 0;

// Period of sending inverter data.
int send_delay = 10000;

// Time passed after inverter data was sent, milliseconds.
int send_passed = 0;

// Set to true if application is subscribed for the RPC messages.
bool subscribed = false;

// RPC handlers
RPC_Response processEnableDisableStatus(const RPC_Data &data) {
  Serial.println("Received an enable/disable flag status toggle method.");

  // All the enable disable vals are just switches.
  
  const size_t NUM_KEYS = 7;
  const size_t MAX_NUM_CHARS = 24;
  char e[] = "mn";
  bool val;
  char keys[NUM_KEYS][MAX_NUM_CHARS] = { "open_buzzer\0", "overload_bypass\0", "lcd_escape\0", "overload_restart\0", "overload_temp_restart\0", "backlight_on\0", "grid_tie\0"};
  char codes[NUM_KEYS][1] = {'A', 'B', 'C', 'D', 'E', 'F', 'I'};

  size_t k_i = 0;
  for (; k_i < NUM_KEYS; ++k_i) {
    if (data.containsKey(keys[k_i])) {
      e[1] = codes[k_i][0];
      val = data[keys[k_i]].as<bool>();
      e[0] = val ? 'E' : 'D';
      break;
    }
  }
  if (k_i == NUM_KEYS) {
    return RPC_Response("unknown", false);
  }
  cmdSender.sendCommand(INFI::SET_ENABLE_DISABLE_STATUS, e);
  if (cmdSender.response.val[1] == '1') {
    return RPC_Response(keys[k_i], val);
  } else {
    return RPC_Response("refuse", false);
  }
}

RPC_Response processSetMaxChargingCurrent(const RPC_Data &data) {
  Serial.println("Received the set max charging current function.");

  // Get the value from data, which is just int.
  int current = data.as<int>();
  Serial.print(current); Serial.println(" Amps");
  char currentChars[6];
  currentChars[0] = '0';
  currentChars[1] = ',';
  INFI::getThreeDigits(current, currentChars, 2);
  cmdSender.sendCommand(INFI::SET_MAX_CHARGING_CURRENT, currentChars);

  if (cmdSender.response.val[1] == '1') {
    return RPC_Response("set_mchgcr", current);
  } else {
    return RPC_Response("set_mchgcr", -1);
  }
}

RPC_Response processSetMaxACChargingCurrent(const RPC_Data &data) {
  Serial.println("Received the set max AC charging current function.");

  // Get the value from data, which is just int.
  int current = data.as<int>();
  Serial.print(current); Serial.println(" Amps");
  char currentChars[6];
  currentChars[0] = '0';
  currentChars[1] = ',';
  INFI::getThreeDigits(current, currentChars, 2);
  cmdSender.sendCommand(INFI::SET_MAX_AC_CHARGING_CURRENT, currentChars);

  if (cmdSender.response.val[1] == '1') {
    return RPC_Response("set_muchgcr", current);
  } else {
    return RPC_Response("set_muchgcr", -1);
  }
}

RPC_Response processSetACOutFreq(const RPC_Data &data)
{
  Serial.println("Received the set AC Out Frequency method");
  
  // Convert rpc call to StaticJsonDocument, because it is just char*
  StaticJsonDocument<256> doc;
  deserializeJson(doc, data.as<char*>());
  serializeJson(doc, Serial);
  Serial.println();
  
  // Get the new frequency from doc.
  int freq = doc["ac_out_freq"].as<int>();
  freq = (freq < 55) ? 50 : 60;
  if (freq == 50) {
    Serial.println("Setting to 50Hz");
    cmdSender.sendCommand(INFI::AC_OUT_FREQ_50, {});
  } else {
    Serial.println("Setting to 60Hz");
    cmdSender.sendCommand(INFI::AC_OUT_FREQ_60, {});
  }

  return RPC_Response("ac_out_freq", freq);
}

RPC_Response processSetOutputSourcePriority(const RPC_Data &data) {
  Serial.println("Received the set AC Output Source Priority method");

  int priority = data.as<int>() % 2; // mod 2 makes sure it's 0/1.
  char priorityChars[2];
  sprintf(priorityChars, "%d", priority);
  cmdSender.sendCommand(INFI::SET_OUTPUT_SOURCE_PRIORITY, priorityChars);

  if (cmdSender.response.val[1] == '1') {
    return RPC_Response("set_pop", priority);
  } else {
    return RPC_Response("set_pop", -1);
  }
}

RPC_Response processSetChargingSourcePriority(const RPC_Data &data) {
  Serial.println("Received the set Charging Source Priority method");

  int priority = data.as<int>() % 3; // mod 3 makes sure it's 0/1/2
  char priorityChars[5];
  sprintf(priorityChars, "0,%d", priority); // hardcode the parallel machine as 0 for now
  cmdSender.sendCommand(INFI::SET_CHARGING_SOURCE_PRIORITY, priorityChars);

  if (cmdSender.response.val[1] == '1') {
    return RPC_Response("set_pcp", priority);
  } else {
    return RPC_Response("set_pcp", -1);
  }
}

RPC_Response processSetSolarPowerPriority(const RPC_Data &data) {
  Serial.println("Received the set Solar Power Priority method");

  int priority = data.as<int>() % 2; // mod 2 makes sure it's 0/1.
  char priorityChars[2];
  sprintf(priorityChars, "%d", priority);
  cmdSender.sendCommand(INFI::SET_SOLAR_POWER_PRIORITY, priorityChars);

  if (cmdSender.response.val[1] == '1') {
    return RPC_Response("set_psp", priority);
  } else {
    return RPC_Response("set_psp", -1);
  }
}

RPC_Response processSetBatteryType(const RPC_Data &data) {
  Serial.println("Received the set Battery Type method");

  int battType = data.as<int>() % 3; // mod 3 makes sure it's 0/1/2.
  char battTypeChars[2];
  sprintf(battTypeChars, "%d", battType);
  cmdSender.sendCommand(INFI::SET_BATTERY_TYPE, battTypeChars);

  if (cmdSender.response.val[1] == '1') {
    return RPC_Response("set_pbt", battType);
  } else {
    return RPC_Response("set_pbt", -1);
  }
}

RPC_Response processSetDateTime(const RPC_Data &data) {
  Serial.println("Received the set Date/Time method");

  // Our custom ng widget should send the correct chars. (lol)
  const char *dtStr = data.as<const char*>();
  cmdSender.sendCommand(INFI::SET_DATE_TIME, dtStr);

  if (cmdSender.response.val[1] == '1') {
    return RPC_Response("set_dat", dtStr);
  } else {
    return RPC_Response("set_dat", false);
  }

  return RPC_Response("unknown",NULL);
}

RPC_Callback callbacks[NUM_RPC_CALLBACKS] = {
  { "enableDisableStatus", processEnableDisableStatus },
  { "setMaxChargingCurrent", processSetMaxChargingCurrent },
  { "setMaxACChargingCurrent", processSetMaxACChargingCurrent },
  { "setACOutFreq", processSetACOutFreq },
  { "setOutputSourcePriority", processSetOutputSourcePriority },
  { "setChargingSourcePriority", processSetChargingSourcePriority },
  { "setSolarPowerPriority", processSetSolarPowerPriority },
  { "setBatteryType", processSetBatteryType },
  { "setDateTime", processSetDateTime }
};

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial2.begin(INFI::SERIAL_BAUD, SERIAL_8N1, RXD2, TXD2);
  while(!Serial || !Serial2) {
    delay(1000);
  }
  
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  InitWiFi();
  INFI::setupGMTTimeForIndia();
}

void loop() {
  if (millis() - quant_now > quant) {
    quant_now = millis();
    
    // Increment send_passed by quant
    send_passed += quant;
  
    // Reconnect to WiFi, if needed
    if (WiFi.status() != WL_CONNECTED) {
      reconnect();
      return;
    }
  
    // Reconnect to ThingsBoard, if needed
    if (!tb.connected()) {
      subscribed = false;
  
      // Connect to the ThingsBoard
      Serial.print("Connecting to: ");
      Serial.println(THINGSBOARD_SERVER);
      if (!tb.connect(THINGSBOARD_SERVER, THINGSBOARD_TOKEN)) {
        Serial.println("Failed to connect");
        return;
      }
    }
  
    // Subscribe for RPC, if needed
    if (!subscribed) {
      Serial.println("Subscribing for RPC...");
  
      // Perform a subscription. All consequent data processing will happen in
      // callbacks as denoted by callbacks[] array.
      if (!tb.RPC_Subscribe(callbacks, NUM_RPC_CALLBACKS)) {
        Serial.println("Failed to subscribe for RPC");
        return;
      }
  
      Serial.println("Subscribe done");
      subscribed = true;
    }
  
    // Check if it is a time to send inverter data.
    if (send_passed > send_delay) {
      //Serial.println("Sending data...");
      
      // Reset send_passed in case there's an early return.
      send_passed = 0;

      // Query the current time
      Serial.println("Getting current time");
      cmdSender.sendCommand(INFI::CURRENT_TIME, {});
      if (cmdSender.response.actualLen == 0) {
        Serial.println("Inverter not responding. Perhaps the inverter is disconnected.\n");
        return;
      }
      Serial.println();

      // Parse current time to get the current day
      Serial.println("Parsing current time to get current day");
      size_t parsedSz = respParser.fromILCurrentTimeToILCurrentDay(cmdSender.response.val, cmdSender.response.actualLen);
      if (parsedSz == 0) {
        Serial.println("Malformed response. Please restart device.\n");
        return;
      }
      Serial.print("Current day: "); Serial.println(respParser.parsed);
      Serial.println();

      // Upload to thingsboard
      Serial.println("Uploading current date/time to Thingsboard.");
      tb.sendTelemetryString("inverter_time", cmdSender.response.val);

      // Query the generated energy for the day.
      Serial.println("Querying generated energy for day");
      cmdSender.sendCommand(INFI::GEN_ENERGY_DAY, respParser.parsed);
      Serial.println();

      // Parse the generated energy response and upload
      Serial.println("Parsing the generated energy response");
      long energy = respParser.fromInfiniGenEnergyToULong(cmdSender.response.val, cmdSender.response.actualLen);
      Serial.print("Energy generated today in Wh: "); Serial.println(energy);
      if (energy >= 0) {        
        Serial.println("Uploading gen_energy_day to ThingsBoard");
        tb.sendTelemetryInt("gen_energy_day", energy);
      }
      Serial.println();

      // Query the generated energy for the month.
      Serial.println("Querying generated energy for month");
      cmdSender.sendCommand(INFI::GEN_ENERGY_MONTH, respParser.parsed);
      Serial.println();

      // Parse the generated energy response and upload
      Serial.println("Parsing the generated energy response");
      energy = respParser.fromInfiniGenEnergyToULong(cmdSender.response.val, cmdSender.response.actualLen);
      Serial.print("Energy generated this month in Wh: "); Serial.println(energy);
      if (energy >= 0) {
        Serial.println("Uploading gen_energy_month to ThingsBoard");
        tb.sendTelemetryInt("gen_energy_month", energy);
      }
      Serial.println();

      // Query the generated energy for the year.
      Serial.println("Querying generated energy for year");
      cmdSender.sendCommand(INFI::GEN_ENERGY_YEAR, respParser.parsed);
      Serial.println();

      // Parse the generated energy response and upload
      Serial.println("Parsing the generated energy response");
      energy = respParser.fromInfiniGenEnergyToULong(cmdSender.response.val, cmdSender.response.actualLen);
      Serial.print("Energy generated this year in Wh: "); Serial.println(energy);
      if (energy >= 0) {
        Serial.println("Uploading gen_energy_year to ThingsBoard");
        tb.sendTelemetryInt("gen_energy_year", energy);
      }
      Serial.println();

      // Query the general status.
      Serial.println("Querying the general status");
      cmdSender.sendCommand(INFI::GENERAL_STATUS, {});
      if (cmdSender.response.actualLen != INFI::START_OFFSET_SZ + INFI::RESP_TO_END_SZS[INFI::GENERAL_STATUS]) {
        Serial.println("Malformed response. Please restart device.\n");
        return;
      }
      Serial.println("Uploading General Status to Thingsboard");
      if (!tb.sendTelemetryString("gs", cmdSender.response.val)) {
        Serial.println("Could not upload General Status to Thingsboard");
      }
      Serial.println();

      // Query rated info.
      Serial.println("Querying rated info");
      cmdSender.sendCommand(INFI::QUERY_RATED_INFORMATION, "");
      Serial.println("Uploading Rated Info to Thingsboard");
      if (!tb.sendTelemetryString("piri", cmdSender.response.val)) {
        Serial.println("Could not upload Rated Info to Thingsboard");
      }
      Serial.println();

      // Query the fault warning status.
      Serial.println("Getting fault warning status");
      cmdSender.sendCommand(INFI::FAULT_WARNING_STATUS, "");
      Serial.println("Uploading Fault Warning Status to Thingsboard");
      if (!tb.sendTelemetryString("fws", cmdSender.response.val)) {
        Serial.println("Could not upload Fault Warning Status to Thingsboard");
      }
      Serial.println();

      // Query the enable/disable status.
      Serial.println("Querying enable disable status");
      cmdSender.sendCommand(INFI::QUERY_ENABLE_DISABLE_STATUS, "");
      Serial.println("Uploading Enable Disable Status to Thingsboard");
      if (!tb.sendTelemetryString("flag", cmdSender.response.val)) {
        Serial.println("Could not upload Enable Disable Status to Thingsboard");
      }
      Serial.println();

      // Query default values of changeable params.
      Serial.println("Querying default values of changeable parameters");
      cmdSender.sendCommand(INFI::QUERY_DEFAULT_VALUE, "");
      // Serial.println("Uploading Default Values to Thingsboard");
      // if (!tb.sendTelemetryString("di", cmdSender.response.val)) {
      //   Serial.println("Could not upload Enable Disable Status to Thingsboard");
      // }
      Serial.println();

      // Query the max charging current selectable value.
      Serial.println("Querying max charging current selectable value");
      cmdSender.sendCommand(INFI::QUERY_MAX_CHARGING_CURRENT, "");
      Serial.println("Uploading Max Charging Current Selectable Value to Thingsboard");
      if (!tb.sendTelemetryString("mchgcr", cmdSender.response.val)) {
        Serial.println("Could not upload Max Charging Current Selectable Value to Thingsboard");
      }
      Serial.println();

      // Query the max ac charging current selectable value.
      Serial.println("Querying max AC charging current selectable value");
      cmdSender.sendCommand(INFI::QUERY_MAX_AC_CHARGING_CURRENT, "");
      Serial.println("Uploading Max AC Charging Current Selectable Value to Thingsboard");
      if (!tb.sendTelemetryString("muchgcr", cmdSender.response.val)) {
        Serial.println("Could not upload Max AC Charging Current Selectable Value to Thingsboard");
      }
      Serial.println();
    }
  
    // Process messages
      tb.loop();
  }
}

//void sendCommand(COMMAND_TYPE commandType, const char* params) {//, Stream *debugStream) {
//  // Reset the response buffer. Is this of any use?
//  cmdSender.response.reset();
//
//  // Make the command
//  cmdSender.makeCommand(commandType, params);
//  if (debugStream != NULL) {
//    cmdSender.printCommandAsHex(&Serial);
//  }
//  
//  // Send message to inverter!
//  Serial2.flush(); // What does this do? Why have I added it?
//  Serial2.write(command.msg, command.actualLen);
//}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
