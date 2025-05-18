# laser_target_watergun
Laser Targetting watergun with esp32 cam and esp32 controlling the laser module.


[Explanatory video](https://youtu.be/Ic2vv7IDxok)
[![Demo Video](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/thumbnail.png)](https://youtu.be/Ic2vv7IDxok)

__Note:__ The __User_Setup.h__ file contains the configuration for the elecrow red display with the esp32. If using cheap yellow display need to set TFT_BL to -1. This would normally be set to 21, but pin 21 is held high by a pullup resistor, so the screen will still work.

## Directories:
__3d print files:__ This contains the files to 3d print the handset.

__schematics:__ This contains the schematics for the wiring for both the watergun side and the tft side

__tft_side_watergun_v2:__ The software for the tft side with the laser module attached

__watergun_side_v3:__ The software for the watergun side

__________________________________________

## Bill of Materials:

### Laser module with TFT screen
1. Laser Module. Note: laser module is not exaclty the same as laser pointer. Search amazon or aliexpress for "laser module". I'm using this one: ![dot laser module](https://www.amazon.com.au/650nm-Laser-Module-Adjustable-Industrial/dp/B07PV4BWVH/)
2. TFT with esp32 connected. I'm using the elecrow red display, but you can use a cheap yellow display or a tft with an esp32. I'm using this one: ![elecrow red display](https://www.elecrow.com/esp32-display-2-8-inch-hmi-display-spi-tft-lcd-touch-screen.html)
3. Push buttons x 2.
4. Pull up resistors X 2. I'm using 6.8 kohm, but anywhere from 5k to 15k should be ok.
5. 18650 battery and battery holder
6. Jumper wires, 3d printer,  hot glue, solder, etc...

![laser_module_handset](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/handset_photo.JPEG)

### Water gun side with camera
1. Esp32 cam. I'm using the AI thinker but any esp32 cam should be ok.
2. Electric wategun. The one I'm using isn't in production anymore. But any electric watergun that can fit easily onto a servo pan-tilt mechanism. Some might have an awkward shape at the bottom that makes it difficult. So keep this in mind. Also should have easy access to battery wiring should work.
3. High torque Servos x 2. I'm using 25kg/cm servos. You can get these from aliexpress.
4. Servo pan-tilt mechanism. One for size mg995 servo should work for the 25kg/cm servos from aliexpress. Don't get the smaller ones for the sg90 servos.
5. Buck converters X 2: one for the 6.5V supply for the servos and one for the 5V supply for the esp32 and watergun. I'm using the LM2596.
6. TB6612FNG motor controller.
7. Small breadboard.
8. Battery. I'm using a 2S lipo. But anything that works with the buck converters should be ok.
9. Jumper cables, hot glue, etc... 

![watergun_side](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/watergun_photo.JPEG)


previous videos on wategun are at:

![esp32 watergun](https://youtu.be/s5WNyXVWSS8)

![esp32 watergun with tracking](https://youtu.be/NK1wj7sDLNc)
