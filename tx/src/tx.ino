#include <Arduino.h>

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
char receive_payload[15];

void setup(void)
{
  radio_setup();
  serial_setup();
}

void serial_setup(void)
{
  Serial.begin(115200);
  Serial.println(F("RF24 Postbox Transmitter"));
}

void radio_setup(void)
{
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(108);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
}

void loop(void)
{
  ping_out();
}

void ping_out(void)
{
  static char send_payload[] = "POST";

  radio.stopListening();

  Serial.print(F("Now sending post message"));
  radio.write( send_payload, 4);

  radio.startListening();

  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 500 )
      timeout = true;

  if ( timeout )
  {
    Serial.println(F("Failed, response timed out."));
  }
  else
  {

    radio.read( receive_payload, 15 );

    Serial.print(F(" answer="));
    Serial.println(receive_payload);
  }

  delay(1000);
}
