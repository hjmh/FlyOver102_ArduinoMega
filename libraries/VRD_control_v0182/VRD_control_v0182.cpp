/*
 *  VRD_control_v0182.h
 *  
 *
 *  Created by Hannah Haberkern on 4/5/12.
 *  Copyright 2012 ETH Zürich. All rights reserved.
 *  Updated 19/11/2015 by Hannah
 *
 */

#ifndef VRD_control_cpp
#define VRD_control_cpp
#include <SPI.h>// include the SPI library code
#include <VRD_control_v0182.h>
#include <VRD_setup_v0182.h>
#endif

//===============
//Constructor
VRD_control::VRD_control(long timeOutInterval, int dacScreenLED, int dacBasler, int dacIRLaser, int calibrationPeriod_Hz, int calibrationPulseTime, int dac2pin)
{
	_timeOutInterval = timeOutInterval; //interval w/o input after which outputs are truned off
	_timeOutTimer = 0;
	_timeOutStarted = 0;
	
	_dacScreenLED = dacScreenLED;
	_dacBasler = dacBasler;
	_dacIRLaser = dacIRLaser;
	
	_calibrationPeriod_Hz = calibrationPeriod_Hz;
	_calibrationPulseTime = calibrationPulseTime;

	_dac2pin = dac2pin;
}


//===============
//Public functions

void VRD_control::initializeDacVariables(int dacScreenLED, int dacBasler, int dacIRLaser)
{
	_dacScreenLED = dacScreenLED;
	_dacBasler = dacBasler;
	_dacIRLaser = dacIRLaser;
}


//---------------

void VRD_control::performCalibration()
{
	VRD_control::TTLpulseWhile();
}

//---------------

void VRD_control::beginRecordingTrialVideo(int trialVideoFramerate_Hz, int videoRecoding_ms)
{
	int stateCameraTrigger = VRD_control::startCameraTrigger(trialVideoFramerate_Hz, videoRecoding_ms);
}

//---------------

int VRD_control::screensOn()
{
	dac2Write(_dacScreenLED, 32767);// set the DAC output to 5V
	//lock3 = STATE_CLOSED;//close the lock
	return 1; // state SCREENS_ON
}



int VRD_control::screensOff()
{
	dac2Write(_dacScreenLED, 0);// set the DAC output to 0V
	return 0; // state SCREENS_OFF
}

//---------------

int VRD_control::baslerOn()
{
	dac2Write(_dacBasler, 32767);// set the DAC output to 5V
	return 1; // state BASLER_ON
}

int VRD_control::baslerOff()
{
	dac2Write(_dacBasler, 0);// set the DAC output to 0V
	return 0; // state BASLER_OFF
}

//---------------

int VRD_control::laserOn(int dutyCycle)
{
	// range of laser duty cycle is from 0 - 255
	//int PMWval = (int) (dutyCycle*255)/100.0; //The variable dutyCycle defines the duty cycle in %
	int PMWval = (int) dutyCycle;
	analogWrite(_dacIRLaser, PMWval);//Set the PWM to high, pretty much 5 volts
	
	return 1; //state LASER=ON
}

int VRD_control::laserOff()
{
	analogWrite(_dacIRLaser, 0);
	return 0; //state LASER_OFF	
}


void VRD_control::dac2Write(uint8_t dac, int16_t value)
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

//==============
//private

int VRD_control::initiateReward(int pin, int dutyValue)
{
	/*This function is called when a reward is initiated
	 pin  -  connection to IR laser
	 value  -  duty cycle (minimum 0 -> 0% duty cycle, maximum 255 -> 100% duty cycle)
	 */
	
    analogWrite(pin, dutyValue); 
	
	return 2;
}

//---------------
void VRD_control::TTLpulseWhile()
{
	//This function sends a TTL when 'c' is sent to the Arduino
	//Can be depreciated if I can find the other PWM on the board

	long startTimeTTL = millis();
	float halfPeriod = ((1/_calibrationPeriod_Hz)*500);
	
	while(millis()-startTimeTTL < _calibrationPulseTime) { // do this for 10 seconds (10000), or 1 s (1000)
        
		dac2Write(2, 32767);// set the DAC output to high (TTL_PULSE_HIGH = 32767 --> about 5V)
        long startPeriod1=millis();
        
		while(millis()- startPeriod1 < halfPeriod){
			//wait for 1/2 calibrationPeriod
		};
		
        dac2Write(2, 0);// set the DAC output to low (TTL_PULSE_LOW = 0 --> 0V)
		long startPeriod2=millis();
		
        while(millis()-startPeriod2 < halfPeriod){
			//wait for 1/2 calibrationPeriod
		};
	};
}


int VRD_control::startCameraTrigger(int trialVideoFramerate_Hz, int videoRecoding_ms)
{
	/*
	This function generates a TTL pulse signal to trigger a camera to capture single frames during a VR trial.
	*/
	
	int stateTrialVideo = 1;
	
	long startTimeTTL = millis();
	float halfPeriod = ((1/trialVideoFramerate_Hz)*500);
	
	while(millis()-startTimeTTL < videoRecoding_ms) { // do this for 10 seconds (10000), or 1 s (1000)
        
		dac2Write(2, 32767);// set the DAC output to high (TTL_PULSE_HIGH = 32767 --> about 5V)
        long startPeriod1=millis();
        
		while(millis()- startPeriod1 < halfPeriod){
			//wait for 1/2 calibrationPeriod
		};
		
        dac2Write(2, 0);// set the DAC output to low (TTL_PULSE_LOW = 0 --> 0V)
		long startPeriod2=millis();
		
        while(millis()-startPeriod2 < halfPeriod){
			//wait for 1/2 calibrationPeriod
		};
	};
	stateTrialVideo = 0;
	
	return stateTrialVideo;
}