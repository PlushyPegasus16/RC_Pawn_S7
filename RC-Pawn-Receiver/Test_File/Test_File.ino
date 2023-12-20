
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define CE_pin  1
#define CSN_pin 2
#define SCK_pin 8
#define MOSI_pin 10
#define MISO_pin 9
#define SIGNAL_TIMEOUT 500  // This is signal timeout in milliseconds. We will reset the data if no signal

const uint64_t pipeIn = 0xF9E8F0F0E1LL;
RF24 radio(CE_pin, CSN_pin); 
unsigned long lastRecvTime = 0;

struct PacketData
{
  byte lxAxisValue;
  byte lyAxisValue;
  byte rxAxisValue;
  byte ryAxisValue;
  byte lPotValue;  
  byte rPotValue;    
  byte switch1Value;
  byte switch2Value;
  byte switch3Value;
  byte switch4Value;  
};
PacketData receiverData;

Servo servo1;     // Pin D2

int led1 = A5;     // LED pin
int led2 = A6;     // LED pin

// Assign default input received values
void setInputDefaultValues()
{
  // The middle position for the joystick. (254/2=127)
  receiverData.lxAxisValue = 127;
  receiverData.lyAxisValue = 127;
  receiverData.rxAxisValue = 127;
  receiverData.ryAxisValue = 127;
  receiverData.lPotValue = 0;  
  receiverData.rPotValue = 0;    
  receiverData.switch1Value = LOW;
  receiverData.switch2Value = LOW;
  receiverData.switch3Value = LOW;
  receiverData.switch4Value = LOW; 
}

void mapAndWriteValues()
{
  servo1.write(map(receiverData.lxAxisValue, 0, 254, 0, 180));
  digitalWrite(led1, receiverData.switch1Value);
  digitalWrite(led2, receiverData.switch2Value);
}

void setup()
{
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening(); // Start the radio receiver 

  servo1.attach(2);  // Attach to pin D2

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  setInputDefaultValues();
  mapAndWriteValues();
}

void loop()
{
  // Check if RF is connected and packet is available 
  if (radio.isChipConnected() && radio.available())
  {
    radio.read(&receiverData, sizeof(PacketData)); 
    lastRecvTime = millis(); 
  }
  else
  {
    // Check for signal loss.
    unsigned long now = millis();
    if (now - lastRecvTime > SIGNAL_TIMEOUT) 
    {
      setInputDefaultValues();
    }
  }
  mapAndWriteValues();         
}
