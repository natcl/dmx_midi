// Buffered DMX library with low channel counts for fast refresh rate, or high channel count without tying up the processor for a long period of time.
#include "Timer.h"
Timer t;
char status_led = 13;

byte buffer[512+4]; // Minimum size = number of channels + 1.  Add more channels if the last light flickers.  This seems reliable: size = number of channels + 4.
// Send at at least 24 channels (25 bytes) to keep time between breaks above 1.2 milliseconds.  Some DMX hardware may not be able to handle faster refresh times.

int serial3TXbuffersizeminus1;

void setup()
{
  pinMode(status_led, OUTPUT);
  digitalWrite(status_led, HIGH);
  // Serial3 is pin8
  Serial3.begin(250000);

  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleVelocityChange(OnVelocityChange);
  
  delay(100);
  serial3TXbuffersizeminus1 = Serial3.availableForWrite();

  Serial.begin(9600);
  Serial.print("serial3 buffer size: ");
  Serial.print((serial3TXbuffersizeminus1 + 1)); // See how large buffer size is using Teensyduino serial monitor.
  // Increase serial3 buffer size to send more channels.
  // open file: hardware/teensy/avr/cores/teensy3/serial3.c
  // edit line: #define TX_BUFFER_SIZE 40

  DMXset(1, 255); // Usually red on an RGB light with start address 1.
  DMXupdate();
  delay(500);

  DMXset(1, 0);
  DMXupdate();
  delay(500);

  DMXset(2, 255); // Usually green on an RGB light with start address 1.
  DMXupdate();
  delay(500);

  DMXset(2, 0);
  DMXupdate();
  delay(500);

  DMXset(3, 255); // Usually blue on an RGB light with start address 1.
  DMXupdate();
  delay(500);

  DMXset(3, 0);
  DMXupdate();
  delay(500);
}

void loop()
{
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
      
      DMXset(channel, value);
    }
  }
  DMXupdate();
}

void DMXupdate() // Will only transmit if previous packet has finished.
{
  if (Serial3.availableForWrite() == serial3TXbuffersizeminus1) // Is serial3 buffer empty?
  {
    Serial3.begin(83333, SERIAL_8N1);
    Serial3.write(0);
    Serial3.flush();
    Serial3.begin(250000, SERIAL_8N2);
    Serial3.write(buffer, sizeof(buffer));
    // Avoid 2nd Serial3.flush() to free up processor for other operations.
  }
}

void DMXset(unsigned int channel, byte value)
{
  if (channel > 0 && channel < sizeof(buffer))
  {
    buffer[channel] = value;
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

void OnVelocityChange(byte channel, byte note, byte velocity) {
  midi_to_dmx(channel, note, velocity);
}

void midi_to_dmx(byte midi_channel, byte dmx_channel, byte value)
{  
  
  // 512 channels of DMX spread across 5 MIDI channels
  if (midi_channel >= 1 && midi_channel <= 5) 
  {
    if (midi_channel == 1)
      DMXset(dmx_channel, byte(value / 127. * 255));
    if (midi_channel == 2)
      DMXset(dmx_channel + 127, byte(value / 127. * 255));
    if (midi_channel == 3)
      DMXset(dmx_channel + 254, byte(value / 127. * 255));
    if (midi_channel == 4)
      DMXset(dmx_channel + 381, byte(value / 127. * 255));
    if (midi_channel == 5 && dmx_channel <= 4)
      DMXset(dmx_channel + 508, byte(value / 127. * 255));
    if (midi_channel == 5 && dmx_channel > 4)
      DMXset(512, byte(value / 127. * 255));
  }
}

// Converts two 7-bit bytes into a 14-bit int
int seven_to_fourteen(char byte_a, char byte_b) {
  return byte_a & 127 | byte_b << 7;
}
