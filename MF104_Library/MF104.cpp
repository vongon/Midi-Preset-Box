/*
  MF-104.cpp - Library for Moog MF-104 settings class
  Created by Ryan J. McGill, January 16, 2015.
  Released into the public domain.
*/
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
        attribute[0][0] = ccOutputLevel;
        attribute[1][0] = ccTime;
        attribute[2][0] = ccFeedback;
        attribute[3][0] = ccMix;
        attribute[4][0] = ccLFOrate;
        attribute[5][0] = ccLFOamount;
        attribute[6][0] = ccLFOwaveform;
        attribute[7][0] = ccRange;
        attribute[8][0] = ccSlew;
        attribute[9][0] = ccEQ;
        attribute[10][0] = ccDelayTimeMult;
        attribute[11][0] = ccLFOphaseReset;
        attribute[12][0] = ccLFOclockDiv;
        attribute[13][0] = ccTimeClockDiv;
        attribute[14][0] = ccSpillover;
        attribute[15][0] = ccTriplets;
        attribute[16][0] = ccTapDest;
        attribute[17][0] = ccLEDdiv;
        attribute[18][0] = ccBypass;
    }

void MF104::sendSettings()
    {
        for(int i = 0; i < totalAttributes; i++)
        {
            Serial.write(ccMIDI); Serial.write(attribute[i][0]); Serial.write(attribute[i][1]);
        }
    }
void MF104::setValue(int parameter, uint8_t value)
    {
        attribute[parameter-1][1] = value;
        Serial.write(ccMIDI); Serial.write(attribute[parameter-1][0]); Serial.write(attribute[parameter-1][1]);
    }
uint8_t MF104::getValue(int parameter)
    {
        return attribute[parameter-1][1];
    }
void MF104::storeData(int address)
    {
        for(int i = 0; i < totalAttributes; i++)
        {
            EEPROM.write(address + i, getValue(i + 1)); //i+1 because we started with 0
        }
    }
void MF104::loadData(int address)
    {
        for(int i = 0; i < totalAttributes; i++)
        {
            setValue(i + 1, EEPROM.read(address + i));
        }
    }
