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
      int channel_a = usbMIDI.getSysExArray()[3];
      int channel_b = usbMIDI.getSysExArray()[4];
      int channel = channel_a & 127 | channel_b << 7;
      
      char value_a = usbMIDI.getSysExArray()[1];
      char value_b = usbMIDI.getSysExArray()[2];
      char value = value_a & 127 | value_b << 7;
      
      DmxSimple.write(channel, value);
      
      Serial.print("Channel: ");
      Serial.println(channel, DEC);
      
      Serial.print("Value: ");
      Serial.println(value, DEC);
    }
  }
}





