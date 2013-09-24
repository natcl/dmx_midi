#include <DmxSimple.h>

void setup() {
  DmxSimple.usePin(3);
  Serial.begin(115200);
}

void loop() {
  
  //uint8_t (*sysex)[6];
  if (usbMIDI.read())
  {
    if (usbMIDI.getType() == 7)
    {
      char channel_a = usbMIDI.getSysExArray()[3];
      char channel_b = usbMIDI.getSysExArray()[4];
      int channel = seven_to_fourteen(channel_a, channel_b);
      
      char value_a = usbMIDI.getSysExArray()[1];
      char value_b = usbMIDI.getSysExArray()[2];
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



