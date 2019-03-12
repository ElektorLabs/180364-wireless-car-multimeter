/*
The MIT License (MIT)

Copyright (c) 2016 Sindre Halbj�rhus - sindre@ihemsedal.no

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef _DS2401_h
#define _DS2401_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <OneWire.h>

class DS2401
{
public:
	DS2401(OneWire* _oneWire);
	bool init();
	bool isDS2401();
	String GetSerial();
    void GetSerial( uint8_t* Data, uint8_t len);
	void Refresh();

private:
	OneWire* _wire;
	void GetData();
	void IsCRCValid();
	void DS2401Present();
	
	byte i;
	byte _data[8];
	byte _crcByte;
	byte _crcCalc;
	bool _crcValid;
	bool _ds2401Present;
	bool _GotData;

protected:


};


#define DS2401_READ_ROM_COMMAND		0x33
#define DS2401_FAMILY_CODE			0x01

#endif

