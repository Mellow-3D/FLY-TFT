#!/bin/bash

CDIR=$(pwd)

sudo apt update
sudo apt install -y git dialog bc device-tree-compiler build-essential libncurses5-dev flex bison openssl libssl-dev dkms raspberrypi-kernel-headers i2c-tools

cd $CDIR/tiny
make 
sudo cp st7796.ko /lib/modules/`uname -r`/kernel/drivers/gpu/drm/tiny/

sudo depmod
sudo modprobe st7796

sudo dtc -I dts -O dtb -o $CDIR/rpi-overlays/fly-tft-v2.dtbo $CDIR/rpi-overlays/fly-tft-v2.dts
sudo dtc -I dts -O dtb -o $CDIR/rpi-overlays/fly-tft-v2-r.dtbo $CDIR/rpi-overlays/fly-tft-v2-r.dts
sudo dtc -I dts -O dtb -o $CDIR/rpi-overlays/fly-tft-v2-0x14.dtbo $CDIR/rpi-overlays/fly-tft-v2-0x14.dts

# sudo dtoverlay $CDIR/rpi-overlays/fly-tft-v2.dtbo
# sudo dtoverlay $CDIR/rpi-overlays/fly-tft-v2-r.dtbo
sudo cp $CDIR/rpi-overlays/*.dtbo /boot/overlays/

sudo cp $CDIR/scripts/99-fbdev.conf /etc/X11/xorg.conf.d/99-fbdev.conf
chmod +x /etc/X11/xorg.conf.d/99-fbdev.conf

echo "请在/boot/config.txt中添加以下内容以启用fly-tft-v2"
echo "Please add the following content in /boot/config.txt to enable fly-tft-v2"
echo "电容屏(Capacitive touchscreen):"
echo "dtoverlay=fly-tft-v2"
echo "电阻屏(Resistive touchscreen):"
echo "dtoverlay=fly-tft-v2-r"
echo "修改后重启生效"