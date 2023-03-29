/*==============================================================================================================*

    @file     MCP9802.h
    @author   Nadav Matalon
    @license  MIT (c) 2016 Nadav Matalon
 
    MCP9802 Driver (12-BIT Temperature Sensor with I2C Interface)
 
    Ver. 1.0.0 - First release (23.9.16)
    Ver. 1.1.0 - Small change in functionality: attempting to set hysteresis or limit beyond the legitimate range
                 (-55°C - 125°C / -67°F - 257°F) now sets the register to the maximum/minumum allowable value
                 rather than do nothing (4.10.16)
    Ver. 1.2.0 - Changed license to MIT (5.10.16)
    Ver. 1.3.0 - Changed auxilliary functions: MCP9802InfoStr() and MCP9802ComStr() to work with the PString class
                 instead of the String class to further reduce memory footprint. For this purpose, added PString.h
                 PString.cpp files to /utility folder. In addition added "I2C STATUS" (CONNECTED / NOT CONNECTED)
                 field to device information string (9.10.16)
    Ver. 1.4.0 - Added namespaces to prevent conflicts with other libraries (15.10.16)
    Ver. 1.5.0 - Switched from 'WSWire' library to 'Wire' library for I2C communications (24.10.16)

 *===============================================================================================================*
    INTRODUCTION
 *===============================================================================================================*
 
    The MCP9802 is a 9 to 12-Bit Single-Channel Temperature Sensor with Hysteresis & Alert capabilities, 
    as well as a hardware I2C interface.
 
    This library contains a robust driver for the MCP9802 that exposes its entire functionality 
    (i.e. Configuration, Temperature, Hysteresis, Limit, and Alert Settings), allowing the user to get/set data 
    in degrees Celsius or Fahrenheit.
 
*===============================================================================================================*
    I2C ADDRESSES
*===============================================================================================================*

    Each MCP9802 has 1 of 8 possible I2C addresses (factory hardwired & recognized by its specific
    part number & top marking on the package itself):
 
        PART               DEVICE I2C ADDRESS          PART
       NUMBER          (BIN)      (HEX)     (DEC)     MARKING
    MCP9802A0T-M/OT   01001000    0x48       72        JKNN
    MCP9802A1T-M/OT   01001001    0x49       73        JLNN
    MCP9802A2T-M/OT   01001010    0x4A       74        JMNN
    MCP9802A3T-M/OT   01001000    0x4B       75        JPNN
    MCP9802A4T-M/OT   01001100    0x4C       76        JQNN
    MCP9802A5T-M/OT   01001101    0x4D       77        JRNN
    MCP9802A6T-M/OT   01001110    0x4E       78        JSNN
    MCP9802A7T-M/OT   01001111    0x4F       79        JTNN

*===============================================================================================================*
    POWER-UP DEFAULTS
*===============================================================================================================*

    CONFIG BYTE:        B00000000
    ALERT OUTPUT TYPE:  COMPARATOR
    ALERT OUTPUT MODE:  ACTIVE-LOW
    FAULT-QUEUE:        1 FAULT
    RESOLUTION:         9-BIT
    CONVERSION MODE:    CONTINUOUS
    TEMP  (DATA):       0x0000 (0°C)
    LIMIT (DATA):       0xA000 (80°C)
    HYST  (DATA):       0x9600 (75°C)

*===============================================================================================================*
    REGISTER POINTERS
*===============================================================================================================*

    TEMP                0x00        // Ambient Temperature Register  (R)    B00000000 (Default)
    CONFIG              0x01        // Configuration Register        (R/W)  B00000001
    HYST                0x02        // Hysteresis Register           (R/W)  B00000010
    LIMIT               0x03        // Temperature Limit Register    (R/W)  B00000011

*===============================================================================================================*
    CONFIGURATION REGIASTER (BIT 0): DEVICE MODE (0 = ON / 1 = OFF)
*===============================================================================================================*

    // The setting of this bit is completely interchangable with that of the Continuous/SINGLE-SHOT conversion 
    // modes determined by Bit 7 of the Configuration Register. Therefore, there was no point in implementing 
    // separate control functions (i.e. 'Turn Device On', 'Switch to Standby Mode', etc.) for this bit. Instead 
    // this bit is implicitly set/cleared by the relevant private methods when needed.
 
    ON                  0x00        // Standby - DISABLED (0)  B00000000 (0) (Default)
    STANDBY             0x01        // Standby - ENABLED  (1)  B00000001 (1)

*===============================================================================================================*
    CONFIGURATION REGIASTER (BIT 1): ALERT OUTPUT TYPE (0 = COMPARATOR / 1 = INTERUPT)
*===============================================================================================================*

    COMP                0x00        // Alert Output Type - COMPARATOR (0)  B00000000 (0x00) (Default)
    INT                 0x02        // Alert Output Type - INTERUPT   (2)  B00000010 (0x02)

*===============================================================================================================*
    CONFIGURATION REGIASTER (BIT 2): ALERT OUTPUT MODE (0 = ACTIVE-LOW / 1 = ACTIVE-HIGH)
*===============================================================================================================*

    ACTIVE-LOW          0x00        // Alert Polarity Output Mode: ACTIVE-LOW  (0)  B00000000 (0x00) (Default)
    ACTIVE-HIGH         0x04        // Alert Polarity Output Mode: ACTIVE-HIGH (4)  B00000100 (0x04)

*===============================================================================================================*
    CONFIGURATION REGIASTER (BITS 3 & 4): FAULT QUEUE (1 / 2 / 4 / 6 FAULTS)
*===============================================================================================================*

    FQ1                 0x00        // Fault Queue - 1 Fault    B00000000 (0x00) (Default)
    FQ2                 0x08        // Fault Queue - 2 Faults   B00001000 (0x08)
    FQ4                 0x10        // Fault Queue - 4 Faults   B00010000 (0x10)
    FQ6                 0x18        // Fault Queue - 6 Faults   B00011000 (0x18)

*===============================================================================================================*
    CONFIGURATION REGIASTER (BITS 5 & 6): RESOLUTION (9 / 10 / 11 / 12-BIT)
*===============================================================================================================*

     9-BIT              0x00        //  9-Bit Resolution (0.5°C    / 30ms)   B00000000 (0x00) (Default)
    10-BIT              0x20        // 10-Bit Resolution (0.25°C   / 60ms)   B00100000 (0x20)
    11-BIT              0x40        // 11-Bit Resolution (0.125°C  / 120ms)  B01000000 (0x40)
    12-BIT              0x60        // 12-Bit Resolution (0.0625°C / 240ms)  B01100000 (0x60)
 
*===============================================================================================================*
    CONFIGURATION REGIASTER (BIT 7): CONVERSION MODE (0 = CONTINUOUS / 1 = SINGLE-SHOT)
*===============================================================================================================*

    CONT                0x00        // Conversion Mode: CONTINUOUS  (0)  B00000000 (0x00) (Default)
    SINGLE              0x80        // Conversion Mode: SINGLE-SHOT (1)  B10000000 (0x80)

*===============================================================================================================*
    LICENSE
*===============================================================================================================*
 
    The MIT License (MIT)
    Copyright (c) 2016 Nadav Matalon
 
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
    documentation files (the "Software"), to deal in the Software without restriction, including without 
    limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
    the Software, and to permit persons to whom the Software is furnished to do so, subject to the following 
    conditions:
 
    The above copyright notice and this permission notice shall be included in all copies or substantial 
    portions of the Software.
 
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
    LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*==============================================================================================================*/

#if 1
__asm volatile ("nop");
#endif

#ifndef MCP9802_h
#define MCP9802_h

#if !defined(ARDUINO_ARCH_AVR)
    #error “The MCP9802 library only supports AVR processors.”
#endif

#include <Arduino.h>
#include "Wire.h"
#include "utility/MCP9802_PString.h"

namespace Mcp9802 {

    const byte  DEFAULT_CONFIG   =   0;          // Configuration Register Default Settings for 'Continuous' conversion mode (0x00)
    const int   DEFAULT_HYST     =  75;          // Hysteresis Register Default Settings in degrees Celsius (0x9600)
    const int   DEFAULT_LIMIT    =  80;          // Limit Register Default Settings in degrees Celsius (0xA000)
    const byte  CONFIG_BYTE      =   1;          // Number of Configuration Register Bytes (CONFIG)
    const byte  DATA_BYTES       =   2;          // Number of Data Register Bytes (TEMP, HYST, LIMIT)
    const byte  INIT_SINGLE_SHOT = 129;          // Initiates a single conversion in 'Single-Shot' mode (0x81)
    const byte  MIN_CON_TIME     =  30;          // 30ms - Minimal Conversion Time (based on 9-BIT Resolution)
    const byte  COM_SUCCESS      =   0;          // I2C Communication Success (No Error)
    const float MINIMUM_TEMP     = -55;          // Minimum temperature value (in degrees Celsius) for Hysteresis & Limit registers
    const float MAXIMUM_TEMP     = 125;          // Maximum temperature value (in degrees Celsius) for Hysteresis & Limit registers
    const float C_TO_F_CONST     =   0.5555556;  // For faster Convert Degrees Fahrenheit to Celsius

    typedef enum:byte {
        TEMP   = 0,
        CONFIG = 1,
        HYST   = 2,
        LIMIT  = 3
    } reg_ptr_t;

    typedef enum:byte {
        COMP = 0,
        INT  = 2
    } alert_type_t;

    typedef enum:byte {
        ACTIVE_LOW  = 0,
        ACTIVE_HIGH = 4
    } alert_mode_t;

    typedef enum:byte {
        FQ1 = 0,
        FQ2 = 8,
        FQ4 = 16,
        FQ6 = 24
    } fault_queue_t;

    typedef enum:byte {
        RES_9  = 0,
        RES_10 = 32,
        RES_11 = 64,
        RES_12 = 96
    } resolution_t;

    typedef enum:byte {
        CONT   = 0,
        SINGLE = 128
    } con_mode_t;

    typedef enum:byte {
        CELSIUS    = 0,
        FAHRENHEIT = 1
    } temp_unit_t;

    class MCP9802 {
        public:
            MCP9802(byte devAddr);
            ~MCP9802();
            byte   ping();
            byte   getAlertType();
            byte   getAlertMode();
            byte   getFaultQueue();
            byte   getResolution();
            byte   getConMode();
            byte   getTempUnit();
            byte   getComResult();
            float  getTemp();
            float  getHyst();
            float  getLimit();
            float  singleCon();
            void   setAlertType(alert_type_t alertType);
            void   setAlertMode(alert_mode_t alertMode);
            void   setFaultQueue(fault_queue_t fqVal);
            void   setResolution(resolution_t resVal);
            void   setConMode(con_mode_t conMode);
            void   setTempUnit(temp_unit_t newTempUnit);
            void   setHyst(float newHyst);
            void   setLimit(float newLimit);
            void   reset();
        private:
            byte   _devAddr;
            byte   _tempUnit;
            byte   _comBuffer;
            byte   _singleConfig;
            void   initCall(byte ptr);
            void   endCall();
            byte   getConfig();
            float  getData(reg_ptr_t ptr);
            void   setConfig(byte newConfig);
            void   setData(reg_ptr_t ptr, float newData);
            float  convertCtoF(float valC);
            float  convertFtoC(float valF);
            float  roundToHalfDegC(float valC);
            friend MCP9802_PString MCP9802ComStr(const MCP9802&);
            friend MCP9802_PString MCP9802InfoStr(const MCP9802&);
    };

}

using namespace Mcp9802;

#endif
