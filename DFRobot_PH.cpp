/*
 * file DFRobot_PH.cpp * @ https://github.com/DFRobot/DFRobot_PH
 *
 * Arduino library for Gravity: Analog pH Sensor / Meter Kit V2, SKU: SEN0161-V2
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-04
 */


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DFRobot_PH.h"
#include <EEPROM.h>

#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

#define PHVALUEADDR 0x00    //the start address of the pH calibration parameters stored in the EEPROM


DFRobot_PH::DFRobot_PH(int device)
{
    this->_temperature    = 25.0;
    this->_phValue        = 7.0;
    this->_acidVoltage    = 2032.44;    //buffer solution 4.0 at 25C
    this->_neutralVoltage = 1500.0;     //buffer solution 7.0 at 25C
    this->_voltage        = 1500.0;
    this->mem_offset      = device*DEVICE_MEM_OFFSET;
}

DFRobot_PH::~DFRobot_PH()
{

}

void DFRobot_PH::begin(bool safe)
{
    Serial.print("[DFRobot_PH] original _neturalVoltage:");
    Serial.println(this->_neutralVoltage);
    EEPROM_read(PHVALUEADDR+this->mem_offset, this->_neutralVoltage);  //load the neutral (pH = 7.0)voltage of the pH board from the EEPROM
    Serial.print("[DFRobot_PH] _neutralVoltage:");
    Serial.println(this->_neutralVoltage);
    if(EEPROM.read(PHVALUEADDR + this->mem_offset)==0xFF && EEPROM.read(PHVALUEADDR+1+this->mem_offset)==0xFF && EEPROM.read(PHVALUEADDR+2+this->mem_offset)==0xFF && EEPROM.read(PHVALUEADDR+3+this->mem_offset)==0xFF){
        this->_neutralVoltage = 1500.0;  // new EEPROM, write typical voltage
        EEPROM_write(PHVALUEADDR+this->mem_offset, this->_neutralVoltage);
    }
    EEPROM_read(PHVALUEADDR+4+this->mem_offset, this->_acidVoltage);//load the acid (pH = 4.0) voltage of the pH board from the EEPROM
    Serial.print("[DFRobot_PH] _acidVoltage:");
    Serial.println(this->_acidVoltage);
    if(EEPROM.read(PHVALUEADDR+4+this->mem_offset)==0xFF && EEPROM.read(PHVALUEADDR+5+this->mem_offset)==0xFF && EEPROM.read(PHVALUEADDR+6+this->mem_offset)==0xFF && EEPROM.read(PHVALUEADDR+7+this->mem_offset)==0xFF){
        this->_acidVoltage = 2032.44;  // new EEPROM, write typical voltage
        EEPROM_write(PHVALUEADDR+4+this->mem_offset, this->_acidVoltage);
    }
    this->safe = safe;
    // Serial.println("\n [Enter anything to calibrate]");
}

float DFRobot_PH::readPH(float voltage, float temperature)
{
    float slope = (7.0-4.0)/((this->_neutralVoltage-1500.0)/3.0 - (this->_acidVoltage-1500.0)/3.0);  // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    float intercept =  7.0 - slope*(this->_neutralVoltage-1500.0)/3.0;
    //Serial.print("slope:");
    //Serial.print(slope);
    //Serial.print(",intercept:");
    //Serial.println(intercept);
    this->_phValue = slope*(voltage-1500.0)/3.0+intercept;  //y = k*x + b
    return _phValue;
}

void DFRobot_PH::setValuesToCalibrate(float voltage, float temperature)
{
    this->_voltage = voltage;
    this->_temperature = temperature;
}

bool DFRobot_PH::isNeutral() {
    return (this->_voltage>1322)&&(this->_voltage<1678);
}

bool DFRobot_PH::calibrateNeutral() {
    if (isNeutral() || !safe){        // buffer solution:7.0{
        Serial.println(F("\n>>>Buffer Solution:7.0"));
        Serial.print(F("Voltage: "));
        Serial.println(this->_voltage, 3);
        this->_neutralVoltage = this->_voltage;
        saveNeutral();
        return true;
    } else {
        Serial.print(this->_voltage, DEC);
        Serial.println(" out of range (1322, 1678). Not pH 7.0");
        return false;
    }
}

bool DFRobot_PH::isAcidic() {
    return (this->_voltage>1854)&&(this->_voltage<2210);
}


bool DFRobot_PH::calibrateAcidic() {
    if (isAcidic() || !safe){  //buffer solution:4.0
        Serial.println(F("\n>>>Buffer Solution:4.0"));
        Serial.print(F("Voltage: "));
        Serial.println(this->_voltage, 3);
        this->_acidVoltage =  this->_voltage;
        saveAcidic();
        return true;
    }  else {
        Serial.print(this->_voltage, DEC);
        Serial.println(" out of range (1854, 2210). Not pH 4.0");
        return false;
    }
}

void DFRobot_PH::saveNeutral() {
    EEPROM_write(PHVALUEADDR+this->mem_offset, this->_neutralVoltage);
}

void DFRobot_PH::saveAcidic() {
    EEPROM_write(PHVALUEADDR+4+this->mem_offset, this->_acidVoltage);
}

void DFRobot_PH::showCalibration() {
    Serial.print(" neutralVoltage (pH 7): ");
    Serial.println(this->_neutralVoltage, 5);
    Serial.print(" acidVoltage (pH 4): ");
    Serial.println(this->_acidVoltage, 5);
}

void DFRobot_PH::setCalibration(float neutralVoltage, float acidVoltage) {
    this->_neutralVoltage = neutralVoltage;
    this->_acidVoltage = acidVoltage;
    EEPROM_write(PHVALUEADDR+this->mem_offset, this->_neutralVoltage);
    EEPROM_write(PHVALUEADDR+4+this->mem_offset, this->_acidVoltage);
}

