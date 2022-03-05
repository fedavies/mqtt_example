#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "mosquitto.h"
#define TRUE 1
#define FALSE 0

static enum STATUS {SUCCESS = 0, FAILURE = 1};

typedef struct {
  char time[5];
  int temp;
} Message;

typedef char * JSON;

/**************************************************************************/

#define CHECK_STATUS(_status) \
  if ((_status) != SUCCESS) \
  { \
    printf ("Status error.\n"); \
    return 1; \
  }

/**************************************************************************/

static void fn_on_message (struct mosquitto *mosq,
                           void *obj,
                           const struct mosquitto_message *message)
{

  printf ("Recieved message '%s' on topic '%s'.\n",
          (char *) message -> payload,
          message -> topic);
}

/**************************************************************************/

static void fn_on_connect (struct mosquitto *mosq, void *obj, int result)
{
  printf ("Connected to MQTT broker");
}

/**************************************************************************/

static enum STATUS initialise ()
{
  int status;

  status = mosquitto_lib_init ();
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;

  return SUCCESS;
}

/**************************************************************************/

static char numbers[100] =
  { 00, 01, 02, 03, 04, 05, 06, 07, 08, 09,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
  }

/**************************************************************************/

static char *itoa_2 (int a)
{
#ifdef ASSERT
  ASSERT (a >= 0);
  ASSERT (a <= 99);
#endif

  return _numbers[a];
}
/**************************************************************************/

static JSON *convert_message_to_JSON (char *time, int temp)
{
  JSON *json;

  json = malloc (16 * sizeof (char));
  if (!json)
    return NULL;

  json[0] = "{";
  json[1] = "\n";

  json[2] = "\"";
  json[3] = "time[0]";
  json[4] = "time[1]";
  json[5] = ":";
  json[6] = "time[2]";
  json[7] = "time[3]";
  json[8] = "\"";

  json[9] = " ";
  json[10] = ":";
  json[11] = " ";

  memcpy (&json[12], itoa_2 (temp), 2);

  json[14] = "\n";
  json[15] = "}";

  return json;

}

/**************************************************************************/

static JSON *create_message ()
{
  time_t raw_time;
  struct tm *timeinfo;
  char time[4];
  int temp;

  mess = malloc (sizeof (Message));
  if (!mess)
    return NULL;

  time (&raw_time);
  time_info = localtime (&rawtime);

  mess.time = time_info;
  /* Create a random temperature that is at least reasonable for a rooom */
  temp = rand () % 30 + 5;

  memcpy (&time[0], itoa_2 (time_info.tm_hour), 2);
  memcpy (&time[2], itoa_2 (time_info.tm_min), 2);
  convert_message_to_JSON (time, temp);

  return mess;
}

/**************************************************************************/

static void destroy_message (Message *message)
{
  free (message);
}

/**************************************************************************/

static enum STATUS publish_message (const char *host,
                                    const int   port,
                                    const Message *message,
                                    const char *topic,
                                    const int   qos)
{
  struct mosquitto *publisher;
  int status;

  if (!(publisher = mosquitto_new (NULL, TRUE, NULL)))
  {
    fprintf (stderr, "Out of memory!\n");
    return FAILURE;
  }

  mosquitto_connect_callback_set (publisher, fn_on_connect);

  status = mosquitto_connect (publisher, host, port, 60);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;


  status = mosquitto_publish (publisher, NULL, topic,
                              strlen((char *)message),
                              message, qos, true);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;



  mosquitto_destroy (publisher);

  return SUCCESS;
}


/**************************************************************************/

static void clean_up ()
{
  mosquitto_lib_cleanup ();
}

/**************************************************************************/

int main (int argc, char *argv[])
{
  enum STATUS status;
  char *host = "127.0.0.1";
  int port = 1883;
  int qos = 1;
  const char *topic = "house/sensor";
#if 0
  const char *message = "A message from beyond\n";
#else
  Message *message;
#endif



  status = initialise ();
  CHECK_STATUS (status);

  message = create_message ();
  status = publish_message (host, port, message, topic, qos);
  CHECK_STATUS (status);
  destroy_message (message);

  clean_up ();

  return 0;
}
