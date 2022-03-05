#include <stdio.h>
#include <string.h>
#include "mqtt_broker_interface.h"
#include "mosquitto.h"

#define FDEBUG

#define CHAR_NOT_A_NUMBER(_c) \
  (((_c) != '0') && \
   ((_c) != '1') && \
   ((_c) != '2') && \
   ((_c) != '3') && \
   ((_c) != '4') && \
   ((_c) != '5') && \
   ((_c) != '6') && \
   ((_c) != '7') && \
   ((_c) != '8') && \
   ((_c) != '9') && \
   ((_c) != ':'))



/************************************************************************/

void fn_read_message (int length, char *message)
{
  /* JSON format */
  int i;

  /* New line to make it easier to read. */
  printf ("|          |          |\n");
  /* Get to the start of the information */
  i = 0;
  while (CHAR_NOT_A_NUMBER (message[i]))
    i++;


  /* Read out the time */
  printf ("| ");
  while (!CHAR_NOT_A_NUMBER (message[i]))
  {
    printf ("%c", message[i]);
    i++;
  }

  /* Get to the temperature */
  while (CHAR_NOT_A_NUMBER (message[i]) || (message [i] == ':'))
    i++;

  /* Read out the temp */
  printf (" | ");
  while (!CHAR_NOT_A_NUMBER (message[i]))
  {
    printf ("%c", message[i]);
    i++;
  }

  printf ("       |\n");

}

/************************************************************************/

int main (int argc, char *argv[])
{
  enum STATUS status;
  char *host = "127.0.0.1";
  int port = 1883;
  int qos = 1;
  const char *topic = "spaceship/polarity_inverter"; /* For example */

  status = initialise ();
  CHECK_STATUS (status);


  printf ("|___TIME___|__TEMP/K__|\n");
  status = subscribe_and_read (host, port, topic, qos, fn_read_message);
  CHECK_STATUS (status);

  clean_up ();

  return 0;
}
