#include <Arduino.h>

/*
Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.
*/

/**
* Example using Dynamic Payloads
*
* This is an example of how to use payloads of a varying (dynamic) size.
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Payload
//

char receive_payload[5]; // +1 to allow room for a terminating NULL char
int counter = 0;


void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(115200);

  Serial.println(F("Base Listening for post message"));

  //
  // Setup and configure rf radio
  //

  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  // optionally, increase the delay between retries & # of retries
  // radio.setRetries(5,15);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)


  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);


  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void loop(void)
{
  pong_in();
}

void pong_in(void)
{

  // if there is data ready
  while ( radio.available() )
  {

    radio.read( receive_payload, 4 );

    // Spew it
    Serial.print(counter);
    Serial.print(F(" Got message from postbox..."));
    Serial.println(receive_payload);
    counter ++;

    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( receive_payload, 14 );
    Serial.println(F("Thanks Postbox"));

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}

// vim:cin:ai:sts=2 sw=2 ft=cpp
