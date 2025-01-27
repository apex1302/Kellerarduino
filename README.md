Kellerarduino
=============
Control the humindity of your basement. 
Sensor 1. checks the humidity inside, sensor 2. the humidity on the outside
The absolute humidity is calculated and comapred. If the outside is dryer than the inside a fan gets turned on.

Code Updated for ESP8266 Wemos D1 and X/Twitter removed to functional and democracy preserving reasons.

kellerarduinoMQTT has the same functionalty with MQTT support added.

Example of the MQTT data:

home/sensors/indoor/humidity -> 55.67
home/sensors/indoor/temperature -> 22.34
home/sensors/indoor/absolute_humidity -> 9.45
home/sensors/outdoor/humidity -> 45.12
home/sensors/outdoor/temperature -> 19.67
home/sensors/outdoor/absolute_humidity -> 8.32
home/relay/status -> ON

