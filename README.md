# MIDI to DMX using the Teensy 3

A MIDI to DMX converter box based on Teensy 3.

## Using Control Change messages

Midi channel = DMX channel offset  
Controller number = DMX channel  
Controller value = DMX value (scaled to 0-512)

Controller numbers 1-127 on MIDI channel 1 = DMX channels 1 to 127  
Controller numbers 1-127 on MIDI channel 2 = DMX channels 128 to 254  
Controller numbers 1-127 on MIDI channel 3 = DMX channels 255 to 381  
Controller numbers 1-127 on MIDI channel 3 = DMX channels 382 to 508  
Controller numbers 1-4 on MIDI channel 5 = DMX channels 509 to 512

## Using Note messages

Midi channel = DMX channel offset  
Note number = DMX channel (Starting at note 1)  
Velocity value = DMX value (scaled to 0-512)

(See above mapping for note number to dmx channel conversion)

## Using System Exclusive messages

By using specially formatted sysex messages you can send the full DMX (0-255) resolution:

```c
240 127 value_lsb value_msb channel_lsb channel_msb 247
```

To create the msb and lsb parts of an integer:

```c
lsb = number & 127
msb = (number >> 7) & 127
```

To go the other way around we do:

```c
number = (lsb & 127) | (msb <<7)
```