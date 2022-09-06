# PsiControl_AdjustablePowersupply

Code for an arduino (Mega) board which is capable of controlling a Ts_AIO-5 card to let it function as a precise USB-C controlled (usb-c of the arduino) power supply. 
The board functions by writing the correct values to the correct registers, based on what you wanna do.

The main functionallities are the connection setup, measuring current and voltage, connecting channels to ground/the bus and putting a specified voltage on the bus.

You can find the GUI in the repository https://github.com/WoutDeleu/PsiControl_GUIAdjustableVoltageSource, and is based on the serial communication between the Arduino and the PC. 
