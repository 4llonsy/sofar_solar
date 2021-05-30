# sofar_solar
A project that logs data from sofar solar on-grid inverter and sends the data to the blynk server using RS485 modbus interface.

## Hardware requirements:
- On-Grid Solar Powerplant with Sofar inverter (I am using Sofar 3300TL-G3 inverter)
- NodeMCU (or any other ESP8266 based microcontroller)
- Max485 module
- Twisted pair cable
- Power supply
  
## Software requirements:
- Arduino IDE configured to program ESP8266 based microcontrollers
- Stable Wifi network
- Inverter datasheet for registry related details ([Here's my inverter's registry details for reference](https://github.com/4llonsy/sofar_solar/blob/main/Registry_Details.pdf))
### NOTE: The registry information varies slightly for every sofar inverter. So one needs to either get the exact datasheet of the inverter from the manufacturer or just guess what information each registry holds using the above pdf as reference.
### Libraries required:
- [ModbusMaster](https://github.com/4-20ma/ModbusMaster) for communicating with Modbus slaves over RS232/485 (via RTU protocol).
- [Blynk](https://github.com/blynkkk/blynk-library) for communicating with the Blynk server.
