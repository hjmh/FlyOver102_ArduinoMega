  /*
   *  FlyOver102_ArduinoMega.cpp
   *
   *  Arduino protocol to be combined with FlyOver version 1.02
   *      - FlyOver controls trial time and writes FlyOver & Treadmill data to file
   *      - Arduino receives reduced data stream (at about 360Hz) containing positional information
   *          of fly in the VR as well as event information
   *      - Arduino parses though this input stream, initiates adequate optogenetics stimulus (optLED) and outputs
   *          initiated stimulation events to the second serial connection (-> Data PC, Serial logger))
   *
   *  Created by Hannah Haberkern on 14/12/15, updated 2017.
   *
   */
  
  #ifndef VRD_ArduinoMain_h
  #define VRD_ArduinoMain_h
  #include <SPI.h>// include the SPI library code
  #include <VRD_setup_v0182.h> // include library for setting up the communication etc.
  #include <VRD_control_v0182.h> // include library for control of VR setup
  #include <VRD_Arduino_ExpParams.h> // include parameter declarations and initializations
  
  //Define constants to be used later in the experiment
  #define CALIBRATION_HZ 500.00 //Hz of your frame trigger pulses
  #define CALIBRATION_PULSE_TIME 10000//in msec, 10000 = 10 seconds
  
  #endif
  
  //==================================================================================================
  //Declare and initialize variables
  
  //----------------------------------------------------------------------
  // Initialize the states of the VR...
  //----------------------------------------------------------------------
  int stateScreens = 0;	//screens OFF (1 == ON)
  int stateBasler = 0;	//basler cameras OFF (1 == ON)
  int stateOpt = 0;	//optLED OFF (1 == ON)
  
  int stateCalibration = 0; //performing calibration? Ignore input from VRPC
  int stateFlyOverTrial = 0; //is currently an experiment running?
  
  int newCharRead = 0;
  
  //==================================================================================================
  // Instanciate objects from VRsetup class and VRcontrol class
  
  VRD_setup VRsetup(lowestPin, highestPin, dac2pin);
  VRD_control VRcontrol_DataPC(timeOutInterval_DataPC, dacScreenLED, dacBasler, dacOptLED, CALIBRATION_HZ, CALIBRATION_PULSE_TIME, dac2pin);
  VRD_control VRcontrol_VRPC(timeOutInterval_VRPC, dacScreenLED, dacBasler, dacOptLED, CALIBRATION_HZ, CALIBRATION_PULSE_TIME, dac2pin);
  
  //==================================================================================================
  void setup() {
    /* Set up
       - Arduino pins
       - Serial communication (3 serials):
           --> with the VRPC (FO input --> Serial2)
           --> with DataPC (debugging, Matlab communication during calibration --> Serial)
           --> with DataPC (Serial logger to write Arduino event data to file --> Serial or Serial3 (the latter is not currently implemented))
       - intialize Basler, optogenetics LED, screens
       - set a couple of boolean flags..
    */
      
    //-------------------------------------------------------------
    //Arduino setup................................................
    // set pins used to output mode
    VRsetup.initializePins(LEDpin);
    /*initialize SPI; set dac variables & control register, set all dac outputs to 0 */
    VRsetup.initializeSPI();
    //write something on LCD disply...
    VRsetup.LCDcommunication("FlyOver trial");
  	
    //-------------------------------------------------------------
    //Data PC control..............................................
    // initialize private variables
    VRcontrol_DataPC.initializeDacVariables(dacScreenLED, dacBasler, dacOptLED);
  	
    //Serial port for sending out data to Data PC (to be looged by SerialLogger.exe)
    //  --> this is a more reliable way of logging Arduino data and should be used during trials
    Serial.begin(baudrateArduino); // --> COM10
    Serial.println("FlyOver to Data PC");
  	
    //-------------------------------------------------------------
    //VR PC control................................................
    // initialize private variables
    VRcontrol_VRPC.initializeDacVariables(dacScreenLED, dacBasler, dacOptLED);
    // Serial communication with VR PC to receive FlyOver data via softwareSerial port
    // set the data rate for the SoftwareSerial port
    Serial2.begin(baudrateArduino);
    Serial2.println("FlyOver to VR PC");
    Serial2.println();
  	
    //-------------------------------------------------------------
    //Initialize screens, opt. LED and Basler camera..................
    digitalWrite(LEDpin, HIGH);
    analogWrite(dacOptLED, 0);//Set the optogenetics LED to OFF
    stateScreens = VRcontrol_DataPC.screensOff(); //turn screens OFF
    stateBasler = VRcontrol_DataPC.baslerOff();
    stateOpt = VRcontrol_DataPC.optLEDOff();
  }
  
  //==================================================================================================
  
  void loop() {
      //----------------------------------------------------------------------
      //check if there is input from Matlab on DataPC (Serial1)
      if (Serial.available()){//input from Matlab/Serial monitor
          in_DataPC = Serial.read();
          Serial.println(in_DataPC);
          newCharRead = 1;
      }

      if(newCharRead > 0){
          String readTestVal; //
          int reinforcementTestVal; //value to be passed on to red LED during light intensity testing
          
          switch (in_DataPC){
            case 'c': // -->  initiate calibration
                stateCalibration = 1;//performing calibration! Ignore input from VRPC
                
                //...if not already initiated and if nothing else is running (should be in dark with no optogenetic stimulation)
                if(!stateScreens && stateOpt==0 && !stateBasler){
                    Serial.print("Calibration Activated");
                    VRcontrol_DataPC.performCalibration();
                    Serial.println("...by Data PC");
                }
                else{
                    Serial.println("Calibration was unsuccesful.");
                    Serial.println("Turn off screens and optLED and don't use Basler cameras.");
                    Serial.print("stateOpt: ");Serial.println(stateOpt);
                    Serial.print("stateScreens: ");Serial.println(stateScreens);
                    Serial.print("stateBasler: ");Serial.println(stateBasler);
                }
                stateCalibration = 0;
                break;
                
            case 's': // --> turn on sugar cubes (projector lamps)
                if(!stateScreens){ stateScreens = VRcontrol_DataPC.screensOn();
                }else{ stateScreens = VRcontrol_DataPC.screensOff();}
                break;
                
            case 'b': // --> turn on Basler camera
                if(!stateBasler){ stateBasler = VRcontrol_DataPC.baslerOn();
                }else{stateBasler = VRcontrol_DataPC.baslerOff();}
                break;
                
            case 'i': // --> turn the optogenetics LED on (to test duty cycle)
                if(stateOpt == 0){
                    Serial.println("Optogenetics LED On");
                    stateOpt = VRcontrol_DataPC.optLEDOn(optLEDTest_dutyCycle);
                }else{ stateOpt = VRcontrol_DataPC.optLEDOff();}
                break;
                
            case 'k':    //breakout based on DataPC input
                stateFlyOverTrial = 0;
                stateScreens = VRcontrol_DataPC.screensOff();
                stateOpt = VRcontrol_DataPC.optLEDOff();
                Serial.println("trial terminated by DataPC input");
                Serial.println("XXX");
                break;
            
            case 'r':    //set red LED output to a certain test value
                // read in reinforcementTestVal
                in_DataPC = Serial.read();
                readTestVal += in_DataPC; //makes the string currLine
                while (in_DataPC != '\n') {
                    in_DataPC = Serial.read();  //gets one byte from serial buffer
                    readTestVal += in_DataPC; //makes the string currLine
                }//endwhile

                Serial.println(readTestVal);
                char charBuf[100];
                readTestVal.toCharArray(charBuf, 100); 
                reinforcementTestVal = atoi(charBuf);
                charBuf[0] = '\0';
                
                stateOpt = VRcontrol_DataPC.optLEDOn(reinforcementTestVal);
                break;
            
            default: //invalid input
                if(Serial.available()== 0){
                    Serial.print("Invalid input from Data PC: ");
                    Serial.println(in_DataPC);
                }
                break;
          }
          newCharRead = 0;
        }//new char read
        
      
      //----------------------------------------------------------------------
      //check if there is input from FlyOver on VRPC (Serial2)
      if (Serial2.available()){//input from FlyOver
          stateFlyOverTrial = 1;//trial initiated, now ignore further input from DataPC via Matlab (Serial)
          if(!stateScreens){
              stateScreens = VRcontrol_DataPC.screensOn();
              //VRcontrol_DataPC.beginRecordingTrialVideo(20, 5000);
              //initiate video recording with basler camera
          }
      }
      
      while (stateFlyOverTrial) {
        //now enter keep reading data from Serial2, exit when termination character is read
          if (Serial2.available()){//read one line from FlyOver input
  	      String currLine; int posInLine = 0; int currComma = 0;
              char in_VRPC;
              in_VRPC = Serial2.read();
              currLine += in_VRPC; //makes the string currLine
              while (in_VRPC != '\n') {
                  in_VRPC = Serial2.read();  //gets one byte from serial buffer
                  currLine += in_VRPC; //makes the string currLine
              }//endwhile
              //finished reading a line, now start parsing through and processing it
              
              //teminate session when termination sequence is read ("Trial_Ended")
              if (currLine.indexOf(FO_terminationString)!=-1) {
                  //FlyOver trial is over
                  stateFlyOverTrial = 0;
                  stateScreens = VRcontrol_DataPC.screensOff();
                  stateOpt = VRcontrol_DataPC.optLEDOn(0);
                  Serial.println("Trial terminated by FlyOver");
                  Serial.println("XXX");//termination sequence for SerialLogger.exe
                  VRcontrol_DataPC.dac2Write(2, 0);// set the DAC output to low
                  
                  break;
              }//endif (termination of trial)
              
              
              //expected format of data from FlyOver:
              // FO_time, reinforcementVal
              String readString;
              char charBuf[100]; //to read in FO data
              
              currComma = currLine.indexOf(',', 0);
              String FO_time = currLine.substring(0, currComma-1);
              posInLine = currComma+1;
              
              //now read events:
              currComma = currLine.indexOf(',', posInLine);
              readString = currLine.substring(posInLine, currComma-1);
              readString.toCharArray(charBuf, 100); 
              int reinforcementVal = atoi (charBuf);
              posInLine = currComma+1;
              charBuf[0] = '\0';
              
              //switch red light on:
              stateOpt = VRcontrol_DataPC.optLEDOn(reinforcementVal);
              
              //send F0_time, FO_xPos, FO_yPos, Arduino_time, PWM_val
              Serial.print(millis());Serial.print(',');
              Serial.print(FO_time);Serial.print(',');
              Serial.print(reinforcementVal);Serial.print(',');
              Serial.print('\n');
              
          }//endif (data stream from FlyOver)
      } //endwhile (FlyOver trial loop)
  }//void loop()
