# FLY-TFT

## 这是FLY-TFT-V2的RPI驱动仓库


### 1. 介绍

FLY-TFT-V2是一款基于st7796的TFT液晶屏，支持电容触摸与电阻触摸两种，分辨率320x480，使用SPI接口

### 2. 硬件连接

使用FLY-G2T转接板与RPI连接，如下图所示

    ![image](images/FLY-TFT-V2_RPI_Connection.png)

### 3. 系统安装

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

### 4. 驱动安装

```bash
git clone https://github.com/fly-iot/FLY-TFT-V2.git
cd FLY-TFT-V2
make


```