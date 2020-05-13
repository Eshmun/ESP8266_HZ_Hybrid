#include <Arduino.h>
#include <ESP8266WiFi.h>

#define DEBUG 0

enum State_enum {HUMAN, ZOMBIE, COMMAND, IDLE};
enum Command_enum {NONE, START_GAME, SET_STATE, SET_MIN_RSSI_HZ, SET_MIN_RSSI_ZH, SET_CURRENT_TIME, SET_START_TIME };

int nextState = IDLE;
int currentState = IDLE;

char ID[] = "20";

String serialString = "";
bool serialComplete = false;

void setCommandSSID(String command);
void serialEvent();
void setIdleSSID();

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    serialString.reserve(200);

    ESP.wdtDisable();

    char state_name[5] = "Idle";

    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    WiFi.begin("Idle", "", 1);
    WiFi.softAP(state_name);

}

void loop()
{
    if (Serial.available()) {
        serialEvent();
    }
    if (serialComplete) {
        Serial.print(serialString);
        if (serialString.startsWith("Idle"))
        {
            setIdleSSID();
        }
        else
        {
            setCommandSSID(serialString);
        }
        serialString = "";
        serialComplete = false;
    }

    if (DEBUG)
    {
        delay(500);
        Serial.println("Debug");
    }
}


void setCommandSSID(String command)
{
    char state_name[20] = "Command.";
    for ( int i = 0; i < command.length(); i++)
    {
        state_name[8+i] = command.charAt(i);
    }
    
    WiFi.softAP(state_name);
    Serial.println(state_name);
}

void setIdleSSID()
{
    char state_name[5] = "Idle";
    WiFi.softAP(state_name); 
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the commandString:
    serialString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      serialComplete = true;
    }
  }
}