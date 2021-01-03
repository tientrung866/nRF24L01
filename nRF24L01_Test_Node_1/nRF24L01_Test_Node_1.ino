#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

// This is just the way the RF24 library works:
// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8
RF24 radio(9, 10);
int count = 0; //TRANSMITTER counting setup
int loss = 0; //LOSS counting setup

byte addresses[][6] = {"1Node", "2Node"};

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("THIS IS THE TRANSMITTER CODE - YOU NEED THE OTHER ARDIUNO TO SEND BACK A RESPONSE");
  //
  

  // Initiate the radio object
  radio.begin();

  // Set the transmit power to lowest available to prevent power supply related issues
  radio.setPALevel(RF24_PA_MIN);

  // Set the speed of the transmission to the quickest available
  radio.setDataRate(RF24_2MBPS);

  // Use a channel unlikely to be used by Wifi, Microwave ovens etc
  radio.setChannel(124);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);

  // Random number seeding (we're going to be sending a single random number)
  randomSeed(analogRead(A0));
}

// -----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -----------------------------------------------------------------------------
void loop() {

  Serial.print(count);
  Serial.print(": ");
  //counting
  count ++;

  // Generate a single random character to transmit
  unsigned char data = random(0, 254);
    
  // Ensure we have stopped listening (even if we're not) or we won't be able to transmit
  radio.stopListening(); 

  // Did we manage to SUCCESSFULLY transmit that (by getting an acknowledgement back from the other Arduino)?
  // Even we didn't we'll continue with the sketch, you never know, the radio fairies may help us
  if (!radio.write( &data, sizeof(unsigned char) )) {
    Serial.println("No acknowledgement of transmission - receiving radio device connected?");    
  }

  // Now listen for a response
  radio.startListening();
  
  // But we won't listen for long, 200 milliseconds is enough
  unsigned long started_waiting_at = millis();

  // Loop here until we get indication that some data is ready for us to read (or we time out)
  while ( ! radio.available() ) {

    // Oh dear, no response received within our timescale
    if (millis() - started_waiting_at > 2 ) {
      Serial.println("No response received - timeout!");
      loss ++; // Data loss
      return;
    }
  }

  // Now read the data that is waiting for us in the nRF24L01's buffer
  unsigned char dataRx;
  radio.read( &dataRx, sizeof(unsigned char) );

  // Show user what we sent and what we got back
  Serial.print("Sent: ");
  Serial.print(data);
  Serial.print(", received: ");
  Serial.println(dataRx);

  // if sent =/= received
//  if (data != dataRx){
//    Serial.print("0\n");
//    loss ++;
//    } else Serial.print("1\n");

  // if count == 10000()
  if (count > 999) {
    Serial.print("\n");
    Serial.println("Data loss: ");
    Serial.println(loss);
    Serial.print("\n");
    
    //count = 0; loss = 0;
    while(1);
    }

  // Try again 1s later
  delay(10);
}
