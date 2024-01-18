#! /bin/bash
sudo avrdude -p m328 -c usbasp -U flash:w:lightsout.hex:i
