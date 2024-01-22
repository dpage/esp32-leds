# ESP32 LEDs

Effects for individually addressable (NeoPixel) LED strips, based on Dave
Plummer's code at https://github.com/davepl/DavesGarageLEDSeries.

The project uses the Heltec WiFi Kit 32 v3 board: 
https://heltec.org/project/wifi-kit32-v3/

## Board Pins

J2, pin 1:  GND

J2, pin 3:  5V external PSU

J2, Pin 14: LED control signal (GPIO 05)

J3, pin 6:  External power detect (GPIO 47)

The external power detect pin should be fed 5V via a suitable diode, such as a
1N4007. This will bring the pin high when external power is present, allowing
the LEDs to run at full brightness, or a safe brightness when using USB power.

## Hardware

The *hardware/* sub directory contains a case design as a FreeCAD drawing,
along with *.3mf* models that can be sliced and 3D printed. The case lid holds
the microcontroller, with a port for the USB connector, buttons, and a hole
for the OLED.

To insert the microcontroller, push the USB connector into the hole, under the 
tab. Then use an M2 x 6mm self-tapping screw with a 2mm ID/5mm OD, 1mm thick
nylon washer to secure the other end of the board. The board should sit in the
recess on the post, and the washer should overlap the top of the board.

The body of the case has an 8MM hole at one end for a standard 2.5x5.5mm barrel
connector, and a 16mm hole with locator sides for a 3 pin aviation style 
connector.
