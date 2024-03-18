# FLY-TFT

[English](./README.md)

## 这是FLY-TFT-V2的RPI驱动仓库

> 收到用户反馈，遇到电容触摸不可用的情况，请按照 [FAQ](./FAQ-ZH.md) 排查问题


### 1. 介绍

FLY-TFT-V2是一款基于st7796的TFT液晶屏，支持电容触摸与电阻触摸两种，分辨率320x480，使用SPI接口

### 2. 系统安装

> 注意：树莓派使用的系统内核版本必须大于`5.17.x`, 执行命令`uname -r`查看内核版本

* 使用 [Raspberry Imager](https://www.raspberrypi.com/software/) 安装最新的 **MainsailOS**
    1. 下载并安装 [Raspberry Imager](https://www.raspberrypi.com/software/) 
    2. 打开**Raspberry Imager**
    3. 点击**CHOOSE DEVICE**
    4. 根据你的设备选择对应型号
    5. 点击**CHOOSE OS**
    6. 选择**Other specific-purpose OS**
    7. 选择**3D printing**
    8. 选择**Mainsail OS**
    9. 选择最新的版本。如果你的设置支持64位系统则选择**rpi64**
    10. 点击**CHOOSE STORAGE**
    11. 选择你的存储设备，比如SD卡
    12. 点击**NEXT**，等待安装完成

### 3. 驱动安装

1. 安装FLY-TFT-V2驱动
    ```bash
    git clone https://github.com/kluoyun/FLY-TFT.git
    cd FLY-TFT
    sudo chmod +x ./scripts/install.sh
    ./scripts/install.sh
    ```

### 4. 使用TFT

* 请确保硬件连接正确
* 安装了驱动
* 在`/boot/config.txt`文件中添加了`dtoverlay=fly-tft-v2`或`dtoverlay=fly-tft-v2-r`的覆盖层支持
* 如果已完成上述所有步骤，请执行`sudo reboot`重启系统
* 部分系统可能存在默认的fb0设备，FLY-TFT会被分配到fb1设备，需要修改配置文件使能fb1设备
* 执行命令`ls /dev/fb*`查看设备，如果出现两个设备fb0和fb1，请执行下面的命令将fb1设备使能（默认是fb0）
* 执行下面的命令修改默认配置为fb1设备
    ```bash
    sudo sed -i 's/\/dev\/fb0/\/dev\/fb1/g' /etc/X11/xorg.conf.d/99-fbdev.conf
    ```

### 5. 使用KlipperScreen

* MainsailOS默认没有安装KlipperScreen，需要手动安装
* 请参考[KlipperScreen](https://github.com/KlipperScreen/KlipperScreen)或使用[kiauh](https://github.com/dw-0/kiauh)安装
* 如果成功安装了KlipperScreen，且第5步骤已正确完成，此时应该已经显示了KlipperScreen界面

### 6. 反馈

> 我们在最新的MainsailOS系统上测试通过，如果有问题欢迎通过Github Issues反馈。