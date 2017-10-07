# ucRTOS
A real time operating system for microcontroller projects with windows and linux software emulation

The goal of this project is a simple ecosystem for fast and easy development of Microcontroller projects. The platform is based on the FreeRTOS real-time kernel.

The ecosystem consists of:
- Display (320x240 LCD)
- Filesystem (SD-Card)
- Inputdevice (An NES-Gamepad)
- A Real-Time Kernel (FreeRTOS)

Supported embedded devices so far are:
- STM32F4Discovery (STM32F407VG)

Supported simulators so far are:
- Windows with Visual Studio 2015
- Linux with Eclipse (Not working yet, but under development)

There are also simulators for faster development on a PC. The simulators do not have real time behaviour, sind either windows nor linux are real time operating systems, but the deterministic behaviour of the real time sheduling is simulated. At this time only Windows with Visual Studio 2015 is supported but Eclipse support is planned, so all development can be done using Linux.

## Building Instructions

### STM32F4

coming soon...

### Windows Simulator with Visual Studio 2015

The LCD simulation is based on the wxWidgets GUI Library which is included as a sub repository. Before you are able to build the simulator you need to clone and build wxWidgets first:

#### Cloning wxWidgets subrepository

1. run cmd.exe
2. cd <Path to ucRTOS repository>
3. git submodule init
4. git submodule update

This meight take a few minutes to complete.

#### Build wxWidgets

Browse to platform\simulators\lib\wxWidgets\build\msw and open wx_vc15.sln. Select Win32 as target platform and hit Build Solution from Build menu.

#### Build the simulator

Browse to platform\simulators\windows, open ucRTOS.sln. Select Win32 as target platform and hit Build Solution from Build menu.
