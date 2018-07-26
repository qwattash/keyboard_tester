EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 25 0
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
L SW_Push SW1
U 1 1 5AD632B0
P 4700 2950
F 0 "SW1" H 4750 3050 50  0000 L CNN
F 1 "SW_Push" H 4700 2890 50  0000 C CNN
F 2 "" H 4700 3150 50  0001 C CNN
F 3 "" H 4700 3150 50  0001 C CNN
	1    4700 2950
	1    0    0    -1  
$EndComp
$Comp
L D_ALT D2
U 1 1 5AD6331D
P 4700 3350
F 0 "D2" H 4700 3450 50  0000 C CNN
F 1 "D_ALT" H 4700 3250 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-123" H 4700 3350 50  0001 C CNN
F 3 "" H 4700 3350 50  0001 C CNN
	1    4700 3350
	-1   0    0    1   
$EndComp
$Comp
L LED_ARGB D1
U 1 1 5AD63407
P 4700 2300
F 0 "D1" H 4700 2670 50  0000 C CNN
F 1 "LED_ARGB" H 4700 1950 50  0000 C CNN
F 2 "LEDs:LED_0606" H 4700 2250 50  0001 C CNN
F 3 "" H 4700 2250 50  0001 C CNN
	1    4700 2300
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J4
U 1 1 5AD63476
P 5950 2500
F 0 "J4" H 5950 2600 50  0000 C CNN
F 1 "Conn_01x01" H 5950 2400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x01_Pitch1.00mm" H 5950 2500 50  0001 C CNN
F 3 "" H 5950 2500 50  0001 C CNN
	1    5950 2500
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J5
U 1 1 5AD634E9
P 5950 2850
F 0 "J5" H 5950 2950 50  0000 C CNN
F 1 "Conn_01x01" H 5950 2750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x01_Pitch1.00mm" H 5950 2850 50  0001 C CNN
F 3 "" H 5950 2850 50  0001 C CNN
	1    5950 2850
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J6
U 1 1 5AD63518
P 5950 3150
F 0 "J6" H 5950 3250 50  0000 C CNN
F 1 "Conn_01x01" H 5950 3050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x01_Pitch1.00mm" H 5950 3150 50  0001 C CNN
F 3 "" H 5950 3150 50  0001 C CNN
	1    5950 3150
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x01 J1
U 1 1 5AD63548
P 3700 2550
F 0 "J1" H 3700 2650 50  0000 C CNN
F 1 "Conn_01x01" H 3700 2450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x01_Pitch1.00mm" H 3700 2550 50  0001 C CNN
F 3 "" H 3700 2550 50  0001 C CNN
	1    3700 2550
	-1   0    0    1   
$EndComp
$Comp
L Conn_01x01 J2
U 1 1 5AD635B5
P 3700 2900
F 0 "J2" H 3700 3000 50  0000 C CNN
F 1 "Conn_01x01" H 3700 2800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x01_Pitch1.00mm" H 3700 2900 50  0001 C CNN
F 3 "" H 3700 2900 50  0001 C CNN
	1    3700 2900
	-1   0    0    1   
$EndComp
$Comp
L Conn_01x01 J3
U 1 1 5AD635E9
P 3700 3300
F 0 "J3" H 3700 3400 50  0000 C CNN
F 1 "Conn_01x01" H 3700 3200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x01_Pitch1.00mm" H 3700 3300 50  0001 C CNN
F 3 "" H 3700 3300 50  0001 C CNN
	1    3700 3300
	-1   0    0    1   
$EndComp
Wire Wire Line
	4900 2300 5350 2300
Wire Wire Line
	5350 2300 5350 2500
Wire Wire Line
	5350 2500 5750 2500
Wire Wire Line
	4500 2100 4150 2100
Wire Wire Line
	4150 2100 4150 2550
Wire Wire Line
	4150 2550 3900 2550
Wire Wire Line
	4500 2300 4250 2300
Wire Wire Line
	4250 2300 4250 2900
Wire Wire Line
	4250 2900 3900 2900
Wire Wire Line
	4500 2500 4350 2500
Wire Wire Line
	4350 2500 4350 3300
Wire Wire Line
	4350 3300 3900 3300
Wire Wire Line
	4900 2950 5300 2950
Wire Wire Line
	5300 2950 5300 2850
Wire Wire Line
	5300 2850 5750 2850
Wire Wire Line
	4500 2950 4500 3350
Wire Wire Line
	4500 3350 4550 3350
Wire Wire Line
	4850 3350 4850 3150
Wire Wire Line
	4850 3150 5750 3150
$EndSCHEMATC
