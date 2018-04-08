# ucRTOS
A real time operating system for microcontroller projects with windows and linux software emulation

The goal of this project is a simple ecosystem for fast and easy development of microcontroller projects. The platform is based on the FreeRTOS real-time kernel.

The ecosystem consists of:
* Display (320x240 LCD)
* Filesystem (SD-Card)
* Input device (An NES-Gamepad)
* A real time Kernel (FreeRTOS)
* A simple menu system

There are also simulators for faster development on a PC. The simulators do not have real time behaviour. This is because of either Windows nor Linux are real time operating systems. However, the deterministic task switching behaviour of the real time scheduler is simulated correctly. At this time only Windows with Visual Studio 2017 is supported but support for Eclipse is already under development. No windows will be needed at all then.

Supported embedded devices so far:
* STM32F4Discovery (STM32F407VG)

Supported simulators so far:
* Windows with Visual Studio 2017
* Linux with Eclipse (Not working yet, but under development)

## Building Instructions

### STM32F4

#### Compile the binary
```
make
```

#### Flash the microcontroller

To flash the .bin file you need to install **st-link** first.
After that you can just flash by typing:

```
make flash
```

### Windows Simulator with Visual Studio 2017

The LCD simulation is based on the wxWidgets GUI Library which is included as a sub repository. Before you are able to build the simulator you need to clone and build wxWidgets first:

#### Cloning wxWidgets subrepository
```
1. run cmd.exe
2. cd \<Path to ucRTOS repository\>
3. git submodule init
4. git submodule update
```

This meight take a few minutes to complete.

#### Build wxWidgets

1. Browse to platform\simulators\lib\wxWidgets\build\msw and open wx_vc15.sln. 
2. Migrate the Project for Visual Studio 2017 by selecting **Solution 'wx_vc15'** in Solution explorer and then select **Project > Retarget Solution**, check all vcxproj files, select the latest Windows SDK Version and click **ok**.
3. Select Win32 as target platform and hit Build Solution from Build menu.
4. After the build is finished, the Simulator can be build.

#### Build the simulator

Browse to platform\simulators\windows, open ucRTOS.sln. Select Win32 as target platform and hit Build Solution from Build menu.
