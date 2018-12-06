EESchema Schematic File Version 4
LIBS:cherry_proto_circuit-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 5
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
L Switch:SW_Push SW2
U 1 1 5B18E5DE
P 3900 1850
F 0 "SW2" V 3946 1802 50  0000 R CNN
F 1 "SW_Push" V 3855 1802 50  0000 R CNN
F 2 "" H 3900 2050 50  0001 C CNN
F 3 "" H 3900 2050 50  0001 C CNN
	1    3900 1850
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW5
U 1 1 5B18E72C
P 6900 1850
F 0 "SW5" V 6946 1802 50  0000 R CNN
F 1 "SW_Push" V 6855 1802 50  0000 R CNN
F 2 "" H 6900 2050 50  0001 C CNN
F 3 "" H 6900 2050 50  0001 C CNN
	1    6900 1850
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5B18E790
P 3900 3050
F 0 "SW3" V 3946 3002 50  0000 R CNN
F 1 "SW_Push" V 3855 3002 50  0000 R CNN
F 2 "" H 3900 3250 50  0001 C CNN
F 3 "" H 3900 3250 50  0001 C CNN
	1    3900 3050
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 5B18E7DA
P 5400 3050
F 0 "SW4" V 5446 3002 50  0000 R CNN
F 1 "SW_Push" V 5355 3002 50  0000 R CNN
F 2 "" H 5400 3250 50  0001 C CNN
F 3 "" H 5400 3250 50  0001 C CNN
	1    5400 3050
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW6
U 1 1 5B18E836
P 6900 3050
F 0 "SW6" V 6946 3002 50  0000 R CNN
F 1 "SW_Push" V 6855 3002 50  0000 R CNN
F 2 "" H 6900 3250 50  0001 C CNN
F 3 "" H 6900 3250 50  0001 C CNN
	1    6900 3050
	0    -1   -1   0   
$EndComp
$Comp
L device:D D1
U 1 1 5B18EB19
P 3550 2150
F 0 "D1" H 3550 2366 50  0000 C CNN
F 1 "D" H 3550 2275 50  0000 C CNN
F 2 "" H 3550 2150 50  0001 C CNN
F 3 "" H 3550 2150 50  0001 C CNN
	1    3550 2150
	1    0    0    -1  
$EndComp
$Comp
L device:D D4
U 1 1 5B18EB8D
P 6550 2150
F 0 "D4" H 6550 2366 50  0000 C CNN
F 1 "D" H 6550 2275 50  0000 C CNN
F 2 "" H 6550 2150 50  0001 C CNN
F 3 "" H 6550 2150 50  0001 C CNN
	1    6550 2150
	1    0    0    -1  
$EndComp
$Comp
L device:D D5
U 1 1 5B18EC65
P 6550 3350
F 0 "D5" H 6550 3566 50  0000 C CNN
F 1 "D" H 6550 3475 50  0000 C CNN
F 2 "" H 6550 3350 50  0001 C CNN
F 3 "" H 6550 3350 50  0001 C CNN
	1    6550 3350
	1    0    0    -1  
$EndComp
$Comp
L device:D D3
U 1 1 5B18ECBB
P 5050 3350
F 0 "D3" H 5050 3566 50  0000 C CNN
F 1 "D" H 5050 3475 50  0000 C CNN
F 2 "" H 5050 3350 50  0001 C CNN
F 3 "" H 5050 3350 50  0001 C CNN
	1    5050 3350
	1    0    0    -1  
$EndComp
$Comp
L device:D D2
U 1 1 5B18ED1D
P 3550 3350
F 0 "D2" H 3550 3566 50  0000 C CNN
F 1 "D" H 3550 3475 50  0000 C CNN
F 2 "" H 3550 3350 50  0001 C CNN
F 3 "" H 3550 3350 50  0001 C CNN
	1    3550 3350
	1    0    0    -1  
$EndComp
Text HLabel 1900 2300 0    50   Output ~ 0
row_1
Text HLabel 1900 3650 0    50   Output ~ 0
row_2
Text HLabel 4300 950  0    50   Input ~ 0
col_1
Text HLabel 5800 1000 0    50   Input ~ 0
col_2
Text HLabel 7350 1000 0    50   Input ~ 0
col_3
Wire Wire Line
	4300 950  4300 1600
Wire Wire Line
	4300 2850 3900 2850
Wire Wire Line
	3900 1650 3900 1600
Wire Wire Line
	3900 1600 4300 1600
Connection ~ 4300 1600
Wire Wire Line
	4300 1600 4300 2850
Wire Wire Line
	3900 2050 3900 2150
Wire Wire Line
	3900 2150 3700 2150
Wire Wire Line
	3900 3250 3900 3350
Wire Wire Line
	3900 3350 3700 3350
Wire Wire Line
	5400 3250 5400 3350
Wire Wire Line
	5400 3350 5200 3350
Wire Wire Line
	6900 3250 6900 3350
Wire Wire Line
	6900 3350 6700 3350
Wire Wire Line
	5400 2850 5800 2850
Wire Wire Line
	5800 2850 5800 1000
Wire Wire Line
	6900 1650 7350 1650
Wire Wire Line
	7350 1650 7350 1000
Wire Wire Line
	7350 1650 7350 2850
Wire Wire Line
	7350 2850 6900 2850
Connection ~ 7350 1650
Wire Wire Line
	6700 2150 6900 2150
Wire Wire Line
	6900 2150 6900 2050
Wire Wire Line
	1900 2300 2550 2300
Wire Wire Line
	6400 2300 6400 2150
Wire Wire Line
	3400 2150 3400 2300
Connection ~ 3400 2300
Wire Wire Line
	3400 2300 6400 2300
Wire Wire Line
	1900 3650 2550 3650
Wire Wire Line
	6400 3650 6400 3350
Wire Wire Line
	4900 3350 4800 3350
Wire Wire Line
	4800 3350 4800 3650
Connection ~ 4800 3650
Wire Wire Line
	4800 3650 6400 3650
Wire Wire Line
	3400 3350 3350 3350
Wire Wire Line
	3350 3350 3350 3650
Connection ~ 3350 3650
Wire Wire Line
	3350 3650 4800 3650
$Comp
L device:R R12
U 1 1 5BEF4B48
P 2550 2550
F 0 "R12" H 2620 2596 50  0000 L CNN
F 1 "47K" H 2620 2505 50  0000 L CNN
F 2 "" V 2480 2550 50  0001 C CNN
F 3 "" H 2550 2550 50  0001 C CNN
	1    2550 2550
	1    0    0    -1  
$EndComp
$Comp
L device:R R13
U 1 1 5BEF4BB2
P 2550 3900
F 0 "R13" H 2620 3946 50  0000 L CNN
F 1 "47K" H 2620 3855 50  0000 L CNN
F 2 "" V 2480 3900 50  0001 C CNN
F 3 "" H 2550 3900 50  0001 C CNN
	1    2550 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2300 2550 2400
Connection ~ 2550 2300
Wire Wire Line
	2550 2300 3400 2300
Wire Wire Line
	2550 2700 2550 2800
Wire Wire Line
	2550 3650 2550 3750
Connection ~ 2550 3650
Wire Wire Line
	2550 3650 3350 3650
Wire Wire Line
	2550 4050 2550 4150
$Comp
L power:GND #PWR016
U 1 1 5BF4F563
P 2550 4150
F 0 "#PWR016" H 2550 3900 50  0001 C CNN
F 1 "GND" H 2555 3977 50  0000 C CNN
F 2 "" H 2550 4150 50  0001 C CNN
F 3 "" H 2550 4150 50  0001 C CNN
	1    2550 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5BF4F58D
P 2550 2800
F 0 "#PWR015" H 2550 2550 50  0001 C CNN
F 1 "GND" H 2555 2627 50  0000 C CNN
F 2 "" H 2550 2800 50  0001 C CNN
F 3 "" H 2550 2800 50  0001 C CNN
	1    2550 2800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
