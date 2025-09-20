# Hunter Roam - Esphome External component
This code is a port of the [project hunter-wifi](https://github.com/ecodina/hunter-wifi)  \(based on [Sebastien](https://github.com/seb821/OpenSprinkler-Firmware-Hunter) project\) to esphome external component. 
It shall be quiet easy to install on any esp8266 or esp32 board, if you are familiar with [esphome](https://esphome.io).

As the above projects, It can control up to 48 zones and 4 programs.

## Compatible devices
I am using it with Hunter X-Core sprinkler controller, but it should work with any Hunter product that is SmartPort (Roam) compatible. Basically, it needs a REM pin. More information on [Hunter's webpage](https://www.hunterindustries.com/en-metric/irrigation-product/remotes/roam)
## Bill of materials

- ESP Board<br>
  I tested it on [Wemos D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html), Wemos D1 R2, and [Sonoff TH320D](https://devices.esphome.io/devices/Sonoff-THR320D) - ESP32, it should work on any [esp32](https://devices.esphome.io/board/esp32), [esp8266](https://devices.esphome.io/board/esp8266) board with esphome.
- cables for connecting board to Hunter's REM pin, and 24V pin

## How to connect the esp to hunter
A quite extensive documentation is available on [hunter-wifi project webpage](https://ecodina.github.io/hunter-wifi/#!pages/hunterconnection.md).<br>
In short (how I'm using it): connect the esp board 3.3V PIN to Hunter controllars AC#2 port , and the configured GPIO to its REM port.

## How to use the code
Import the source code as [external component](https://esphome.io/components/external_components):
```
external_components:
  - source: github://andrastakacs/hunter-remote@main
    components: [hunter_remote]
```
Configurate it by defining an id of the component and the port REM pin is connected to:
```
hunter_remote:
  id: hunterremote
  pin: GPIO1
```
The component provides 3 functions:

- startProgram

```
      - lambda:  |-
         id(hunterremote).startProgram(programNr);
```
Used to start hunter controllers stored program by number (A-1,B-2,C-3,D-4). 

- startZone

```
      - lambda:  |-
         id(hunterremote).startZone(zoneNr, minutes);
```
Used to start selected zone (1,2,...48), and specifing minutes the zone shall be running.

- stopZone

```
      - lambda:  |-
         id(hunterremote).stopZone(zoneNr);
```
Used to start selected zone (1,2,...48), and specifing minutes the zone shall be running. 

Two sample esphome example yamls are also available:
1. [d1mini_base.yaml](./example_d1mini_base.yaml)<br>
Simple yaml for [D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html) exposing the functions as services on esphome API.
2. [d1mini_opensprinkler.yaml](./example_d1mini_opensprinkler.yaml)<br>
Example yaml for [D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html) using the component behind [esphome sprinkler component](https://esphome.io/components/sprinkler) with 2 valves.


## License
Given that [Eloi Codina Torras](https://github.com/ecodina/hunter-wifi) and [Sebastien](https://github.com/seb821/OpenSprinkler-Firmware-Hunter) published their code under the GNU GPL v3, this project follows the same license.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
