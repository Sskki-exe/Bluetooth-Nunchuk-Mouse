# ESP32-Nunchuk-Mouse #
This repository is for a personal project of mine converting a Wii Nunchuk into a Bluetooth Mouse.

## Controls ##
These are the controls I've configured. The accelerometer is not used. The C button is used for "Change Mode", cycling from 0 to 2. The joystick and Z button will change their behaviour depending on which mode the Nunchuk Mouse is in:
0. Z button is left click. Joystick will move mouse around.
1. Z button is right click. If Z button is not pressed, joystick will scroll (horizontally and vertically). If Z button is pressed, joystick will move mouse around
2. Z button is middle click. Joystick will move mouse around

## Electronics ##
### Components ###
The inside of the Nunchuk Mouse contains:
* Original circuitry of the Nunchuk
* WROOM32 ESP32 for Bluetooth HID (the chip itself without dev board to save space)
* Custom PCB to mount ESP32 onto
* WEMOS Battery Shield for power and battery management
* 3.7V 250mah 502030 Li-Po battery
* 3.3V Buck Step Down Converter

### The Custom PCB ###
<img src="https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/esp32-pinout-chip-ESP-WROOM-32.png?resize=1024%2C523&quality=100&strip=all&ssl=1" height="300">
<img src="https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/ESP32-DOIT-DEVKIT-V1-Board-Pinout-36-GPIOs-updated.jpg?w=750&quality=100&strip=all&ssl=1" height="300">
The PCB has component holes connected to these pins of the ESP32 WROOM32:
* 3.3V
* GND
* RX (GPIO1)
* TX (GPIO3)
* I2C SCL (GPIO22)
* I2C SDA (GPIO21)
3.3V and GND are required for powering the ESP32. RX and TX are required for programming. The I2C pins are required for reading data from the Nunchuk.
The PCB also have pullup resistors and buttons for Enable and Boot (GPIO0). Do not forget these if you choose to design your own PCB.
#### For those who don't know how pullup resistors work ####
* A pullup resistor is a resistor connecting a pin (such as Enable) to high (3.3V)
* A button is connected to the same pin and ground
* Normally, the current flows from high, through the resistor, to the pin
* When the button is pressed, the current flows from high, through the resistor, to ground (path of least resistance)
If a button is just connected to high and a pin, when the button is not pressed, it "floating". This means pin may not be at ground voltage because of random electromagnetic waves in the air (from radios etc.) which may confuse a circuit into thinking the button was pressed. By using a pullup resistor, when the button is not pressed, the pin is "pulled up" to 3.3V while when the button is pressed, it is connected directly to ground so the pin is never floating.

### The Circuitry ###
#### Nunchuk to ESP32 ####
The circuitry of the Nunchuk remain unchanged, the cable is cut off and connected to the ESP32. If your Nunchuk is not genuine, the colors may be different so go by their position on the connector.

![Nunchuk Pinout](https://hackster.imgix.net/uploads/attachments/243612/nunchuk-plug-schematic.png?auto=compress%2Cformat&w=1280&h=960&fit=max)

Nunchuk      | Position on Nunchuk | ESP32
------------ | ------------------- | ----------------
White (GND)  | 4 Bottom Left       | GND
Green (SDA)  | 1 Top Left          | I2C SDA (GPIO21)
Yellow (SCL) | 6 Bottom Right      | I2C SCL (GPIO22)
Red (3V3)    | 3 Top Right         | 3.3V

#### Power Management ####
The power management board may need trimming:
* The battery connector was desoldered, the battery was soldered onto it directly
* The USB micro port was removed, the leads were extended and a new micro port can be soldered and positioned in the cable hole of the Nunchuk. I got a magnetic USB charging cable (for phones), mangled the phone side plug part to get to the tiny pcb inside and soldered it to the power management board. There's a magnetic connector at the bottom of my Nunchuck which is neat.
* The GPIO holes that aren't used as well as where the batter connector used to be can be Dremeled off at your own risk

The wiring for the power management is pretty straight forward:
WEMOS Battery Shield | Battery         | Buck Down Converter | ESP32
-------------------- | --------------- | ------------------- | -----
Battery Positive (+) | The red one duh | .                   | .
Battery Negative (-) | The black one   | .                   | .
5V (the GPIO one)    | .               | Input +             | .
GND (also GPIO one)  | .               | Input -             | .
.                    | .               | Output +            | 3.3V
.                    | .               | Output -            | GND


## Code ##
The Arduino code uses [W-vK's ESP32-BLE-Mouse Library](https://github.com/T-vK/ESP32-BLE-Mouse) and [Robert Eisele's Nunchuk Library](https://github.com/infusion/Fritzing/tree/master/Nunchuk). The ESP32-BLE-Mouse library can be installed as a zip file while the Nunchuk library is a .h file that needs to be included in the same folder as the .ino file. You will need to calibrate it and you can optionally customise the Bluetooth information.
### Calibration ###
The Nunchuk is not perfect and requires calibrations of deadzones and limits. Modify the code to your specific Nunchuk. Write a program that prints the Nunchuk joystick values to serial (you can ask me for help if you have no idea how to do that).
#### Deadzones ####
Leave your joystick in the "neutral position". The lowest and highest values for each axis are the low and high bounds of the deadzones.
Variable Name | Full Name
------------- | ----------------------------------
xDzL          | x axis deadzone lower bound
xDzH          | x axis deadzone higher/upper bound
yDzL          | y axis deadzone lower bound
yDzH          | y axis deadzone higher/upper bound
#### Physical Limits of Joystick ####
Push your joystick to each extreme (push it all the way left, right, up and down). The lowest and highest values for each axis are the low and high bounds of the x and y axes.
Variable Name | Full Name
------------- | -----------------------
xLow          | minimum x value
xHigh         | maximum x value
yLow          | minimum y value
yHigh         | maximum y value
#### Moving & Scrolling Speeds ###
This one is going to be trial and error. Change these values, upload to your Nunchuk Mouse, try it out and repeat until you're satisfied. I think these values should be fine for most people. As I implemented my code, the values must be integers so if you want finer adjustments, you'll have to edit the code.
Variable Name | Full Name
------------- | -----------------------
mSped         | move speed
sSped         | scroll speed
### Changing the Bluetooth Information ###
You can rename the name, manufacturer and batter level shown for the Nunchuk by changing the parameters in line 4 of code. 
* Replace "Nunchuk Mouse" with the device name you want to show up on your computer/etc.
* Replace "Sskki" with the manufacturuer name you want.
* Replace 69 with whatever battery percentage you want to show up on your computer/etc. because battery monitoring was not wired up.
```arduino
BleMouse bleMouse("Nunchuk Mouse","Sskki",69);
```
