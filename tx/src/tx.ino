#include <Arduino.h>

/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */



#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"


RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Payload
//

char receive_payload[15]; // +1 to allow room for a terminating NULL char

void setup(void)
{

  //
  // Print preamble
  //

  Serial.begin(115200);

  Serial.println(F("RF24 Postbox Transmitter"));


  //
  // Setup and configure rf radio
  //

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);

  // optionally, increase the delay between retries & # of retries
  // radio.setRetries(5,15);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);

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
  pingout();
}

void pingout(void)
{
  // The payload will always be the same, what will change is how much of it we send.
  static char send_payload[] = "POST";

  // First, stop listening so we can talk.
  radio.stopListening();

  // Take the time, and send it.  This will block until complete
  Serial.print(F("Now sending post message"));
  radio.write( send_payload, 4);

  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 500 )
      timeout = true;

  // Describe the results
  if ( timeout )
  {
    Serial.println(F("Failed, response timed out."));
  }
  else
  {

    radio.read( receive_payload, 15 );

    // Spew it
    Serial.print(F(" answer="));
    Serial.println(receive_payload);
  }

  // Try again 1s later
  delay(1000);
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
