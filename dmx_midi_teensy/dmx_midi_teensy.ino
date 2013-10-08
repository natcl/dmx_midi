#include <DmxSimple.h>
#include "Timer.h"

Timer t;
char status_led = 5;

void setup() {
  pinMode(status_led, OUTPUT);
  digitalWrite(status_led, HIGH);
  
  DmxSimple.usePin(3);

  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleVelocityChange(OnVelocityChange);
  
  Serial.begin(115200);
}

void loop() {
  
  t.update();  
  
  if (usbMIDI.read())
  {
    // Blink LED when messages are received
    t.pulseImmediate(status_led, 100, LOW);
    
    // If the message is sysex and starts with 127
    if (usbMIDI.getType() == 7 && usbMIDI.getSysExArray()[1] == 127)
    {
      char channel_a = usbMIDI.getSysExArray()[4];
      char channel_b = usbMIDI.getSysExArray()[5];
      int channel = seven_to_fourteen(channel_a, channel_b);
      
      char value_a = usbMIDI.getSysExArray()[2];
      char value_b = usbMIDI.getSysExArray()[3];
      char value = seven_to_fourteen(value_a, value_b);
      
      dmx_write(channel, value);
    }
  }
}

void OnControlChange(byte channel, byte control, byte value) {
  midi_to_dmx(channel, control, value);
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  midi_to_dmx(channel, note, velocity);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  midi_to_dmx(channel, note, 0);
}

void OnVelocityChange(byte channel, byte note, byte velocity) {
  midi_to_dmx(channel, note, velocity);
}

void midi_to_dmx(byte channel, byte control, byte value){
  int dmx_channel;
  
  if (channel >= 1 && channel <= 5) {
    if (channel == 1)
      dmx_channel = control;
    if (channel == 2)
      dmx_channel = control + 127;
    if (channel == 3)
      dmx_channel = control + 254;
    if (channel == 4)
      dmx_channel = control + 381;
    if (channel == 5 && control <= 4)
      dmx_channel = control + 508;
    if (channel == 5 && control > 4) {
      dmx_channel = 512;
      return;
    }

    dmx_write(dmx_channel, byte(value / 127. * 255));
  }
}

void dmx_write(int channel, byte value) {
      DmxSimple.write(channel, value);
      
      Serial.print("Channel: ");
      Serial.println(channel, DEC);
      
      Serial.print("Value: ");
      Serial.println(value, DEC); 
}

// Converts two 7-bit bytes into a 14-bit int
int seven_to_fourteen(char byte_a, char byte_b) {
  return byte_a & 127 | byte_b << 7;
}

