# ROSdue
ROS serial communication interface that works with Arduino, including the Arduino DUE. This was created (very hastily)
after there were found to be issues using ROSserial with the DUE, especially over long periods of time. Very simple, this repo
includes a library for Arduino, and a ROS package to run on the computer.


### Arduino
Move the `rosdue` folder to your Arduino Libraries folder. After this, you'll be able to use publishers and subscribers in your
Arduino code. It's pretty simple, an example sketch might look like this:
```c
#include <rosdue.h>

void callback2(float f) {
  // Do something when you get a message on topic 2
}

static ROSdue ros;
static Publisher pub1("topic1");
static Subscriber sub1("topic2", &callback2);

void setup() {
  ros.setup();
  ros.addPublisher(pub1);
  ros.addSubscriber(sub1);
}

void loop() {
  ros.publish(pub1, "1");
  ros.spinOnce();
  delay(10);
}
```

### Computer
First, make sure that you have ROS setup on the machine, as well as have a workspace you can move the package into, and also
have pyserial installed. After that, you can move the contents of the `rosdue` folder/package to your workspace and build the
package. All it contains is the script that connect to the Arduino over serial and publish/subscribe to the topics on the Arduino.

Once you have the package built and are ready to go, make sure you have `roscore` going, and run the node:
```
$ rosrun rosdue serial_due.py /path/to/arduino/device
```
Where `/path/to/ardiuno/device` is the path to the serial interface for the Arduino. After that, you should see some messages
about creating the publishers and subscribers that you had setup on the Arduino and you should be good to go!

## Contributing

This project was made very quickly to fill a need on a project, so feel free to contribute if you see something that could
be improved or want to add some functionality.
