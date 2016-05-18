/*
 * Test for setting up some rosdue publishers and subscribers
 */

#include <rosdue.h>

void callback1(float f) {
    // Do something
}

static ROSdue ros;
static Publisher pub1("topic1");
static Subscriber sub1("topic1", &callback1);

void setup() {
  ros.setup();
  ros.addPublisher(pub1);
  ros.addSubscriber(sub1);
}

void loop() {
  ros.publish(pub1, "1.01");
  ros.spinOnce();
  delay(50);
}
