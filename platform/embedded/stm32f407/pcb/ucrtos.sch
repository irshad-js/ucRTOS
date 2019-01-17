EESchema Schematic File Version 4
LIBS:ucrtos-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ucrtos-rescue:nes_gamepad_02x03 J1
U 1 1 5A4D2E55
P 1400 1250
F 0 "J1" H 1450 1450 50  0000 C CNN
F 1 "nes_gamepad_02x03" H 1450 1050 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x03_P2.54mm_Vertical" H 1400 1250 50  0001 C CNN
F 3 "" H 1400 1250 50  0001 C CNN
	1    1400 1250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5A4D2EC1
P 1100 1650
F 0 "#PWR0101" H 1100 1400 50  0001 C CNN
F 1 "GND" H 1100 1500 50  0000 C CNN
F 2 "" H 1100 1650 50  0001 C CNN
F 3 "" H 1100 1650 50  0001 C CNN
	1    1100 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 1050 1100 1150
Wire Wire Line
	1100 1150 1200 1150
Wire Wire Line
	1200 1350 1100 1350
Wire Wire Line
	1100 1350 1100 1650
$Comp
L power:VCC #PWR0102
U 1 1 5A4D2EAB
P 1100 1050
F 0 "#PWR0102" H 1100 900 50  0001 C CNN
F 1 "VCC" H 1100 1200 50  0000 C CNN
F 2 "" H 1100 1050 50  0001 C CNN
F 3 "" H 1100 1050 50  0001 C CNN
	1    1100 1050
	1    0    0    -1  
$EndComp
$Comp
L w_connectors:STM32F4_Discovery_Header J2
U 1 1 5C40F60D
P 4350 2600
F 0 "J2" H 4350 4193 60  0000 C CNN
F 1 "STM32F4_Discovery_Header" H 4350 4087 60  0000 C CNN
F 2 "w_conn_misc:stm32f4_discovery_header" H 4350 3981 60  0000 C CNN
F 3 "" H 4350 2950 60  0000 C CNN
	1    4350 2600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
