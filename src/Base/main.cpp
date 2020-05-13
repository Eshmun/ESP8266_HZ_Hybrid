#include <Arduino.h>
#include <ESP8266WiFi.h>

#define DEBUG 0

enum State_enum {HUMAN, ZOMBIE, COMMAND, IDLE};
enum Command_enum {NONE, START_GAME, SET_STATE, SET_MIN_RSSI_HZ, SET_MIN_RSSI_ZH, SET_CURRENT_TIME, SET_START_TIME };

int nextState = IDLE;
int currentState = IDLE;

char ID[] = "20";

String commandString = "";
bool stringComplete = false;

void setSSID(int command);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    commandString.reserve(200);

    ESP.wdtDisable();

    char state_name[5] = "Idle";

    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    WiFi.begin("Idle", "", 1);
    WiFi.softAP(state_name);

}

void loop()
{
    if (stringComplete) {
    Serial.println(commandString);
    // clear the string:
    commandString = "";
    stringComplete = false;
  }

    if (DEBUG)
    {
        delay(500);
    }
}


void setSSID(int command)
{
    char state_name[9] = "Command.";
    state_name[8] = command;
    WiFi.softAP(state_name);
    Serial.println(state_name);
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the commandString:
    commandString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}