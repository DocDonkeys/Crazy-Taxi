Logitech Steering Wheel SDK for PC
Copyright (C) 2015 Logitech Inc. All Rights Reserved


Introduction
--------------------------------------------------------------------------

This package is aimed at driving games and enables to quickly and
easily make a complete implementation for reading input data, doing
force feedback, and getting/setting wheel properties.


Contents of the package
--------------------------------------------------------------------------

- Logitech Steering Wheel SDK header, libs and dlls, 32 and 64 bit
- Documentation
- Demo program
- Sample in-game implementation program file


The environment for use of the package
--------------------------------------------------------------------------

1. Microsoft DirectX 9.0c end-user runtime or newer
(http://msdn.microsoft.com/downloads)

2. Visual Studio 2010 to build and run the demo program

3. Drivers installed for particular wheels/joysticks/rumble pads. For
example if using Logitech wheels/joysticks/rumble pads make sure the
latest Gaming Software is installed


Testing setup
------------------------------

Devices used

Logitech
- G29
- G920
- Driving Force GT
- G27
- G25
- Driving Force Pro
- MOMO Force
- MOMO Racing
- Formula Force GP
- Driving Force
- Formula Force
- Force 3D
- Strike Force 3D
- Freedom 2.4 Cordless Joystick
- Cordless Rumblepad
- Cordless Rumblepad 2
- Rumblepad

Microsoft
- Sidewinder Force Feedback 2 (Stick)
- Sidewinder Force (Wheel)

Other (Immersion drivers)
- Saitek Cyborg 3D Force
- Act-Labs Force RS Wheel

Operating Systems
- Windows XP, Vista, 7, 8

DirectX versions
- 9.0c

Driver versions
- Logitech Gaming Software 5.04 or later


Disclaimer
--------------------------------------------------------------------------
This is work in progress. If you find anything wrong with either
documentation or code, please let us know so we can improve on it.


Where to start
--------------------------------------------------------------------------

For a demo program to see what some forces do:

Launch SteeringWheelSDKDemo.exe.

Or:

1. Open up project in Samples/SteeringWheelSDKDemo.

2. Compile and run

3. Plug in one or multiple PC wheel, joystick, gamepad or other game
   controllers at any time.

4. Read instructions at top of program's window


To implement game controller support in your game:

1. Include the following header file in your game:

- Include/LogitechSteeringWheelLib.h

2. Include the following library and dll in your game:

- Lib\x86\LogitechSteeringWheelLib.lib and Lib\x86\LogitechSteeringWheelLib.dll or
- Lib\x64\LogitechSteeringWheelLib.lib and Lib\x64\LogitechSteeringWheelLib.dll

3. Alternatively you may only add the LogitechSteeringWheel.dll (shipped with LGS) to your game and use
   GetProcAddress to access its functions.

4. Read and follow instructions from Doc/LogitechGamingSteeringWheelSDK.pdf

For questions/problems/suggestions send email to:
devtechsupport@logitech.com