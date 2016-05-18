#!/usr/bin/env python

import rospy
from std_msgs.msg import Float32

import argparse
import serial

# Serial connection
serial_connection = serial.Serial()
serial_connection.baudrate = 115200
serial_connection.timeout = 0.5

# publishers[topic] = publisher
publishers = {}

# subscriber[topic] = subscriber
subscribers = {}

# On close, ensures the serial connection gets closed
def on_close():
    print('ROSdue: closing serial connection')
    serial_connection.close()
import atexit
atexit.register(on_close)

# Function to create closure for topic handler
def subscriber_handler(topic):
    def handler(data):
        serial_connection.write(topic + ';' + str(data.data) + '\n')
    return handler

def main():

    # Input argument parse
    parser = argparse.ArgumentParser(description='ROS node for rosdue')
    parser.add_argument('serial_port', help='path to microcontroller serial port')
    args = parser.parse_args()

    # Setup connection
    rospy.loginfo('ROSdue: attempting to connect to serial port: ' + args.serial_port)
    serial_connection.port = args.serial_port;
    serial_connection.open()

    # Start node
    rospy.init_node('ROSdue', anonymous=True)

    while not rospy.is_shutdown():

        input_string = serial_connection.readline().strip()

        try:

            # Test to see if we got only ascii chars (if not throws error)
            input_string.encode('ascii')

            input_vals = input_string.split(';')

            # If we have our 3 values: "topic;data;message_length"
            if len(input_vals) == 3:
                topic = input_vals[0];
                data = input_vals[1];
                msg_length = input_vals[2];

                if(len(topic) + 1 + len(data) == int(msg_length)):

                    # New subscriber
                    if (topic == 'new_sub'):
                        serial_connection.write(b'ACK;' + data + '\n')
                        if(data in subscribers.keys()):
                            pass
                            #rospy.loginfo('ROSdue: wating for DUE to respond to subscriber ACK for:' + data)
                        else:
                            subscribers[data] = rospy.Subscriber(data, Float32, subscriber_handler(data))
                            rospy.loginfo('ROSdue: Created Subscriber: ' + data)

                    # Publisher
                    elif (topic):
                        if topic not in publishers:
                            rospy.loginfo('ROSdue: Creating new publisher: ' + topic)
                            publishers[topic] = rospy.Publisher(topic, Float32, queue_size=10)
                        publishers[topic].publish(float(data))

                else:
                    rospy.loginfo('ROSdue: Invalid message sent over serial (length wrong)')

            else:
                if len(publishers) == 0:
                    pass # We timed out but are not sending anything so whatever
                else:
                    rospy.loginfo('ROSdue: Invalid message sent over serial: ' + str(input_string))

        except UnicodeDecodeError:
            rospy.loginfo('ROSdue: Invalid message sent over serial (not unicode)')

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        on_close()
