
# MCP9802 DRIVER

[![Platform Badge](https://img.shields.io/badge/platform-Arduino-orange.svg)](https://www.arduino.cc/)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
[![SemVer](https://img.shields.io/badge/SemVer-1.5.0-brightgreen.svg)](http://semver.org/)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
[![SemVer](https://img.shields.io/badge/eagle-pcb-yellow.svg)](https://cadsoft.io/)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

## INTRODUCTION

The MCP9802 is a 9 to 12-Bit Single-Channel Temperature Sensor with Hysteresis & Alert capabilities,  
as well as a hardware I2C interface.

This library contains a robust driver for the MCP9802 that exposes its entire functionality (i.e. Configuration, Temperature, Hysteresis, Limit, and Alert Settings), allowing the user to get/set data in degrees Celsius or Fahrenheit.

[MCP9802 DATASHEET](http://ww1.microchip.com/downloads/en/DeviceDoc/21909d.pdf)

[MCP9802 DATASHEET ERRATA](http://ww1.microchip.com/downloads/en/DeviceDoc/80331B.pdf)

## REPOSITORY CONTENTS

- **MCP9802.h** - Library Header file.
- **MCP9802.cpp** - Library Compilation.
- **/utility** 
  - **MCP9802InfoStr.h** - Header file containing a functional extention of the library to include generating printable information String (see Note #8 below).
  - **MCP9802ComStr.h** - Header file containing a functional extention of the library to include generating a printable I2C Communication Result String (see Note #9 below).
  - **MCP9802_PString.h** - Header file for PString class (lighter alternative to String class) 
  - **MCP9802_PString.cpp** - Compilation file for PString class (lighter alternative to String class) 
- **/examples**  
  - **/MCP9802_Test**
    - **MCP9802_Test.ino** - A basic sketch for testing whether the MCP9802 is hooked-up and operating correctly.
  - **/MCP9802_Usage**
    - **MCP9802_Usage.ino** - A much more extensive sketch offering a complete usage illustration, as well as a rubust testing mechanism.
  - **/MCP9802_Info**
    - **MCP9802_Info.ino** - A short sketch showing how to generate a Printable Device Information String of the MCP9802's current settings (i.e. Configuration, Limit &anp; Hysteresis registers, etc.).
  - **/MCP9802_I2C_Status**
    - **MCP9802_I2C_Status.ino** - A short sketch for verifying I2C communication has been established between the controller and the MCP9802.
- **/extras** 
  - **License.txt** - A cope of the end-user license agreement.  
  - **/eagle**
    - **MCP9802.sch** - Schematic file for the MCP9802 breakout board.
    - **MCP9802.brd** - Board layout file for the MCP9802 breakout board.
- **keywords.txt** - Keywords for this library which will be highlighted in sketches within the Arduino IDE. 
- **library.properties** - General library properties for the Arduino's IDE (>1.5) Library Package Manager.
- **README.md** - The readme file for this library.
- **library.json** - JSON file for the Arduino's IDE (>1.5) Package Manager.

## HOOK-UP

* __PIN 1 (VCC)__ - Connect VCC to the Arduino's 5V output
* __PIN 2 (GND)__ - Connect GND to the Arduino's GND
* __PIN 3 (ALERT)__ - Connect ALERT to the Arduino's Digital Pin 2
* __PIN 4 (SCL)__ - Connect SCL to the Arduino's PIN A5 with a 2K2 pull-up resistor
* __PIN 5 (SDA)__ - Connect SDA to the Arduino's PIN A4 with a 2K2 pull-up resistor
* __DECOUPING__: Connect a 0.1uF Ceramic Capacitor between the MCP9802's VCC &amp GND PINS

## GENERAL NOTES

1) __I2C Communications Library Dependency__

This library depends on the Arduino IDE's native '[Wire](https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr/libraries/Wire)' library for I2C communication between the Arduino (Master) and the MCP9802 (Slave). 

2) __Device Temperature Range__

The MCP9802 is designed to measure temperature btween -55°C to 125°C (-67°F to 257°F). Measurments below or above this range will return the minimum or maximum measurable value. Concurently, the ability to custom set the HYSTERESIS or LIMIT values has been limited to this range in software (even though logically, these values would need to be al least slightly lower or higher with respect to the actual measurable temperature). Hence, attempts to set these registers with a new value beyond the said range will actually set the register's value to the maximum/minimum allowable value. 

3) __Standby & Conversion Mode__

The first bit of the configuration byte controls the device mode of operation, namely: ON, in which the device operates in 'CONTINUOUS' mode, or OFF - or more precisely STANDBY (as I2C communication remains active) - in which the device operates in 'SINGLE-SHOT' nmode. As such, setting the 'CONVERSION MODE' of the device to 'CONTINUOUS' will effectively ensure that it is 'ON', while setting it to 'SINGLE-SHOT' mode will turn it into STANDBY mode.

4) __Hysteresis & Limit Registers Resolution__

The Temperature register has a setteble range of 9 to 12-BIT (0.5 to 0.0625 degrees Celsius respectively). However, both the LIMIT and HYSTERESIS registers only have a 9-BIT fixed resolution. This means these registers can only be set with a maximum accuracy of 0.5 degrees Celsius. Hence, while the relevant functions (i.e. setHyst() and setLimit() ) will happily accept any float value within the premmitted parameter range (-55°C to 125°C) for either of these two registers, this float value will be automatically rounded to the nearest 0.5C.

5) __Degrees Celsius & Fahrenheit__

The libraty offers the option of getting/setting all termperature values (Abmient [read-only], Limit [read-write] and/or Hysteresis [read-write]) in either degrees Celsuis or Fahrenheit (the default is degrees Celsius).

6) __Degrees Fahrenheit Accuracy Limitation__

As the MCP9802 was designed to work in a degrees Celsuis scheme, all Fahrenheit values obtained (or custom set by the user) can only represent as close approximations as possible with relation to the Celsius values generated or stored by the device. This limitation is perhaps most noticable when setting the LIMIT or HYSTERESIS registers to custom Fahrenheit values, as a double operation needs to take place, namely: conversion of this figure to the equivalent Celsius value and then rounding that value to the nearest 0.5 degree Celisus (the latter stems from the 9-BIT size of the HYSTERESIS & LIMIT registers as noted above).

7) __Alert Output Signal__

The MCP9802's Alert functionality is based on an 'open collector' architecture which means it requires a pull-up resistor in order to work (this is true for both Alert Types, i.e. 'ACTIVE-LOW' and 'ACTIVE-HIGH). For the purposes of this testing sketch, the Atmega's (weak) internal pull-up resistor is used and so the only connection needed in this context is between the MCP9802's ALERT pin and the Arduino's Digital Pin D2. However, for any real-life use of the device, it is highly recommended to implement a suitable external pull-up resistor (typically 10K) hooked-up betweem the ALERT pin and VCC.

8) __Device Information String__

It is now possible to extend the MCP9802 Library to include a function for generating a pritable device information string showing all the relevant details about the devices current Configuration, Limit & Hysteresis settings. As the additional functionality comes at the cost of increased memory footprint, it was implemented as an optional add-on rather than added directly to the core MCP9802 Library. See the [MCP9802_Info](https://github.com/nadavmatalon/MCP9802/blob/master/examples/MCP9802_Info/MCP9802_Info.ino) example sketch for detailed explanation and an actual usage demo.

9) __Device I2C Communications String__

It is now possible to also extend the MCP9802 Library to include a function for generating a pritable I2C Communications string showing the result of each I2C transaction in a human-friendly way, something that may be useful, for example, during debugging sessions. As the additional functionality comes at the cost of increased memory footprint, it was implemented as an optional add-on rather than added directly to the core MCP9802 Library. See the [MCP9802_I2C_Status](https://github.com/nadavmatalon/MCP9802/blob/master/examples/MCP9802_I2C_Status/MCP9802_I2C_Status.ino) example sketch for detailed explanation and an actual usage demo.


## I2C ADDRESSES

Each MCP9802 has 1 of 8 possible I2C addresses (factory hardwired & recognized by its specific part number & top marking on the package itself):

| PART NO.        | BIN      | HEX  | DEC | MARKING |
|-----------------|----------|------|-----|---------|
| MCP9802A0T-M/OT | 01001000 | 0x48 | 72  | JKNN    |
| MCP9802A1T-M/OT | 01001001 | 0x49 | 73  | JLNN    |
| MCP9802A2T-M/OT | 01001010 | 0x4A | 74  | JMNN    |
| MCP9802A3T-M/OT | 01001011 | 0x4B | 75  | JPNN    |
| MCP9802A4T-M/OT | 01001100 | 0x4C | 76  | JQNN    |
| MCP9802A5T-M/OT | 01001101 | 0x4D | 77  | JRNN    |
| MCP9802A6T-M/OT | 01001110 | 0x4E | 78  | JSNN    |
| MCP9802A7T-M/OT | 01001111 | 0x4F | 79  | JTNN    |


## LIBRARY INSTALLATION & SETUP

Begin by installing the library either by using the Arduino IDE's Installation Wizard (Arduino Version >1.5) or simply download the library's ZIP folder from GITHUB, extract it, and copy the extraxcted folder to your Arduino 'libraries' folder.

Next, include the library at the top of the sketch as follows:

```
#include "MCP9802.h"
```

At this point you can construct a new MPC9802 object(s) by using the following command (at the top of the sketch after the 'include' line):

```
MCP9802 device_name(device_address);
```

>__NOTE__: replace the '__device_name__' above with a name of your choice. Also, make sure to replace the variable '__device_address__' with the specific I2C address of your device - see I2C ADDRESSES section above.


## LIBRARY FUNCTIONS

With the library installed & included in the sketch, and an MCP9802 object initiallized, the following functions are available (see the usage example sketch for a detailed implementation):

__Note About Methods' Return Values:__  
All 'get' methods return some sort of value (e.g. temp reading, hysteresis setting, etc.), while all 'set' methods return nothing. Nevertheless, ALL methods implicitly update the library's __I2C _comBuffer__ (=communication buffer) after each I2C transmission. The reason for this functional design is to maintain structural coherance between the 'get' and 'set' methods. As 'get' methods cannot return both the desired value and the I2C transmission's result simultaniously. Consequently, if the relevant value hasn't been obtained by a particular 'get' method, the user can simply check the content of the _comBuffer to see what error occured. Similarly, it is possible to check if a particular setting has been successfully applied via a 'set' method either by preforming the corresponding 'get' method - e.g. getHystC() after using setHystC() - or by checking the content of the _comBuffer (0 indicates a successful transmission, 1-6 indicate an error as listed below). 

__ping();__                                  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Searches for the MCP9802 at the pre-defined I2C Bus address &amp; returns byte with the relevant success/error code, as follows:  
0  ... Success (no error)  
1  ... Buffer overflow  
2  ... Address sent, NACK received  
3  ... Data send, NACK received  
4  ... Other error (lost bus arbitration, bus error, etc.)  
5  ... Timed-out while trying to become Bus Master  
6  ... Timed-out while waiting for data to be sent
>6 ... Unlisted error (potential future implementation/s)<br>

Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;byte  

__getTemp();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns current temperature reading in degrees Celsius  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;float

__getHyst();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current Hysteresis register value in degrees Celsius   
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;float

__getLimit();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current Limit register value in degrees Celsius    
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;float

__getAlertType();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current alert type setting (0 = COMPARATOR / 1 = INTERRUPT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;byte

__getAlertMode();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current alert mode setting (0 = ACTIVE-LOW / 1 = ACTIVE-HIGH)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__getFaultQueue();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current Fault Queue setting (1 / 2 / 4 / 6 fault readings - i.e. below Hysteresis or above the Limit - before activating the alert)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;byte

__getResolution();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current Resolution setting (9 = 9-BIT / 10 = 10-BIT / 11 = 11-BIT / 12 = 12-BIT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;byte

__getConMode();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current Conversion Mode setting (0 = CONTINUOUS / 1 = SINGLE-SHOT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;byte

__getTempUnit();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Returns the current Degrees Unit setting (0 = CELSIUS / 1 = FAHRENHEIT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;byte

__singleCon();__  
Parameters:&nbsp;&nbsp;&nbsp;None   
Conditions:&nbsp;&nbsp;&nbsp;&nbsp;Works only in __Single-Shot__ mode  
Description:&nbsp;&nbsp;&nbsp;Carries out a single conversion &amp; returns a temperature reading in degrees Celsius  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;float

__setHyst();__  
Parameters:&nbsp;&nbsp;&nbsp;int / float (range: -55 to +125)  
Description:&nbsp;&nbsp;&nbsp;Sets the Hysteresis register value in degrees Celsius  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__setLimit();__  
Parameters:&nbsp;&nbsp;&nbsp;int / float (range: -55 to +125)  
Description:&nbsp;&nbsp;&nbsp;Sets the Limit register value in degrees Celsius   
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__setAlertType();__  
Parameters:&nbsp;&nbsp;&nbsp;COMP / INT  
Description:&nbsp;&nbsp;&nbsp;Sets the alert output type (COMPARATOR / INTERRUPT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__setAlertMode();__  
Parameters:&nbsp;&nbsp;&nbsp;ACTIVE_LOW / ACTIVE_HIGH  
Description:&nbsp;&nbsp;&nbsp;Sets the alert output mode (ACTIVE-LOW / ACTIVE-HIGH)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__setFaultQueue();__  
Parameters:&nbsp;&nbsp;&nbsp;FQ1 / FQ2 / FQ4 / FQ6  
Description:&nbsp;&nbsp;&nbsp;Sets the Fault Queue value (FQ1 = 1 Fault / FQ2 = 2 Faults / FQ4 = 4 Faults / FQ6 = 6 Faults)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__setResolution();__  
Parameters:&nbsp;&nbsp;&nbsp;RES_9 / RES_10 / RES_11 / RES_12  
Description:&nbsp;&nbsp;&nbsp;Sets the Resolution value (RES_9 = 9-BIT / RES_10 = 10-BIT / RES_11 = 11-BIT / RES_12 = 12-BIT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__setConMode();__  
Parameters:&nbsp;&nbsp;&nbsp;CONT / SINGLE  
Description:&nbsp;&nbsp;&nbsp;Sets the Conversion Mode (CONTINUOUS / SINGLE-SHOT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__setTempUnit();__  
Parameters:&nbsp;&nbsp;&nbsp;CELSIUS / FAHRENEHIT  
Description:&nbsp;&nbsp;&nbsp;Sets the Degree Unit type (CELSIUS / FAHRENEHIT)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__reset();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;&nbsp;Resets the device to power-up default settings (except for the TEMP Register which is Read-Only), as follows:

* REG POINTER: 0x00 (TEMP REGISTER)
* CONFIG: 0x00
* DEVICE MODE: ON
* ALERT OUTPUT TYPE: COMPARATOR
* ALERT OUTPUT MODE: ACTIVE-LOW
* FAULT-QUEUE: 1 FAULT
* RESOLUTION: 9-BIT
* CONVERSION MODE: CONTINUOUS
* DEGREES TYPE: CELSIUS
* HYSTERESIS: 0x9600 (75°C)
* LIMIT: 0xA000 (80°C)

Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;None

__getComResult();__  
Parameters:&nbsp;&nbsp;&nbsp;None  
Description:&nbsp;&nbsp;Returns the latest I2C Communication result code (see Success/Error codes above)  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;byte

__Destructor__  
If you want to destruct an instantiated MCP9802 object, you can use the following method to do so:

```
~MCP9802 device_name();
```
>__NOTE__: replace the '__device_name__' above with the name of your MCP9802 device.

## Extended Functionality*

(* requires an additional '\#include' of the relevant *.h file as shown in the corresponding example sketches)

__MCP9802ComStr();__  
Parameters:&nbsp;&nbsp;&nbsp;Name of an initialized MCP9802 instance  
Description:&nbsp;&nbsp;Returns printable string containing human-friendly information about the device's latest I2C communication result  
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;PString

__MCP9802InfoStr();__  
Parameters:&nbsp;&nbsp;&nbsp;Name of an initialized MCP9802 instance  
Description:&nbsp;&nbsp;Returns printable string containing detailed information about the device's current settings   
Returns:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;PString

## RUNNING THE EXAMPLE SKETCHES

1) Hook-up the MCP9802 to the Arduino as explained above.   
2) Upload the relevant example sketch to the Arduino.  
3) Open the Serial Communications Window (make sure the baud-rate is set to 9600 or change it in the sketch to match your Serial Port's buad-rate).  

## BUG REPORTS

Please report any issues/bugs/suggestions at the [Issues](https://github.com/nadavmatalon/MCP9802/issues) section of this Github repository.

## TODO

* __CORE LIBRARY__: add emptyBuffer() function and amend I2C communication functions to include 3 attempts to get data
* __CORE LIBRARY__: Change setX() functions to work with bitMasks (?)
* __CORE LIBRARY__: Create interger-math methods for getting/setting Temp/Hyst/Limit   
* __CORE LIBRARY__: Add running average functionality  

## VERSION HISTORY

__Ver. 1.0.0__ - First release (26.9.16)  
__Ver. 1.1.0__ - Small change in functionality: attempting to set hysteresis or limit beyond the legitimate range (-55°C - 125°C / -67°F - 257°F) now sets the register to the maximum/minumum allowable value rather than do nothing (4.10.16)  
__Ver. 1.2.0__ - Changed license to MIT (5.10.16)  
__Ver. 1.3.0__ - Changed auxilliary functions: MCP9802InfoStr() and MCP9802ComStr() to work with the PString class instead of the String class to further reduce memory footprint. For this purpose, added PString.h & PString.cpp files to /utility folder. In addition added "I2C STATUS" (CONNECTED / NOT CONNECTED) field to device information string (9.10.16)  
__Ver. 1.4.0__ - Added namespaces to prevent conflicts with other libraries (15.10.16)  
__Ver. 1.5.0__ - Switched from 'WSWire' library to 'Wire' library for I2C communications (24.10.16)

## LICENSE

[The MIT License (MIT)](https://opensource.org/licenses/MIT)
Copyright (c) 2016 Nadav Matalon

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

