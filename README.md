# laser_target_watergun
Laser Targetting watergun with esp32 cam and esp32 controlling the laser module

__Note:__ The __User_Setup.h__ file contains the configuration for the elecrow red display with the esp32. If using cheap yellow display need to set TFT_BL to -1. This would normally be set to 21, but pin 21 is held high by a pullup resistor, so the screen will still work.

## Directories:
__3d print files:__ This contains the files to 3d print the handset.

__schematics:__ This contains the schematics for the wiring for both the watergun side and the tft side

__tft_side_watergun_v2:__ The software for the tft side with the laser module attached

__watergun_side_v3:__ The software for the watergun side



## Bill of Materials:

### Laser module with TFT screen
![laser_module_handset](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/handset_photo.JPEG)

### Water gun side with camera
![watergun_side](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/watergun_photo.JPEG)

### Servo test rig
![test_rig](https://github.com/jonathanrandall/laser_target_watergun/blob/main/pictures/test_rig_photo.JPEG)

