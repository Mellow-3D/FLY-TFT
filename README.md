# FLY-TFT

[中文](./README-ZH.md)

## This is the RPI driver repository for FLY-TFT-V2

> We have received feedback from users that if capacitive touch is not available, please follow [FAQ](./FAQ.md) to troubleshoot the problem.

### 1. Introduction

FLY-TFT-V2 is a TFT LCD screen based on the ST7796 controller, supporting both capacitive and resistive touch input. It has a resolution of 320x480 pixels and communicates via the SPI interface.

<table>
<tr>
<td><img src="./images/ks.jpg" title="KlipperScreen" width="200" border=10></td>
<td><img src="./images/dp.jpg" title="Desktop" width="200" border=10></td>
</tr>
<tr>
<td><img src="./images/shu.jpg" title="vertical" width="200" border=10></td>
<td><img src="./images/heng.jpg" title="Horizontal" width="200" border=10></td>
</tr>
</table>

### 2. System Installation

> Note: The kernel version used by your Raspberry Pi must be greater than `5.17.x`. Run the command `uname -r` to check your current kernel version.

* Install the latest **MainsailOS** using [Raspberry Pi Imager](https://www.raspberrypi.com/software/)
    1. Download and install [Raspberry Pi Imager](https://www.raspberrypi.com/software/) 
    2. Open **Raspberry Pi Imager**
    3. Click **CHOOSE OS**
    4. Select the appropriate model for your device
    5. Click **CHOOSE OS** again
    6. Select **Other specific-purpose OS**
    7. Choose **3D printing**
    8. Select **Mainsail OS**
    9. Choose the latest version available. If your setup supports 64-bit systems, select **rpi64**
    10. Click **CHOOSE STORAGE**
    11. Select your storage device, such as an SD card
    12. Click **WRITE**, and wait for the installation to complete.

### 3. Driver Installation

1. Install the FLY-TFT-V2 driver:
    ```bash
    git clone https://github.com/kluoyun/FLY-TFT.git
    cd FLY-TFT
    sudo chmod +x ./scripts/install.sh
    ./scripts/install.sh
    ```

### 4. Enable TFT display

1. After installing the driver, you need to add the overlay support of `dtoverlay=fly-tft-v2` in the config.txt file.
2. Open `/boot/config.txt` (`/boot/firmware/config.txt` for bookworm system)
     ```bash
     sudo nano /boot/config.txt
     ```
3. Add the following configuration at the end of the file
    ```bash
     dtoverlay=fly-tft-v2
    ```
    * The default screen is 90-degree horizontal screen display. If you need to rotate the screen, please use the following configuration
    ```bash
     dtoverlay=fly-tft-v2,r90 # Same as the default direction, horizontal
     dtoverlay=fly-tft-v2,r270 # 270-degree horizontal screen, horizontal (flip)
     dtoverlay=fly-tft-v2,r0 # 0 degree vertical screen, portrait orientation
     dtoverlay=fly-tft-v2,r180 # 180-degree vertical screen, portrait (flip)
     ```
     * Only one configuration can be added, multiple configurations cannot be added
     * Under normal circumstances, the touch direction of the TFT will automatically rotate to follow the display direction, without modifying the touch configuration in the system
4. After adding it to the end of the file, execute the command `sudo reboot` to restart the system.
   
### 5. Usage TFT

* Ensure the hardware connections are correct.
* Install the driver as instructed above.
* Add the overlay support `dtoverlay=fly-tft-v2` or `dtoverlay=fly-tft-v2-r` in the `/boot/config.txt` (`/boot/firmware/config.txt` for bookworm system) file.
* In some systems, there might be a default fb0 device; FLY-TFT may be assigned to fb1. You need to enable the fb1 device if it appears.
* Run the command `ls /dev/fb*` to view the devices. If you see both fb0 and fb1, execute the following commands to enable the fb1 device (default is fb0).
* Execute the following command to modify the configuration to use fb1 instead of fb0:
    ```bash
    sudo sed -i 's/\/dev\/fb0/\/dev\/fb1/g' /etc/X11/xorg.conf.d/99-fbdev.conf
    ```

### 6. Usage KlipperScreen

* KlipperScreen is not installed by default on MainsailOS and needs to be installed manually.
* Please refer to [KlipperScreen](https://github.com/KlipperScreen/KlipperScreen) or use [kiauh](https://github.com/dw-0/kiauh) to install
* If KlipperScreen is installed successfully and step 5 has been completed correctly, the KlipperScreen interface should be displayed at this time

### 7. All available configurations

* `speed` [**Parameter required**]: Set the maximum SPI clock frequency of TFT, in Hz, the default is 96000000 (please lower this value appropriately if severe screen blur occurs)
   * Example: `dtoverlay=fly-tft-v2,speed=80000000`

* `r0/r90/r180/r270` [**Optional**][**No parameters required**]: Set the display and touch direction of TFT, the default is 90 degrees, the optional values are 0,90,180,270, (only A configuration can be added)
   * Example: `dtoverlay=fly-tft-v2,r90` or `dtoverlay=fly-tft-v2,r180`

* `disable_touch` [**No parameters required**]: Disable the touch function of TFT, the default is enabled
   * Example: `dtoverlay=fly-tft-v2,disable_touch`

* `invx`, `invy` [**No parameters required**]: Set the touch direction of TFT, `invx` is the X direction flip, `invy` is the Y direction flip
   * Example: `dtoverlay=fly-tft-v2,invx` or `dtoverlay=fly-tft-v2,invy` or `dtoverlay=fly-tft-v2,invx,invy`

* `swapxy` [**No parameters required**]: Set the touch direction of TFT, the X axis and Y axis are interchanged, that is, horizontal screen
     * Example: `dtoverlay=fly-tft-v2,swapxy`

> **Note:** All parameters can be set at the same time (only one setting can be selected for optional parameters). Multiple parameters are separated by commas. Multiple parameters can only be on the same line.

> **Warning:** Under normal circumstances, only four parameters `r0`, `r90`, `r180`, `r270` are used, other parameters are only used in special circumstances, especially `invx`, `invy`,` swapxy`
* Example: `dtoverlay=fly-tft-v2,speed=80000000,r270`

### 8. Feedback

> We've tested this on the latest MainsailOS system. If you encounter any issues, please feel free to provide feedback through Github Issues.
