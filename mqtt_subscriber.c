#include <stdio.h>
#include <string.h>
#include "mqtt_broker_interface.h"
#include "mosquitto.h"

#define FDEBUG

/* Include : to make time parsing easier */
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



/*************************************************************************/
/* A callback function called whenever the subscriber recieves a message */
/*************************************************************************/

void fn_read_message (int length, char *message)
{
  /* JSON format */
  int i;
  int num_printed;

  /* New line to make it easier to read. */
  printf ("|          |          |\n");
  /* Get to the start of the information */
  i = 0;
  while (CHAR_NOT_A_NUMBER (message[i]))
    i++;


  /* Read out the time */
  printf ("| ");
  num_printed = 2;
  while (!CHAR_NOT_A_NUMBER (message[i]))
  {
    printf ("%c", message[i]);
    num_printed++;
    i++;
  }

  /* Get to the temperature */
  while (CHAR_NOT_A_NUMBER (message[i]) || (message [i] == ':'))
    i++;

  /* Read out the temp */
  printf (" | ");
  num_printed += 3;
  while (!CHAR_NOT_A_NUMBER (message[i]))
  {
    printf ("%c", message[i]);
    num_printed++;
    i++;
  }

  while (num_printed < 22)
  {
    printf (" ");
    num_printed++;
  }
  printf ("|\n");

}

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

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
