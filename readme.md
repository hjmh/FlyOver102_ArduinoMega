## Libraries
Note that the library folders
- VRD_Arduino_ExpParams (Collection of parameters used in my experiments)
- VRD_control_v0182 (Functions used during a FlyOver trial or for testing and calibrating the rig)
- VRD_setup_v0182 (Functions used for initalising the hardware during the setup routine)

need to be placed in Libraries/Documents/Arduino/libraries. The exact path depends on the platform, check https://www.arduino.cc/en/hacking/libraries for details.

## Serial communication with FlyOver
The Arduino Mega has 3 serial ports: Serial1 on pins 19 (RX) and 18 (TX), Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).

- Serial is used to send data from the Arduino to a "Data PC". This port can be used to log data during an experiment. This serial connection can also be used to testing and debugging of the hardware and for calibration. For example by sending commands to switch on the optogenetic stimulation LED to a certain intensity or generate a TTL trigger for the calibration camera.
- Serial 2 ised used to send data from FlyOver (running on the "VR PC") to the Arduino during a VR trial.

