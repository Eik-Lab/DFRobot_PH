/*
 * file DFRobot_PH.ino
 * @ https://github.com/DFRobot/DFRobot_PH
 *
 * This is the sample code for Gravity: Analog pH Sensor / Meter Kit V2, SKU:SEN0161-V2
 * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *   enterph -> enter the calibration mode
 *   calph   -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
 *   exitph  -> save the calibrated parameters and exit from calibration mode
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-04
 */

#include "DFRobot_PH.h"
#include <EEPROM.h>

#define PH_PIN A0
float voltage, phValue;
DFRobot_PH ph(1);

String input_str;

void setup() {
    Serial.begin(9600);
    ph.begin(false); // safe=false (skip voltage range checks)
}

void loop() {
    static unsigned long timepoint = millis();
    voltage = analogRead(PH_PIN)/1024.0*5000;
    phValue = ph.readPH(voltage, NULL);
    Serial.print(phValue, 3);
    Serial.print(" (");
    Serial.print(voltage, 3);
    Serial.println(")");

    delay(1000);
}

void serialEvent() {
    if (Serial.available() > 0) {
        input_str = Serial.readString();
        ph.calibration(voltage, NULL); // pass voltage to library
        switch (input_str.c_str()[0]) {
            case '4':
                ph.calibrateAcidic();
            case '7':
                ph.calibrateNeutral();
            default:
                Serial.println("Command doesn't start with \"4\" or \"7\"");
        }
    }
}
