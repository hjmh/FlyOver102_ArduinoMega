/*
 *  VRD_setup_v0182.h  --> runs with Arduino v 1.8.2
 *  Library for various functions used
 *	to set up the Arduino-mediated comunication.
 *
 *  Created by Hannah Haberkern on 4/5/12.
 *  Updated 07/05/2017 by Hannah Haberkern
 *
 */

#ifndef VRD_setup_h
#define VRD_setup_h
#include <SPI.h>// include the SPI library code
//#include <Arduino.h>

class VRD_setup
	{
	public:
		
		VRD_setup(int lowestPin, int highestPin, int dac2pin);
		
		void initializePins(int LEDpin);
		
		//initialize SPI; set dac variables & control register, set all dac outputs to 0
		void initializeSPI();
		
		//communication with LCD display (clear, write something)
		void LCDcommunication(String message);
		
		void dac2Write(uint8_t dac, int16_t value);
		
	private:
		int _lowestPin;
		int _highestPin;
		int _dac2pin;
	};
#endif