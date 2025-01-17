/*
 * file DFRobot_PH.h * @ https://github.com/DFRobot/DFRobot_PH
 *
 * Arduino library for Gravity: Analog pH Sensor / Meter Kit V2, SKU: SEN0161-V2
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-04
 */

#ifndef _DFROBOT_PH_H_
#define _DFROBOT_PH_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define ReceivedBufferLength 10  //length of the Serial CMD buffer

#define DEVICE_MEM_OFFSET 8

class DFRobot_PH
{
public:
    DFRobot_PH(byte device = 0);
    ~DFRobot_PH();
    void    setValuesToCalibrate(float voltage);//, float temperature);
    float   readPH(float voltage, float temperature); // voltage to pH value, with temperature compensation
    void    begin(bool safe=true);   //initialization

    bool    isAcidic();
    bool    isNeutral();
    bool    calibrateAcidic();
    bool    calibrateNeutral();

    void    showCalibration();
    void    setCalibration(float neutralVoltage, float acidVoltage);

private:
    //float  _phValue;
    float  _acidVoltage;
    float  _neutralVoltage;
    float  _voltage;
    float  _temperature;

    void    saveAcidic();
    void    saveNeutral();

private:
    void    phCalibration(byte mode); // calibration process, wirte key parameters to EEPROM
    int     mem_offset; // for multiple devices operation)
    bool    safe;
};

#endif
