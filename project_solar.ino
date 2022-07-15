#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#define RTS_PIN D5                  //Pin connected to both DE and RE of the MAX485 module
#define RX_PIN  D6                  //Pin connected to RO
#define TX_PIN  D7                  //Pin connected to DI
ModbusMaster node;                  //Instantiate modbus object
SoftwareSerial ss(RX_PIN, TX_PIN);  //Softwareserial object to be used for communicating with RS485


#define START_ADDRESS 0x00
#define STOP_ADDRESS 0x2E

unsigned short  Inverter_State;
float           PV_Voltage;
float           PV_Current;
float           PV_Power;
float           Active_Power;
float           Reactive_Power;
float           Grid_Frequency;
float           Voltage;
float           Current;
unsigned long   Total_Production;
unsigned long   Total_Time;
float           Today_Production;
unsigned int    Today_Time;
float           CT_Power;


#define BLYNK_PRINT Serial
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define AUTH            "<Blynk auth token>"
#define MAIN_SSID       "<wifi name>"
#define MAIN_PASS       "<wifi pass>"

WidgetTerminal terminal(V0);  //(optional) terminal widget on blynk app is connected to V0 for debugging purposes
WidgetLED led(V13);           //led widget on blynk app is connected to V13
BlynkTimer timer;

void preTransmission(){digitalWrite(RTS_PIN, HIGH);}
void postTransmission(){digitalWrite(RTS_PIN, LOW);}

BLYNK_WRITE(V0){
  terminal.write(param.getBuffer(), param.getLength());
  terminal.flush(); //ensures everything is sent to the terminal.
}

//Modify this block of code according to your inverter registry details.
bool fetch_data(){
  uint8_t result;
  result = node.readHoldingRegisters(START_ADDRESS, STOP_ADDRESS); //0x1B (27) registers are read starting from address 0x0000
  if (result == node.ku8MBSuccess)
  {
    Inverter_State    = (unsigned int)node.getResponseBuffer(0x00);
    PV_Voltage        = (unsigned int)node.getResponseBuffer(0x06)*0.1;
    PV_Current        = (unsigned int)node.getResponseBuffer(0x07)*0.01;
    PV_Power          = (unsigned int)node.getResponseBuffer(0x0A)*0.01;

    Active_Power      = (int)node.getResponseBuffer(0x0C)*0.01;
    Reactive_Power    = (unsigned int)node.getResponseBuffer(0x0D)*0.01;

    Grid_Frequency    = (unsigned int)node.getResponseBuffer(0x0E)*0.01;
    Voltage           = (unsigned int)node.getResponseBuffer(0x0F)*0.1;
    Current           = (unsigned int)node.getResponseBuffer(0x10)*0.01;
    
    Total_Production  = (unsigned int)node.getResponseBuffer(0x15)*65536 + (unsigned int)node.getResponseBuffer(0x16);
    Total_Time        = (unsigned int)node.getResponseBuffer(0x17)*65536 + (unsigned int)node.getResponseBuffer(0x18);
    Today_Production  = (unsigned int)node.getResponseBuffer(0x19)*0.01;
    Today_Time        = (unsigned int)node.getResponseBuffer(0x1A);
    CT_Power          = (int16_t)node.getResponseBuffer(0x29)*0.01;
    return true;
  }
  return false;
}

void blynk_update(){
  terminal.println("Trying to fetch.");
  if(fetch_data()){
//    terminal.println("Data fetched successfully.");
//    terminal.println(PV_Voltage);
//    terminal.println(PV_Current);
//    terminal.println(PV_Power);
//    terminal.println(Active_Power);
//    terminal.println(Reactive_Power);
//    terminal.println(Grid_Frequency);
//    terminal.println(Voltage);
//    terminal.println(Current);
//    terminal.println(Total_Production);
//    terminal.println(Total_Time);
//    terminal.println(Today_Production);
//    terminal.println(Today_Time);
    switch(Inverter_State){
      case 0:
        terminal.println("Waiting");
        Blynk.setProperty(V13,"color","#ED9D00"); //Yellow
        break;
      case 1:
        terminal.println("Checking");
        Blynk.setProperty(V13,"color","#ED9D00"); //Yellow
        break;
      case 2:
        terminal.println("Normal");
        Blynk.setProperty(V13,"color","#23C48E"); //Green
        break;
      case 3:
        terminal.println("Fault");
        Blynk.setProperty(V13,"color","#D3435C"); //Red
        Blynk.notify("Fault");
        break;
      case 4:
        terminal.println("Permanent");
        Blynk.setProperty(V13,"color","#D3435C"); //Red
        Blynk.notify("Permanent Fault");
        break;
    }
    terminal.println("Success!");
    Blynk.virtualWrite(V1, Voltage);
    Blynk.virtualWrite(V2, Current);
    Blynk.virtualWrite(V3, Active_Power);
    Blynk.virtualWrite(V4, Reactive_Power);
    Blynk.virtualWrite(V5, PV_Voltage);
    Blynk.virtualWrite(V6, PV_Current);
    Blynk.virtualWrite(V7, PV_Power);
    Blynk.virtualWrite(V8, Grid_Frequency);
    Blynk.virtualWrite(V9, Today_Production);
    Blynk.virtualWrite(V10, Today_Time);
    Blynk.virtualWrite(V11, Total_Production);
    Blynk.virtualWrite(V12, Total_Time);
    Blynk.virtualWrite(V14, CT_Power);
    led.on();
    terminal.println("\n--debug--");
    for(int i = START_ADDRESS; i <= STOP_ADDRESS; i++){
      terminal.printf("%02X: %i\n", i, (int)node.getResponseBuffer(i));
    }
    terminal.println("----debug----\n");
  }
  else {
    terminal.println("Unable to fetch data.");
    Blynk.virtualWrite(V13, LOW);
  }
  terminal.flush();
}

void setup(){
  pinMode(RTS_PIN, OUTPUT);
  digitalWrite(RTS_PIN, 0);
  
  Serial.begin(115200);
  ss.begin(9600);
  node.begin(0x01, ss);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Blynk.begin(AUTH, MAIN_SSID, MAIN_PASS, "<local blynk server ip>eg. 192.168.2.20", 8080);
  terminal.clear();
  timer.setInterval(1000l, blynk_update); //updates data on blynk every second
}

void loop(){
  Blynk.run();
  timer.run();
}