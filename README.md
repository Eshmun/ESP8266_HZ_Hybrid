# ESP8266 Human Zombie tag game
## Overview
This is a prototype for a game of tag. All players except for a few start as humans. The others start as zombies. When a zombie gets close to a human, the human will be converted to a zombie. This zombie is now able to convert more humans. Throughout the playing field, beacons can be found which can apply a power up.  

## Hardware
The current state of a player is indicated by an RGB LED. A RGB LED ring shows the nearest zombie to a human and visa versa. This is all attached to an ESP8266 microcontroller. The device is worn on the wrist and battery powered

## Software
The detection of other players is done with WIFI networks. All devices set up a WIFI network with either the SSID Human or Zombie. depending on the current state. The devices scan for other networks nearby and depending on the strength take action. A base station creates a WIFI SSID starting with Command followed by a code. This code is used to configure devices and set up beacons to provide power ups. The devices only communicate with SSIDs, no connection is made
