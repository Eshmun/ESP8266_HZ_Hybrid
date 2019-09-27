#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
//#include <Ticker.h>

#define NUM_LEDS 8
#define DATA_PIN D5
#define DEBUG 0

//enum State_enum {HUMAN, ZOMBIE, COMMAND};
//enum Scan_enum {NONE, ZOMBIE_FOUND, HUMAN_FOUND, ZOMBIE_PROX, HUMAN_PROX, COMMAND_PROX};
//enum Command_enum {NONE, START_GAME, CHANGE_STATE, CHANGE_MIN_RSSI_HZ, CHANGE_MIN_RSSI_ZH};

int MIN_RSSI_HZ = -40;
int MIN_RSSI_ZH = -40;
int MIN_RSSI_CMD = -20;

int nextState = 0;
int currentState = 0;

int ID = 20;


//Ticker ticker;
Adafruit_NeoPixel pixels(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

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

    pixels.begin();

    ESP.wdtDisable();

    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    WiFi.begin("Human", "", 1);

}

void loop()
{
    scanResults scanresults = scanForNetworks();

    switch (currentState)
    {
    case 0:
        rssi_to_leds(scanresults.closestZombie, MIN_RSSI_HZ);
        if (scanresults.closestZombie > MIN_RSSI_HZ)
        {
            Serial.println("ZOMBIE DETECTED!");
            digitalWrite(LED_BUILTIN, LOW);
            nextState = 1;
        }

        if (scanresults.closestCommand > MIN_RSSI_CMD)
        {
            Serial.println("Command Detected!");
            nextState = 2;
        }

        break;
    case 1:
        rssi_to_leds(scanresults.closestHuman, MIN_RSSI_ZH);
        if (scanresults.closestHuman > MIN_RSSI_ZH)
        {
            Serial.println("HUMAN DETECTED!");
            nextState = 0;
        }

        if (scanresults.closestCommand > MIN_RSSI_CMD)
        {
            Serial.println("Command Detected!");
            nextState = 2;
        }

        break;
    case 2:
        switch (scanresults.commandType)
        {
        case 1:
            break;

        case 2:
            break;

        case 3:
            if ((scanresults.commandMessage > -100) && (scanresults.commandMessage < 0))
            {
                MIN_RSSI_HZ = scanresults.commandMessage;
            }

            break;

        case 4:
            if ((scanresults.commandMessage > -100) && (scanresults.commandMessage < 0))
            {
                MIN_RSSI_ZH = scanresults.commandMessage;
            }
            break;

        default:
            break;
        }
        break;
    }

    currentState = nextState;

    if (DEBUG)
    {
        Serial.print("Zombie:   ");
        Serial.println(scanresults.closestZombie);
        Serial.print("ID Z:     ");
        Serial.println(scanresults.idZombie);

        Serial.print("Human:    ");
        Serial.println(scanresults.closestHuman);
        Serial.print("ID H:     ");
        Serial.println(scanresults.idHuman);

        Serial.print("Command:  ");
        Serial.println(scanresults.closestCommand);
        Serial.print("CMD T:    ");
        Serial.println(scanresults.commandType);
        Serial.print("CMD M:    ");
        Serial.println(scanresults.commandMessage);

        Serial.print("State:    ");
        Serial.print(currentState);
        Serial.println(nextState);

        Serial.println();
        delay(500);
    }
}

void rssi_to_leds(int rssi, int min_rssi)
{
    int RSSI_LEDS = map(0 - rssi, 100, min_rssi, 0, NUM_LEDS - 1);

    Serial.println(rssi);
    Serial.println(RSSI_LEDS);

    int b1 = (NUM_LEDS * 0.25) - 1;
    int b2 = (NUM_LEDS * 0.5) - 1;
    int b3 = (NUM_LEDS * 0.75) - 1;

    for (int j = 0; j <= RSSI_LEDS; j++)
    {
        if (j <= b1)
        {
            //leds[j] = CRGB::Green;
            pixels.setPixelColor(j, pixels.Color(0, 128, 0));
        }
        else if (j > b1 && j <= b2)
        {
            //leds[j] = CRGB::Yellow;
            pixels.setPixelColor(j, pixels.Color(255, 255, 0));
        }
        else if (j > b2 && j <= b3)
        {
            //leds[j] = CRGB::Orange;
            pixels.setPixelColor(j, pixels.Color(255, 100, 0));
        }
        else
        {
            //leds[j] = CRGB::Red;
            pixels.setPixelColor(j, pixels.Color(255, 0, 0));
        }
    }
    for (int j = RSSI_LEDS + 1; j <= NUM_LEDS; j++)
    {
        //leds[j] = CRGB::Black;
        pixels.setPixelColor(j, pixels.Color(0, 0, 0));
    }
    //FastLED.show();
    pixels.show();
}

scanResults scanForNetworks()
{
    int n = WiFi.scanNetworks(false, false, 1, NULL); //scan channel 1

    int indexZombie    = 1000;
    int indexHuman     = 1000;
    int indexCommand   = 1000;

    scanResults results;
    results.closestZombie   = -100;
    results.idZombie        = 0;

    results.closestHuman    = -100;
    results.idHuman         = 0;

    results.closestCommand  = -100;
    results.commandType     = 0;
    results.commandMessage  = 0;

    for (int i = 0; i < n; i++)
    {
        if (WiFi.SSID(i).startsWith("Zombie"))
        {
            if (WiFi.RSSI(i) > results.closestZombie)
            {
                results.closestZombie = WiFi.RSSI(i);
                indexZombie = i;
            }
        }
        else if (WiFi.SSID(i).startsWith("Human"))
        {
            if (WiFi.RSSI(i) > results.closestHuman)
            {
                results.closestHuman = WiFi.RSSI(i);
                indexHuman = i;
            }
        }
        else if (WiFi.SSID(i).startsWith("Command"))
        {
            if (WiFi.RSSI(i) > results.closestCommand)
            {
                results.closestCommand = WiFi.RSSI(i);
                indexCommand = i;
            }
        }
    }
    if (indexCommand < 1000)
    {
        String SSID = WiFi.SSID(indexCommand);
        results.commandType     = SSID.substring(SSID.indexOf(".") + 1, SSID.indexOf(".") + 2).toInt();
        results.commandMessage  = SSID.substring(SSID.indexOf(".") + 2).toInt();
    }
    if (indexZombie < 1000)
    {
        String SSID             = WiFi.SSID(indexZombie);
        results.idZombie        = SSID.substring(SSID.indexOf(".") + 1).toInt();
    }
    if (indexHuman < 1000)
    {
        String SSID             = WiFi.SSID(indexHuman);
        results.idHuman         = SSID.substring(SSID.indexOf(".") + 1).toInt();
    }
    return results;
}
