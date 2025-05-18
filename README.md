# laser_target_watergun
Laser Targetting watergun with esp32 cam and esp32 controlling the laser module

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
4. Jumper wires.
5. 18650 battery and battery holder

![laser_module_handset](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/handset_photo.JPEG)

### Water gun side with camera
![watergun_side](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/watergun_photo.JPEG)

### Servo test rig
![test_rig](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/test_rig_photo.JPEG)

