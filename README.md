# Raspberry Pi 2 Gas Sensor

A Raspberry PI project to detect gas concentration status.

How it works:
=============
1. System will contantly monitor gas PPM concentration and store data in database(will delete all data every 24 hours to save space).
2. If Gas leaks is detected Piezzo Buzzer(Alarm) will alert user by producing high pitched sound and will send SMS to registered phone    	 number and notify about the gas leaks.
3. System will check for gas leaks every 1 second, a slight delay in detection might occur while the alarm is ringing as the program is 		 running on a single thread.

OS used
=======
1. Raspbian 8.0 Jessie

Language Used
=============
1. C Programming Language

Required Tools/Application
==========================

|No.|Tool|
|-|-------------------------|
|1| Nginx web server |
|2| PHP 5.0 or 7.0 fpm |
|3| Mysql DB |
|4| PHPMyAdmin |

Required Dependencies/Libraries
===============================

|No.|Dependency|
|-|-------------------------|
|1| Wiring Pi |
|2| Wiring Pi Dev |
|3| Mysql.c |

Database Structure
==================
1. Database Name : alarmsystem
2. Table Name : status
3. Table Structure : 

|         ID          | PPM  | Date/Time | Status |
|---------------------|------|-----------|--------|
| INT, auto-increment | text |   text    |  text  |

Hardware interfaces used:
=========================
|No.|Interface|Usage|
|-|-------------------------|--|
|1| UART | Serial AT-Command communication between Raspberry Pi <-> Sim900A GSM Module |
|2| SPI | Grab analog data received from MCP3008 |
|3| I2C | Enable communication between Raspberry Pi <-> 16x2 LCD |
|4| ttyAMA0 | UART interface |

Hardware and sensors used:
==========================

|No.|List|
|-|-------------------------|
|1| Raspberry Pi 2 Model B. |
|2| MQ-2 Gas Sensor. |
|3| Sim900A GSM Module. |
|4| MCP3008 Analog to Digital Converter. |
|5| 16x2 LCD with I2C Backpack module. |
|6| Piezzo Buzzer. |



