#include "rosdue.h"

////////////
// ROSdue //
////////////

ROSdue::ROSdue(){};

void ROSdue::setup(){
    Serial.begin(BAUD_RATE);
}

void ROSdue::addPublisher(Publisher& p) {
    publishers[publishers_index]= &p;
    publishers_index++;
}

void ROSdue::publish(Publisher& p, String data) {
    String msg = p.topic + DELIMITER + data;
    Serial.println(msg + DELIMITER + msg.length());
}

void ROSdue::addSubscriber(Subscriber& s) {
    new_subscribers[new_subscribers_index] = &s;
    new_subscribers_index++;
}

void ROSdue::spinOnce() {

    if (serialBufferIndex > 0) {

        // Process the buffer
        for (int i=0; i<serialBufferIndex; i++) {
            _parse_input(serialInBuffer[i]);
        }

        // Buffer has been processed, clear it
        serialBufferIndex = 0;
    }


    // Check to see if we need to advertise new subs
    if(new_subscribers_index > 0) {
        for(int i=0; i<new_subscribers_index; i++){
            Subscriber s = *new_subscribers[i];
            String msg = MSG_NEW_SUB + DELIMITER + s.topic;
            Serial.println(msg + DELIMITER + msg.length());
        }
    }

}

// Parses the input from serial
void ROSdue::_parse_input(String s) {

    // Get topic and data
    String topic;
    String data;
    for(int i=0; i<s.length(); i++){
        if(s[i] == DELIMITER) {
            topic = s.substring(0,i);
            data = s.substring(i+1);
        }
    }

    // subscriber ACK msg? Find the sub and activate it.
    if(topic == MSG_ACK){
        for(int i=0; i<new_subscribers_index; i++) {
            Subscriber s = *new_subscribers[i];
            if (s.topic == data) {

                // Add sub to active
                subscribers[subscribers_index] = new_subscribers[i];
                subscribers_index++;

                /////////////////////////////////
                // Remove from new_subscribers //
                /////////////////////////////////

                // Decrement insertion index
                new_subscribers_index--;

                // Move everything after to the left (up to insertion point)
                for(int j=i; j<new_subscribers_index; j++){
                    new_subscribers[j] = new_subscribers[j++];
                }

            }
        }
    }

    // Topic we subscribe to, find the sub
    for(int i=0; i<subscribers_index; i++) {
        Subscriber s = *subscribers[i];
        if (s.topic == topic) {
            s.callback(data.toInt());
        }
    }

}

////////////////
// Publisher //
///////////////

Publisher::Publisher(String t){
    topic = t;
};

/////////////////
// Subscriber //
////////////////

Subscriber::Subscriber(String t, void (*callback_func)(int i)){
    topic = t;
    callback = callback_func;
};

///////////
// Other //
///////////

// ISR that occurs when data comes in over serial rx
void serialEvent(){
    while(Serial.available()){

        char c = (char) Serial.read();

        if (c == '\n') {

            // Copy current input string to the line buffer, clear input string
            serialInBuffer[serialBufferIndex] = "" + serialInString;
            serialInString = "";
            serialBufferIndex++;

            // Overflow Check
            if (serialBufferIndex >= INPUT_BUFFER_SIZE) {
                serialBufferIndex = 0;
            }

        } else {
            serialInString += c;
        }
    }
}
