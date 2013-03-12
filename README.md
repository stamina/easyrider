EasyRider
=========

EasyRider is a motorcycle telemetrics project, developed specifically for my Honda C90 classic bike.

The software is written in C for the Atmel AVR ATmega1284p main microcontroller.

The PCB was created with CadSoft Eagle 6.x, you'll find the schematic and board layout (including all components) in the /eagle directory.
The file /doc/info.txt also contains the [BOM] and some project notes. (Partly in Dutch)

![Board and Bike](https://github.com/stamina/easyrider/raw/master/doc/easyrider.jpg "Board and Bike")

Functionality
-------------

EasyRider basically does the following things:

- Controls the lighting system: indicators, rear light, brake light, etc.
- Controls the claxon
- Contains a simple alarm system, based on an accelerometer
- Reads the motorcycle battery and gives a visual indication of the current voltage
- Contains a little buzzer for some funny startup sounds
- Contains a custom serial UART shell for communication with a terminal program or another microcontroller

Software
--------

The sofware is built around a simple *State Machine* pattern: when the driver is braking or toggling a switch, the software
is sending an event, which is handled by the correct event handler based on the current state (a bitmask of multiple substates).

The buzzer music was created with the **MIDICSV** MIDI to CSV conversion tool. The midi2byte.rb Ruby script converts the
plaintext CSV to a C header file containing an array of music notes.

The custom UART shell code is capable of registering commands and command handlers. E.g. typing "help" or "show state" in TeraTerm, Minicom, Putty
or any other serial terminal program. (configuration: 19200 baud/8N1)

At the moment the shell is communicating with a secondary telemetrics board, called BlackAdder (ARM Cortex-M4 based), which polls for state information. BlackAdder
also contains a GPS and WiFi module for easy tracking and communicating with a mobile phone app (A BlackAdder github repo will be available soon).


*[BOM]: Bill Of Materials
