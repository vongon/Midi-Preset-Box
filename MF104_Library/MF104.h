/*
  MF-104.h - Library for Moog MF-104 settings class
  Created by Ryan J. McGill, January 16, 2015.
  Released into the public domain.
*/
#ifndef MF104_h
#define MF104_h

#include "Arduino.h"
#include <EEPROM.h>

class MF104
{
  public:
    MF104();
    void sendSettings();
    void setValue(int parameter, uint8_t value);
    uint8_t getValue(int parameter);
    uint8_t getCC(int parameter);
    void storeData(int address);
    void loadData(int address);
 private:
    uint8_t parameters[19][2];
    const char *parameterNames[19];
};

#endif