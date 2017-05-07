/*
 *  VRD_setup_v0182.cpp
 *    --> runs with Arduino version 1.8.2
 *
 *  Created by Hannah Haberkern on 4/6/12.
 *  Copyright 2012 ETH Zürich. All rights reserved.
 *
 */

#ifndef VRD_setup_cpp
#define VRD_setup_cpp
#include <SPI.h>// include the SPI library code
#include <VRD_setup_v0182.h>
#endif

//constructor
VRD_setup::VRD_setup(int lowestPin, int highestPin, int dac2pin)
{
	_lowestPin = lowestPin;
	_highestPin = highestPin;
	_dac2pin = dac2pin;
}

void VRD_setup::initializePins(int LEDpin)
{
	
	for (int thisPin = _lowestPin; thisPin <= _highestPin; thisPin++)
	{ 
		pinMode(thisPin, OUTPUT); 
	} 
	
	//Set other pins
	pinMode(LEDpin, OUTPUT); // Green LED on the front
	pinMode(_dac2pin, OUTPUT); //Need to send out TTL signal
	digitalWrite(LEDpin, LOW);
	digitalWrite(_dac2pin, HIGH);
}

void VRD_setup::initializeSPI()
{
	/*initialize SPI; set dac variables & control register, set all dac outputs to 0 */
	
	// initialize SPI:
	SPI.begin();
	// use SPI clock mode 2
	SPI.setDataMode(SPI_MODE2);
	// set clock mode to FCLK/4
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	
	//Set DAC variables............................................
	//DAC 
	digitalWrite(_dac2pin, LOW);
	SPI.transfer(0x10);
	SPI.transfer(0x00);
	SPI.transfer(0x1f);
	digitalWrite(_dac2pin, HIGH);
	
	// DAC control register (SDO turned off)
	digitalWrite(_dac2pin, LOW);
	SPI.transfer(0x19); // write to control register
	SPI.transfer(0x00);
	SPI.transfer(0x0d); // TSD (thermal shutdown) enable, Clamp (current limit) enable, Clear to 0V, SDO (serial data out) disabled
	digitalWrite(_dac2pin, HIGH);  
	
	/* DAC output range register (all ch +/-5V range)*/
	digitalWrite(_dac2pin, LOW);
	SPI.transfer(0x0c); // all four DACs
	
	// 0x08 = DAC1, 0x09 = DAC2, 0x0a = DAC3, 0x0b = DAC4, 0x0c = all DACs
	SPI.transfer(0x00);
	SPI.transfer(0x03);
	
	// 0 = +5V range, 1 = +10V range, 2 = +10.8V range, 3 = +/-5V range
	// 4 = +/-10V range, 5 = +/- 10.8V range
	digitalWrite(_dac2pin, HIGH);
	
	//Set outputs to 0V............................................
	dac2Write(0, 0);
	dac2Write(1, 0);
	dac2Write(2, 0);
	dac2Write(3, 0);
}

/*communication with LCD display (clear, write something) */
void VRD_setup::LCDcommunication(String message)
{
	//Serial serial_LCD = Serial1;
	Serial1.begin(19200);
	Serial1.print(0x0c); // clear the display
	Serial1.print(0x11); // Back-light on
	Serial1.print(message);
}

void VRD_setup::dac2Write(uint8_t dac, int16_t value)
{
	// AD5754 DAC write routine
	// dac = dac output channel, 0 - 3
	// value = 16 bit output value
	digitalWrite(_dac2pin, LOW);
	SPI.transfer(dac);
	SPI.transfer((value & 0xff00)>>8);
	SPI.transfer((value & 0x00ff));
	digitalWrite(_dac2pin, HIGH);
	digitalRead(_dac2pin);
}