/*
 *  VRD_control_v0182.h  --> runs with Arduino v 1.8.2
 *  Library for various functions used to control the VR via serial 
 *  communication from the VR and Data PC
 *
 *  Created by Hannah Haberkern on 4/5/12.
 *  Updated 07/05/2017 by Hannah Haberkern
 *
 */

#ifndef VRD_control_h
#define VRD_control_h
#include <SPI.h>// include the SPI library code
#include <VRD_setup_v0182.h>

class VRD_control
	{
	public:
	  //variables
		 
	  //functions
		
		VRD_control(long timeOutInterval, int dacScreenLED, int dacBasler, int dacIRLaser, int calibrationPeriod_Hz, int calibrationPulseTime, int dac2pin);
		
		void initializeDacVariables(int dacScreenLED, int dacBasler, int dacIRLaser);
		
		void performCalibration();
		
		void beginRecordingTrialVideo(int trialVideoFramerate_Hz, int videoRecoding_ms);
		
		//void stopRecordingTrialVideo();
		
		int screensOn();
		int screensOff();
		
		int baslerOn();
		int baslerOff();
		
		int optLEDOn(int dutyCycle);
		int optLEDOff();
		
		void dac2Write(uint8_t dac, int16_t value);
		
	private:
	  //variables
		int _dacScreenLED;
		int _dacBasler;
		int _dacOptLED;

		int _dac2pin;
		
		long _timeOutInterval;
		long _timeOutTimer;
		int _timeOutStarted;
		
		int _calibrationPeriod_Hz;
		int _calibrationPulseTime;
		
	  //functions
		int initiateReward(int pin, int dutyValue);
		
		void TTLpulseWhile();
		
		int startCameraTrigger(int trialVideoFramerate_Hz, int videoRecoding_ms);
		
	};
#endif