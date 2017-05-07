/*
 *  VRiD_Arduino_ExpParams.h
 *  Includes all parameter declarations and initializations
 *
 *  Created by Hannah Haberkern on 7/24/12.
 *  Copyright 2012 ETH Zürich. All rights reserved.
 *
 */

//======================================================================
//Declare and initialize variables

//----------------------------------------------------------------------
// Serial communication
//----------------------------------------------------------------------
uint8_t in_DataPC, in_VRPC, in_VRPC_new; // Read characters from serial
long baudrateArduino = 115200;
long timeOutInterval_DataPC = 100000;
long timeOutInterval_VRPC = 5000;
long SerialLoggerTimeOut = 0;

//Buffer
char vrbuffer[128];
uint8_t vr_idx = 0;//Set the index to zero;

//----------------------------------------------------------------------
// Pin assignments
//----------------------------------------------------------------------
int lowestPin = 2;
int highestPin = 13;

int LEDpin = 13;// pin 13 on the Arduino board is connected to a red LED
int dac2pin = 48;// Chip select pins used with the SPI interface

//Dac outputs
int dacScreenLED = 0; //dac2Write(0, 32767) is to turn on the LED lights
int dacBasler = 2;    //dac2Write(2, 32767) is for the Basler
int dacIRLaser = 7;   //analogWrite(7, 0); is to turn the IR light on, Port 7 or '1' on the Arduino, PWM