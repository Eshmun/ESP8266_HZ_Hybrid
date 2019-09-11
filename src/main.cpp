#include <Arduino.h>
#include "osapi.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "os_type.h"
#include <stdio.h>
#include <string.h>
#include "ESP8266WiFi.h"
#include <FastLED.h>

#define NUM_LEDS 8
#define DATA_PIN 14

#define MIN_RSSI_HZ -40
#define MIN_RSSI_ZH -40

CRGB leds[NUM_LEDS];

bool human = true; //1=Human 0=Zombie

bool zombie_found = false;
bool human_found = false;

bool zombie_prox = false;
bool human_prox = false;

void rssi_to_leds(int rssi);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

    ESP.wdtDisable();

    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    WiFi.begin("Human", "", 1);
}

void loop()
{
    int n = WiFi.scanNetworks(false, false, 1, NULL); //scan channel 1
    for (int i = 0; i < n; i++)
    {
        if (WiFi.SSID(i) == "Zombie")
        {
            zombie_found = true;

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
            human_found = true;

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
    }
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