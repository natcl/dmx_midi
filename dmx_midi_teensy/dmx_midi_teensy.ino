#include <DmxSimple.h>
#include "Timer.h"

Timer t;
char status_led = 13;

void setup() {
  pinMode(status_led, OUTPUT);
  digitalWrite(status_led, HIGH);
  DmxSimple.usePin(3);
  Serial.begin(115200);
}

void loop() {
  
  t.update();  
  
  if (usbMIDI.read())
  {
    // Blink LED when messages are received
    t.pulse(status_led, 100, HIGH);
    
    // If the message is sysex and starts with 127
    if (usbMIDI.getType() == 7 && usbMIDI.getSysExArray()[1] == 127)
    {
      char channel_a = usbMIDI.getSysExArray()[4];
      char channel_b = usbMIDI.getSysExArray()[5];
      int channel = seven_to_fourteen(channel_a, channel_b);
      
      char value_a = usbMIDI.getSysExArray()[2];
      char value_b = usbMIDI.getSysExArray()[3];
      char value = seven_to_fourteen(value_a, value_b);
      
      DmxSimple.write(channel, value);
      
      Serial.print("Channel: ");
      Serial.println(channel, DEC);
      
      Serial.print("Value: ");
      Serial.println(value, DEC); 
    }
  }
}

// Converts two 7-bit bytes into a 14-bit int
int seven_to_fourteen(char byte_a, char byte_b) {
  return byte_a & 127 | byte_b << 7;
}

