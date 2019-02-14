# ucRTOS
A real time operating system for microcontroller projects with Windows and Linux software emulation.

The goal of this project is a simple ecosystem for fast and easy development of microcontroller projects. The platform is based on the FreeRTOS real-time kernel.

The ecosystem consists of:
* Display (320x240 LCD)
* File system (SD-Card)
* Input device (NES-Gamepad)
* Bus Interface (RS485)
* RF Interface (NRF24)
* Real time kernel (FreeRTOS v9.0.0)
* Simple menu system

For faster development there are simulators which run under Windows and Linux. This allows quick protoyping on e.g. menus or images without the need of flashing the microcontroller after each change. The simulators do not have real time behaviour. This is because of either Windows nor Linux are real time operating systems. However, the deterministic task switching behaviour of the real time scheduler is simulated correctly. Simulators are tested on Windows 7 with Visual Studio 2017 and Arch Linux.

Supported embedded devices so far:
* STM32F4Discovery (STM32F407VG)

Supported simulators so far:
* Windows 7 with Visual Studio 2017
* Linux with GCC

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

Note: The simulator does not work properly on Windows 10. It can be compiled and run but the timings are incorrect. Therefore **it is suggested using Windows 7**.

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
4. After compiling is finished, the simulator can be built.

#### Build the simulator

1. To build the simulator navigate to platform/simulators/linux and execute **make**
2. Run the simulator by executing **platform/simulators/linux/bin/ucrtos.elf**

### Debugging using Eclipse

Create a Debug configuration and select **platform/simulators/linux/bin/ucrtos.elf** als target. Since the purpose of the simulators is debugging, the executable is always built with debug information.



