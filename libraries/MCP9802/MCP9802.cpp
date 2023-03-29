/*==============================================================================================================*

    @file     MCP9802.cpp
    @author   Nadav Matalon
    @license  MIT (c) 2016 Nadav Matalon
 
    MCP9802 Driver (12-BIT Temperature Sensor with I2C Interface)
 
    Ver. 1.0.0 - First release (23.9.16)
    Ver. 1.1.0 - Small change in functionality: attempting to set hysteresis or limit beyond the legitimate range
                 (-55°C - 125°C / -67°F - 257°F) now sets the register to the maximum/minumum allowable value 
                 rather than do nothing (4.10.16)
    Ver. 1.2.0 - Changed license to MIT (5.10.16)
    Ver. 1.3.0 - Changed auxilliary functions: MCP9802InfoStr() and MCP9802ComStr() to work with the PString class
                 instead of the String class to further reduce memory footprint. To this end, added PString.h and
                 PString.cpp files to /utility folder. In addition added "I2C STATUS" (CONNECTED / NOT CONNECTED) 
                 field to device information string (9.10.16)
    Ver. 1.4.0 - Added namespaces to prevent conflicts with other libraries (15.10.16)
    Ver. 1.5.0 - Switched from 'WSWire' library to 'Wire' library for I2C communications (24.10.16)

 *==============================================================================================================*
    LICENSE
 *==============================================================================================================*
 
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

#include "MCP9802.h"

/*==============================================================================================================*
    CONSTRUCTOR
 *==============================================================================================================*/

MCP9802::MCP9802(byte devAddr) {
    _devAddr      = devAddr;
    _tempUnit     = CELSIUS;
    _comBuffer    = ping();
    _singleConfig = DEFAULT_CONFIG;
}

/*==============================================================================================================*
    DESTRUCTOR
 *==============================================================================================================*/

MCP9802::~MCP9802() {}

/*==============================================================================================================*
    PING (0 = SUCCESS / 1-7 = ERROR CODE)
 *==============================================================================================================*/

// See explication of error code numbers in the README

byte MCP9802::ping() {
    Wire.beginTransmission(_devAddr);
    return Wire.endTransmission();
}

/*==============================================================================================================*
    GET ALERT OUTPUT TYPE (0 = COMPARATOR / 1 = INTERRUPT)
 *==============================================================================================================*/

byte MCP9802::getAlertType() {
    return bitRead(getConfig(), 1);
}

/*==============================================================================================================*
    GET ALERT OUTPUT MODE (0 = ACTIVE-LOW / 1 = ACTIVE-HIGH)
 *==============================================================================================================*/

byte MCP9802::getAlertMode() {
    return bitRead(getConfig(), 2);
}

/*==============================================================================================================*
    GET FAULT-QUEUE SETTINGS (1 / 2 / 4 / 6 FAULT/S)
 *==============================================================================================================*/

byte MCP9802::getFaultQueue() {
    byte fqVal = ((getConfig() & 0x18) >> 2);
    if (!fqVal) fqVal = 1;
    return fqVal;
}

/*==============================================================================================================*
    GET RESOLUTION SETTINGS (9 / 10 / 11 / 12-BIT)
 *==============================================================================================================*/

byte MCP9802::getResolution() {
    return ((getConfig() & 0x60) >> 5) + 9;
}

/*==============================================================================================================*
    GET CONVERSION MODE (0 = CONTINUOUS / 1 = SINGLE-SHOT)
 *==============================================================================================================*/

byte MCP9802::getConMode() {
    return bitRead(getConfig(), 0);
}

/*==============================================================================================================*
    GET TEMPERATURE UNIT (0 = CELSIUS / 1 = FAHRENHEIT)
 *==============================================================================================================*/

byte MCP9802::getTempUnit() {
    return _tempUnit;
}

/*==============================================================================================================*
    GET COMMUNICATION RESULT
 *==============================================================================================================*/

byte MCP9802::getComResult() {
    return _comBuffer;
}

/*==============================================================================================================*
    GET AMBIENT TEMPERATURE
 *==============================================================================================================*/

float MCP9802::getTemp() {
    return getData(TEMP);
}

/*==============================================================================================================*
    GET HYSTERESIS REGISTER VALUE
 *==============================================================================================================*/

float MCP9802::getHyst() {
    return getData(HYST);
}

/*==============================================================================================================*
    GET LIMIT REGISTER VALUE
 *==============================================================================================================*/

float MCP9802::getLimit() {
    return getData(LIMIT);
}

/*==============================================================================================================*
    SINGLE CONVERSION ('SINGLE-SHOT' MODE ONLY)
 *==============================================================================================================*/
//  Conversion Time: 9-BIT = 40ms / 10-BIT = 70ms / 11-BIT = 130ms / 12-BIT = 250ms

float MCP9802::singleCon() {
    if (_singleConfig) {
        unsigned int conTime = MIN_CON_TIME * (1 << ((_singleConfig & 0x60) >> 5)) + 10;
        setConfig(_singleConfig | INIT_SINGLE_SHOT);
        delay(conTime);
        return getTemp();
    }
}

/*==============================================================================================================*
    SET ALERT OUTPUT TYPE (0 = COMPARATOR / 1 = INTERRUPT)
 *==============================================================================================================*/

void MCP9802::setAlertType(alert_type_t alertType) {                    // PARAMS: COMP / INT
    byte newConfig = getConfig();
    alertType ? bitSet(newConfig, 1) : bitClear(newConfig, 1);
    setConfig(newConfig);
}

/*==============================================================================================================*
    SET ALERT OUTPUT MODE (0 = ACTIVE-LOW / 1 = ACTIVE-HIGH)
 *==============================================================================================================*/

void MCP9802::setAlertMode(alert_mode_t alertMode) {                    // PARAMS: ACTIVE-LOW / ACTIVE-HIGH
    byte newConfig = getConfig();
    alertMode ? bitSet(newConfig, 2) : bitClear(newConfig, 2);
    setConfig(newConfig);
}

/*==============================================================================================================*
    SET FAULT-QUEUE SETTINGS (1 / 2 / 4 / 6 FAULT/S)
 *==============================================================================================================*/

void MCP9802::setFaultQueue(fault_queue_t fqVal) {                      // PARAMS: FQ1 / FQ2 / FQ4 / FQ6
    byte newConfig = getConfig();
    for (byte j=4; j>2; j--) bitClear(newConfig, j);
    newConfig |= (byte)fqVal;
    setConfig(newConfig);
}

/*==============================================================================================================*
    SET RESOLUTION SETTINGS (9 / 10 / 11 / 12-BIT)
 *==============================================================================================================*/

void MCP9802::setResolution(resolution_t resVal) {                 // PARAMS: RES_9 / RES_10 / RES_11 // RES_12
    byte newConfig = getConfig();
    for (byte j=6; j>4; j--) bitClear(newConfig, j);
    newConfig |= (byte)resVal;
    setConfig(newConfig);
}

/*==============================================================================================================*
    SET CONVERSION MODE (CONTINUOUS / SINGLE-SHOT)
 *==============================================================================================================*/

void MCP9802::setConMode(con_mode_t conMode) {                    // PARAMS: CONT / SINGLE
    byte newConfig = getConfig();
    bitClear(newConfig, 7);
    conMode ? bitSet(newConfig, 0) : bitClear(newConfig, 0);
    _singleConfig = conMode ? newConfig : 0;
    setConfig(newConfig);
}

/*==============================================================================================================*
    SET TEMPERATURE UNIT
 *==============================================================================================================*/

void MCP9802::setTempUnit(temp_unit_t newTempUnit) {          // (PARAMS: CELSIUS / FAHRENHEIT)
    _tempUnit = newTempUnit;
}

/*==============================================================================================================*
    SET HYSTERESIS REGISTER VALUE (-55°C - 125°C / -67°F - 257°F)
 *==============================================================================================================*/

void MCP9802::setHyst(float newHyst) {
    setData(HYST, newHyst);
}

/*==============================================================================================================*
    SET LIMIT REGISTER VALUE (-55°C - 125°C / -67°F - 257°F)
 *==============================================================================================================*/

void MCP9802::setLimit(float newLimit) {
    setData(LIMIT, newLimit);
}

/*==============================================================================================================*
    RESET
 *==============================================================================================================*/

void MCP9802::reset() {
    _singleConfig = CONT;
    setTempUnit(CELSIUS);
    setConfig(DEFAULT_CONFIG);
    setHyst(DEFAULT_HYST);
    setLimit(DEFAULT_LIMIT);
}

/*==============================================================================================================*
    INITIATE I2C COMMUNICATION
 *==============================================================================================================*/

void MCP9802::initCall(byte ptr) {
    Wire.beginTransmission(_devAddr);
    Wire.write(ptr);
}

/*==============================================================================================================*
    END I2C COMMUNICATION
 *==============================================================================================================*/

void MCP9802::endCall() {
    _comBuffer = Wire.endTransmission();
}

/*==============================================================================================================*
    GET CONFIGURATION REGISTER
 *==============================================================================================================*/

byte MCP9802::getConfig() {
    byte config;
    initCall(CONFIG);
    endCall();                              // check if needed
    if (_comBuffer == COM_SUCCESS) {
        Wire.requestFrom(_devAddr, CONFIG_BYTE);
        if (Wire.available() == CONFIG_BYTE) config = Wire.read();
    }
    return config;
}

/*==============================================================================================================*
    GET DATA REGISTERS VALUE (TEMPERATURE / HYSTERESIS / LIMIT)
 *==============================================================================================================*/

float MCP9802::getData(reg_ptr_t ptr) {                                          // PARAMS: TEMP / HYST / LIMIT
    float data = 0;
    initCall(ptr);
    endCall();
    if (_comBuffer == COM_SUCCESS) {
        Wire.requestFrom(_devAddr, DATA_BYTES);
        if (Wire.available() == DATA_BYTES) data = (((Wire.read() << 8) | (Wire.read())) / 256.0);
    }
    return _tempUnit ? convertCtoF(data) : data;
}

/*==============================================================================================================*
    SET CONFIGURATION REGISTER
 *==============================================================================================================*/

void MCP9802::setConfig(byte newConfig) {
    initCall(CONFIG_BYTE);
    Wire.write(newConfig);
    endCall();
}

/*==============================================================================================================*
    SET DATA REGISTERS VALUE (HYSTERESIS / LIMIT)
 *==============================================================================================================*/

void MCP9802::setData(reg_ptr_t ptr, float newData) {                                 // PARAMS: HYST / LIMIT
    union Data_t { int i; byte b[2]; } data16;
    if (_tempUnit) newData = convertFtoC(newData);
    if (ptr == HYST || ptr == LIMIT) {
        newData = constrain(newData, MINIMUM_TEMP, MAXIMUM_TEMP);
        data16.i = (int)(roundToHalfDegC(newData) * 256.0);
        initCall(ptr);
        for (byte j=2; j>0; j--) Wire.write(data16.b[j-1]);
        endCall();
    }
}

/*==============================================================================================================*
    CONVERT DEGREES CELSIUS TO FAHRENHEIT
 *==============================================================================================================*/

float MCP9802::convertCtoF(float valC) {
    return (valC * 1.8 + 32);
}

/*==============================================================================================================*
    CONVERT DEGREES FAHRENHEIT TO CELSIUS
 *==============================================================================================================*/

float MCP9802::convertFtoC(float valF) {
    return ((valF - 32) * C_TO_F_CONST);
}

/*==============================================================================================================*
    ROUND TO NEAREST 0.5 DEGREES CELSIUS
 *==============================================================================================================*/

float MCP9802::roundToHalfDegC(float valC) {
    if (valC > 0) return (int)((valC * 2.0) + 0.5) / 2.0;
    else if (valC < 0) return (int)((valC * 2.0) - 0.5) / 2.0;
    else return 0;
}
