#ifndef ROSDUE_H
#define ROSDUE_H

#include "Arduino.h"

#define BAUD_RATE         115200
#define NUM_PUBLISHERS    100
#define NUM_SUBSCRIBERS   100
#define INPUT_BUFFER_SIZE 100

#define DELIMITER ';'

const String MSG_NEW_SUB = "new_sub";
const String MSG_ACK = "ACK";

// Serial
static String serialInBuffer [INPUT_BUFFER_SIZE];
static int serialBufferIndex = 0;
static String serialInString;

// Publisher: Represents a publisher that will be used to publish on a topic
class Publisher {
public:
    String topic;
    Publisher(String t);
};

// Subscriber: Represents a subscriber that will be used to subscribe to a topic
class Subscriber {
public:
    String topic;
    void (*callback) (float f);
    Subscriber(String t, void (*callback_func)(float f));
};

// ROSdue: Represents a connection to a computer running ROS over serial
class ROSdue {
private:

    // Array of current publishers
    Publisher* publishers [NUM_PUBLISHERS];
    int publishers_index = 0;

    // Array of subscribers that have yet to be ACK'd
    Subscriber* new_subscribers [NUM_SUBSCRIBERS];
    int new_subscribers_index = 0;

    // Array of active subscribers
    Subscriber* subscribers [NUM_SUBSCRIBERS];
    int subscribers_index = 0;

    // Parses the input from serial
    void _parse_input(String s);

public:
    ROSdue();

    // Call in the Arduino sketch setup function
    void setup();

    // Called once every loop, does serial send and receive stuff
    void spinOnce();

    void addPublisher(Publisher& p);
    void publish(Publisher& p, String msg);

    void addSubscriber(Subscriber& s);

};

#endif
