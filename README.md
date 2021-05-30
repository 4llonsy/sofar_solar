# sofar_solar
A project that logs data from sofar solar inverter and sends the data to thingspeak or blynk server using RS485 modbus interface.

## Hardware requirements:
- On-Grid Solar Powerplant with Sofar inverter (I am using Sofar 3300TL-G3 inverter)
- NodeMCU (or any other ESP8266 based microcontroller)
- Max485 module
- Twisted pair cable
- Power supply
  
## Software requirements:
- Arduino IDE configured to program ESP8266 based microcontrollers
- Stable Wifi network
- Inverter datasheet for registry related details (I have attached my inverter's datasheet for reference)
