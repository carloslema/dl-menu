#ifndef _NUMBER_H
#define _NUMBER_H

#include <EEPROM.h>
#include <Arduino.h>
#include "DLMenu.h"

#define SIZE 4 // size of float, unsigned int, etc.

class DLNumber
{
  public:
    DLNumber(int address);

    float getFloatValue(void);
    unsigned long getUintValue(void);

    void setValue(float f);
    void setValue(unsigned long l);

  private:
    int address;

    union {
        float f;
        byte b[SIZE];
        unsigned long l;
    } data;

    void save(void);
    bool load(void);
};

#endif
