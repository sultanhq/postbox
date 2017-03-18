#include <Arduino.h>

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

char receive_payload[5];
int counter = 0;

void setup(void)
{

  Serial.begin(115200);
  Serial.println(F("Base Listening for post message"));

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();

}

void loop(void)
{
  pong_in();
}

void pong_in(void)
{

  while ( radio.available() )
  {

    radio.read( receive_payload, 4 );

    Serial.print(counter);
    Serial.print(F(" Got message from postbox..."));
    Serial.println(receive_payload);
    counter ++;

    radio.stopListening();

    radio.write( receive_payload, 14 );
    Serial.println(F("Thanks Postbox"));

    radio.startListening();
  }
}
