# FLY-TFT

[中文](./README-ZH.md)

## This is the RPI driver repository for FLY-TFT-V2

> We have received feedback from users that if capacitive touch is not available, please follow [FAQ](./FAQ.md) to troubleshoot the problem.

### 1. Introduction

FLY-TFT-V2 is a TFT LCD screen based on the ST7796 controller, supporting both capacitive and resistive touch input. It has a resolution of 320x480 pixels and communicates via the SPI interface.

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

### 4. Usage TFT

* Ensure the hardware connections are correct.
* Install the driver as instructed above.
* Add the overlay support `dtoverlay=fly-tft-v2` or `dtoverlay=fly-tft-v2-r` in the `/boot/config.txt` file.
* If all the steps above have been completed, execute `sudo reboot` to restart the system.
* In some systems, there might be a default fb0 device; FLY-TFT may be assigned to fb1. You need to enable the fb1 device if it appears.
* Run the command `ls /dev/fb*` to view the devices. If you see both fb0 and fb1, execute the following commands to enable the fb1 device (default is fb0).
* Execute the following command to modify the configuration to use fb1 instead of fb0:
    ```bash
    sudo sed -i 's/\/dev\/fb0/\/dev\/fb1/g' /etc/X11/xorg.conf.d/99-fbdev.conf
    ```

### 5. Usage KlipperScreen

* KlipperScreen is not installed by default on MainsailOS and needs to be installed manually.
* Please refer to [KlipperScreen](https://github.com/KlipperScreen/KlipperScreen) or use [kiauh](https://github.com/dw-0/kiauh) to install
* If KlipperScreen is installed successfully and step 5 has been completed correctly, the KlipperScreen interface should be displayed at this time

### 6. Feedback

> We've tested this on the latest MainsailOS system. If you encounter any issues, please feel free to provide feedback through Github Issues.
