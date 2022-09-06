# PsiControl_AdjustablePowersupply

Code for an arduino (Mega) board which is capable of controlling a Ts_AIO-5 card to let it function as a precise USB-C controlled (usb-c of the arduino) power supply. 
The board functions by writing the correct values to the correct registers, based on what you wanna do.

The main functionallities are the connection setup, measuring current and voltage, connecting channels to ground/the bus and putting a specified voltage on the bus.

You can find the GUI in the repository https://github.com/WoutDeleu/PsiControl_GUIAdjustableVoltageSource. It is based on the serial communication between the Arduino and the PC. The Arduino uses to cmdMessenger library to simplify the serial communication.

To run and execute the program, connect the correct pins from the arduino to the correct pins on the AIO-board. Load the correct code on the arduino. After completing the upload, the GUI can be started and used to control the Arduino, using the build in serial communication.
