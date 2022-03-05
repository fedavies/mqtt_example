#! /bin/sh

sudo pacman -S mosquitto
mosquitto -d
gcc -Wall -g mqtt_publisher.c mqtt_broker_interface.c -lmosquitto --output mqtt_publisher
gcc -Wall -g mqtt_subscriber.c mqtt_broker_interface.c -lmosquitto --output mqtt_subscriber

./mqtt_publisher &
./mqtt_subscriber
