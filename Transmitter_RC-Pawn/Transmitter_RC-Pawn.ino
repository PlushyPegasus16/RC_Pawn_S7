#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN pins 

const byte address[6] = "00001"; // byte array:pipeline address, to communicate. 

void setup() {
  radio.begin();
  radio.openWritingPipe(address); //setting address to write data through
  radio.setPALevel(RF24_PA_MIN); //power amplifier level, how far the modules are away use capacitors for stable voltage. 
  radio.stopListening(); //defining the module as the transmitter. 
}

void loop() {
  const char text[] = "Hello World"; //test code for action 
  radio.write(&text, sizeof(text));
  delay(1000);
}
