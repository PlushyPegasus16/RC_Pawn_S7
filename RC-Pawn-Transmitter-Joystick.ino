//Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 9); // CE, CSN
const byte address[6] = "00001"; // byte array:pipeline address, to communicate.

char xyData[32] = ""; //defining xy-data array
String xAxis, yAxis;

void setup() {
  Serial.begin(9600);
  radio.begin(); //start transmitter listening and receiving
  radio.openWritingPipe(address); //setting address to write data through
  radio.setPALevel(RF24_PA_MIN); //power amplifier level, how far the modules are away use capacitors for stable voltage.
  radio.stopListening(); //stop transmitter listen and receiving 
}

void loop() {
  
  xAxis = analogRead(A0); // Read Joysticks X-axis
  yAxis = analogRead(A1); // Read Joysticks Y-axis
  // X value
  xAxis.toCharArray(xyData, 5); // Put the String (X Value) into a character array
  radio.write(&xyData, sizeof(xyData)); // Send the array data (X value) to the other NRF24L01 modile
  // Y value
  yAxis.toCharArray(xyData, 5); //put the String (x Value)into a character array
  radio.write(&xyData, sizeof(xyData));
  delay(20);
}
