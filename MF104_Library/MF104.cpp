#include "Arduino.h"
#include "MF104.h"
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

#define totalAttributes 19
#define ccMIDI          0xB0
#define ccOutputLevel   7
#define ccTime          12
#define ccFeedback      13
#define ccMix           14
#define ccLFOrate       15
#define ccLFOamount     16
#define ccLFOwaveform   102
#define ccRange         86
#define ccSlew          5
#define ccEQ            85
#define ccDelayTimeMult 87
#define ccLFOphaseReset 105
#define ccLFOclockDiv   107
#define ccTimeClockDiv  90
#define ccSpillover     82
#define ccTriplets      88
#define ccTapDest       116
#define ccLEDdiv        118
#define ccBypass        80
  


MF104::MF104()
    {
        parameters[0][0] = ccOutputLevel;
        parameters[0][1] = 60;
        parameterNames[0] = "OutputLvl";

        // parameters[1][0] = ccTime;
        // parameters[1][2] = "Dly Time";

        // parameters[2][0] = ccFeedback;
        // parameters[1][2] = "Feedback";

        // parameters[3][0] = ccMix;
        // parameters[3][2] = "Mix";

        // parameters[4][0] = ccLFOrate;
        // parameters[4][2] = "LFO Rate";

        // parameters[5][0] = ccLFOamount;
        // parameters[5][2] = "LFO Amount";        

        // parameters[6][0] = ccLFOwaveform;
        // parameters[6][2] = "LFO Waveform";

        // parameters[7][0] = ccRange;
        // parameters[7][2] = "Dly Time Range";

        // parameters[8][0] = ccSlew;
        // parameters[8][2] = "Time Slew Rate";

        // parameters[9][0] = ccEQ;
        // parameters[9][2] = "EQ";

        // parameters[10][0] = ccDelayTimeMult;
        // parameters[10][2] = "DlyTimeMultiply";

        // parameters[11][0] = ccLFOphaseReset;
        // parameters[11][2]

        // parameters[12][0] = ccLFOclockDiv;
        // parameters[13][0] = ccTimeClockDiv;
        // parameters[14][0] = ccSpillover;
        // parameters[15][0] = ccTriplets;
        // parameters[16][0] = ccTapDest;
        // parameters[17][0] = ccLEDdiv;
        parameters[18][0] = ccBypass;
    }

void MF104::sendSettings()
    {
        for(int i = 0; i < totalAttributes; i++)
        {
            Serial.write(ccMIDI); Serial.write(parameters[i][0]); Serial.write(parameters[i][1]);
        }
    }
void MF104::setValue(int parameterIdx, uint8_t value)
    {
        parameters[parameterIdx][1] = value;
        Serial.write(ccMIDI); Serial.write(parameters[parameterIdx][0]); Serial.write(parameters[parameterIdx][1]);
    }
uint8_t MF104::getValue(int parameterIdx)
    {
        return parameters[parameterIdx][1];
    }
uint8_t MF104::getCC(int parameterIdx)
    {
        return parameters[parameterIdx][0];
    }
void MF104::storeData(int address)
    {
        for(int i = 0; i < totalAttributes; i++)
        {
            EEPROM.write(address + i, getValue(i)); //i+1 because we started with 0
        }
    }
void MF104::loadData(int address)
    {
        for(int i = 0; i < totalAttributes; i++)
        {
            setValue(i, EEPROM.read(address + i));
        }
    }
