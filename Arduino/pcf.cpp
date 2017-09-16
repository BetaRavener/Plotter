#include "pcf.h"
#include <Wire.h>

PCF::PCF(uint8_t address)
{
    _address = address;
}

void PCF::write(uint8_t byte)
{
    Wire.beginTransmission(_address);
    Wire.write(byte);
    Wire.endTransmission();
}