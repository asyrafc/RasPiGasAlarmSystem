# R-Pi-Gas-Sensor

A Raspberry PI project to detect gas concentration status.

Hardware and sensors used:
==========================

1. Raspberry Pi 2 Model B.
2. MQ-2 Gas Sensor.
3. Sim900A GSM Module.
4. MCP3008 Analog to Digital Converter.
5. 16x2 LED with I2C Backpack module.
6. Piezzo Buzzer.

How it works:
=============

1. System will contantly monitor gas PPM concentration and store data in database(will delete all data every 24 hours to save space).
2. If Gas leaks is detected Piezzo Buzzer(Alarm) will alert user by producing high pitched sound and will send SMS to registered phone    	 number and notify about the gas leaks.
3. System will check for gas leaks every 1 second, a slight delay in detection might occur while the alarm is ringing as the program is 		 running on a single thread.
