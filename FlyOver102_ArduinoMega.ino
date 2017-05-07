  /*
   *  FlyOver094_Arduino.cpp
   *
   *  Arduino protocol to be combined with FlyOver version from June 2013
   *      - FlyOver controls trial time and writes FlyOver & Treadmill data to file
   *      - Arduino receives reduced data stream (at about 360Hz) containing positional information
   *          of fly in the VR as well as event information
   *      - Arduino parses though this input stream, initiates adequate laser punishment and outputs
   *          initiated punishment events to the second serial connection (-> PC2, Serial logger))
   *
   *  Created by Hannah Haberkern on 14/12/15.
   *
   */
  
  #ifndef VRD_ArduinoMain_h
  #define VRD_ArduinoMain_h
  #include <SPI.h>// include the SPI library code
  #include <VRD_setup_v0182.h> // include library for setting up the communication etc.
  #include <VRD_control_v0182.h> // include library for control of VR setup
  #include <VRD_Arduino_ExpParams.h> // include parameter declarations and initializations

  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
  
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
  int stateLaser = 0;		//laser OFF (1 == ON, 2)
  
  int stateCalibration = 0; //performing calibration? Ignore input from VRPC
  int stateFlyOverTrial = 0; //is currently an experiment running?
  
  String FO_terminationString = "Trial_Ended"; //message printed by FlyOver when trial is terminated at end of trial time
  
  int newCharRead = 0;
  
  int laserTest_dutyCycle = 50; //for laser positioning
  
  //==================================================================================================
  // Instanciate objects from VRsetup class and VRcontrol class and VRiD_LaserGradient class
  
  VRD_setup VRsetup(lowestPin, highestPin, dac2pin);
  VRD_control VRcontrol_DataPC(timeOutInterval_DataPC, dacScreenLED, dacBasler, dacIRLaser, CALIBRATION_HZ, CALIBRATION_PULSE_TIME, dac2pin);
  VRD_control VRcontrol_VRPC(timeOutInterval_VRPC, dacScreenLED, dacBasler, dacIRLaser, CALIBRATION_HZ, CALIBRATION_PULSE_TIME, dac2pin);
  
  //==================================================================================================
  void setup() {
    /* Set up
       - Arduino pins
       - Serial communication (3 serials):
           --> with the VRPC (FO input)
           --> with DataPC (Matlab communication during calibration)
           --> with DataPC (Serial logger to write Arduino event data to file)
       - intialize Basler, IR laser, screens
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
    VRcontrol_DataPC.initializeDacVariables(dacScreenLED, dacBasler, dacIRLaser);
  	
    //Serial port for sending out data to Data PC (to be looged by SerialLogger.exe)
    //  --> this is a more reliable way of logging Arduino data and should be used during trials
    Serial.begin(baudrateArduino); // --> COM10
    Serial.println("FlyOver to Data PC");
  	
    //-------------------------------------------------------------
    //VR PC control................................................
    // initialize private variables
    VRcontrol_VRPC.initializeDacVariables(dacScreenLED, dacBasler, dacIRLaser);
    // Serial communication with VR PC to receive FlyOver data...
    Serial2.begin(baudrateArduino);
    Serial2.println("FlyOver to VR PC");
    Serial2.println();
  	
    //-------------------------------------------------------------
    //Initialize screens, laser and Basler camera..................
    digitalWrite(LEDpin, HIGH);
    analogWrite(dacIRLaser, 0);//Set the IR light to OFF
    stateScreens = VRcontrol_DataPC.screensOff(); //turn screens OFF
    stateBasler = VRcontrol_DataPC.baslerOff();
    stateLaser = VRcontrol_DataPC.laserOff();
  }
  
  //==================================================================================================
  
  void loop() {
      //----------------------------------------------------------------------
      //check if there is input from Matlab on DataPC (Serial3)
      if (Serial.available()){//input from Matlab/Serial monitor
          in_DataPC = Serial.read();
          Serial.println(in_DataPC);
          newCharRead = 1;
      }

      if(newCharRead > 0){
          String readTestVal; //
          int reinforcementTestVal; //value to be passed on to red LED ('laser') during light intensity testing
          
          switch (in_DataPC){
            case 'c': // -->  initiate calibration
                stateCalibration = 1;//performing calibration! Ignore input from VRPC
                
                //...if not already initiated and if nothing else is running (should be in dark with no laser)
                if(!stateScreens && stateLaser==0 && !stateBasler){
                    Serial.print("Calibration Activated");
                    VRcontrol_DataPC.performCalibration();
                    Serial.println("...by Data PC");
                }
                else{
                    Serial.println("Calibration was unsuccesful.");
                    Serial.println("Turn off screens and laser and don't use Basler cameras.");
                    Serial.print("stateLaser: ");Serial.println(stateLaser);
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
                
            case 'i': // --> turn on the IR laser
                if(stateLaser == 0){
                    Serial3.println("Laser On");
                    stateLaser = VRcontrol_DataPC.laserOn(laserTest_dutyCycle);
                }else{ stateLaser = VRcontrol_DataPC.laserOff();}
                break;
                
            case 'k':    //breakout based on DataPC input
                stateFlyOverTrial = 0;
                stateScreens = VRcontrol_DataPC.screensOff();
                stateLaser = VRcontrol_DataPC.laserOff();
                Serial.println("trial terminated by DataPC input");
                Serial.println("XXX");
                break;
            
            case 'r':    //set red LED ('laser') output to a certain test value
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
                
                stateLaser = VRcontrol_DataPC.laserOn(reinforcementTestVal);
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
          stateFlyOverTrial = 1;//trial initiated, now ignore further input from DataPC via Matlab (Serial3)
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
                  stateLaser = VRcontrol_DataPC.laserOn(0); //stateLaser = VRcontrol_DataPC.laserOff();
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
              stateLaser = VRcontrol_DataPC.laserOn(reinforcementVal);
              
              //send F0_time, FO_xPos, FO_yPos, Arduino_time, PWM_val
              Serial.print(millis());Serial.print(',');
              Serial.print(FO_time);Serial.print(',');
              Serial.print(reinforcementVal);Serial.print(',');
              Serial.print('\n');
              
          }//endif (data stream from FlyOver)
      } //endwhile (FlyOver trial loop)
  }//void loop()
