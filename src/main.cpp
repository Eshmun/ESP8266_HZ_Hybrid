#include <Arduino.h>
#include "osapi.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "os_type.h"
#include <stdio.h>
#include <string.h>
#include "ESP8266WiFi.h"
#include <FastLED.h>
#include <Ticker.h>

#define NUM_LEDS 8
#define DATA_PIN 14

enum State_enum {HUMAN, ZOMBIE, COMMAND};
//enum Scan_enum {NONE, ZOMBIE_FOUND, HUMAN_FOUND, ZOMBIE_PROX, HUMAN_PROX, COMMAND_PROX};
enum Command_enum {NONE, START_GAME, CHANGE_STATE, CHANGE_MIN_RSSI_HZ, CHANGE_MIN_RSSI_ZH};

uint8_t state = HUMAN;
uint8_t scan_result = NONE;

int MIN_RSSI_HZ = -40;
int MIN_RSSI_ZH = -40;

CRGB leds[NUM_LEDS];

Ticker ticker;

struct scanResults
{
    int closestHuman, idHuman;
    int closestZombie, idZombie;
    int closestCommand, commandType, commandMessage;
};

//bool human = true; //1=Human 0=Zombie

void rssi_to_leds(int rssi);
scanResults scanForNetworks();

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

    ESP.wdtDisable();

    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    WiFi.begin("Human", "", 1);

    //ticker.attach_ms(100, scanForNetworks);
}

void loop()
{
    scanResults scanresults = scanForNetworks();

    switch (state)
    {
    case HUMAN:
        rssi_to_leds(scanresults.closestZombie);
        if (scanresults.closestZombie > MIN_RSSI_HZ)
        {
            Serial.println(scanresults.idZombie);
            state = ZOMBIE;
        }

        break;
    case ZOMBIE:
        rssi_to_leds(scanresults.closestHuman);
        if (scanresults.closestHuman > MIN_RSSI_ZH)
        {
            Serial.println(scanresults.idHuman);
            state = ZOMBIE;
        }

        break;
    case COMMAND:
        break;
    }
    /*
    int n = WiFi.scanNetworks(false, false, 1, NULL); //scan channel 1
    for (int i = 0; i < n; i++)
    {
        if (WiFi.SSID(i) == "Zombie")
        {

            if (human)
            {
                rssi_to_leds((int)WiFi.RSSI(i));

                if (WiFi.RSSI(i) > MIN_RSSI_HZ)
                {
                    WiFi.softAP("Zombie");
                    Serial.println("I'm now a Zombie!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

                    for (int j = 0; j <= 7; j++)
                    {
                        leds[j] = CRGB::Red;
                    }

                    human = false;
                }
            }


        }
        else if (WiFi.SSID(i) == "Human")
        {

            if (!human)
            {
                rssi_to_leds((int)WiFi.RSSI(i));
            }
            if (WiFi.RSSI(i) > MIN_RSSI_ZH)
            {
                if (!human)
                {
                    WiFi.softAP("Human");
                    Serial.println("I'm now a Human!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

                    for (int j = 0; j <= 7; j++)
                    {
                        leds[j] = CRGB::Green;
                    }

                    human = true;
                }
            }
        }
        FastLED.show();

    }*/
}

void rssi_to_leds(int rssi)
{
    int RSSI_LEDS = map(0 - rssi, 100, 40, 0, NUM_LEDS - 1);

    int b1 = (NUM_LEDS * 0.25) - 1;
    int b2 = (NUM_LEDS * 0.5) - 1;
    int b3 = (NUM_LEDS * 0.75) - 1;

    for (int j = 0; j <= RSSI_LEDS; j++)
    {
        if (j <= b1)
        {
            leds[j] = CRGB::Green;
        }
        else if (j > b1 && j <= b2)
        {
            leds[j] = CRGB::Yellow;
        }
        else if (j > b2 && j <= b3)
        {
            leds[j] = CRGB::Orange;
        }
        else
        {
            leds[j] = CRGB::Red;
        }
    }
    for (int j = RSSI_LEDS + 1; j <= NUM_LEDS; j++)
    {
        leds[j] = CRGB::Black;
    }
}

scanResults scanForNetworks()
{
    int n = WiFi.scanNetworks(false, false, 1, NULL); //scan channel 1

    int idZombie    = 1000;
    int idHuman     = 1000;
    int idCommand   = 1000;

    scanResults results;
    results.closestZombie   = -100;
    results.idZombie        = 0;

    results.closestHuman    = -100;
    results.idHuman         = 0;

    results.closestCommand  = -100;
    results.commandType     = NONE;
    results.commandMessage  = 0;

    for (int i = 0; i < n; i++)
    {
        if (WiFi.SSID(i).startsWith("Zombie"))
        {
            if (WiFi.RSSI(i) > results.closestZombie)
            {
                results.closestZombie = WiFi.RSSI(i);
                idZombie = i;
            }
        }
        else if (WiFi.SSID(i).startsWith("Human"))
        {
            if (WiFi.RSSI(i) > results.closestHuman)
            {
                results.closestHuman = WiFi.RSSI(i);
                idHuman = i;
            }
        }
        else if (WiFi.SSID(i).startsWith("Command"))
        {
            if (WiFi.RSSI(i) > results.closestCommand)
            {
                results.closestCommand = WiFi.RSSI(i);
                idCommand = i;
            }
        }
    }
    if (idCommand < 1000)
    {
        String SSID = WiFi.SSID(idCommand);
        results.commandType     = SSID.substring(SSID.indexOf("|"), SSID.indexOf("|") + 1).toInt();
        results.commandMessage  = SSID.substring(SSID.indexOf("|") + 1).toInt();
    }
    if (idZombie < 1000)
    {
        String SSID             = WiFi.SSID(idZombie);
        results.idZombie        = SSID.substring(SSID.indexOf("|")).toInt();
    }
    if (idHuman < 1000)
    {
        String SSID             = WiFi.SSID(idHuman);
        results.idHuman         = SSID.substring(SSID.indexOf("|")).toInt();
    }
    return results;
}
