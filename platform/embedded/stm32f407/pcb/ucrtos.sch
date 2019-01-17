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
L ucrtos-rescue:nes_gamepad_02x03 J?
U 1 1 5A4D2E55
P 1400 1250
F 0 "J?" H 1450 1450 50  0000 C CNN
F 1 "nes_gamepad_02x03" H 1450 1050 50  0000 C CNN
F 2 "" H 1400 1250 50  0001 C CNN
F 3 "" H 1400 1250 50  0001 C CNN
	1    1400 1250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5A4D2EC1
P 1100 1650
F 0 "#PWR?" H 1100 1400 50  0001 C CNN
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
L power:VCC #PWR?
U 1 1 5A4D2EAB
P 1100 1050
F 0 "#PWR?" H 1100 900 50  0001 C CNN
F 1 "VCC" H 1100 1200 50  0000 C CNN
F 2 "" H 1100 1050 50  0001 C CNN
F 3 "" H 1100 1050 50  0001 C CNN
	1    1100 1050
	1    0    0    -1  
$EndComp
$EndSCHEMATC
