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

static Message *create_message ()
{
  Message *mess;
  time_t raw_time;
  struct tm *timeinfo;

  mess = malloc (sizeof (Message));
  if (!mess)
    return NULL;

  time (&raw_time);
  time_info = localtime (&rawtime);

  mess.time = asctime (timeinfo);
  /* Create a random temperature that is at least reasonable for a rooom */
  mess.temp = rand () % 30 + 5;

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


  status = mosquitto_publish (publisher, NULL, topic, strlen (message),
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
