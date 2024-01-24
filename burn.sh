#! /bin/bash
sudo avrdude -p m328p -c usbasp -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -U flash:w:lightsout.hex:i
