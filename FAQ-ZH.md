# 常见问题

> [English FAQ](./FAQ.md)

<details> 
    <summary><font size="4">为什么屏幕显示正常，电容触摸不能使用？</font></summary>

1. 确认是否正确安装驱动程序
2. 确认是否添加正确配置参数到config.txt
3. 输入 `i2cdetect -y -r 1` 检查触摸设备状态
4. 如果出现下面的情况，说明触摸驱动正常
    ```bash
    pi@mainsailos:~ $ i2cdetect -y -r 1
        0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    00:                         -- -- -- -- -- -- -- -- 
    10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    50: -- -- -- -- -- -- -- -- -- -- -- -- -- UU -- -- 
    60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    70: -- -- -- -- -- -- -- -- 
    ```
5. 如果出现下面的情况，说明触摸驱动没有正确安装
    ```bash
    pi@mainsailos:~ $ i2cdetect -y -r 1
        0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    00:                         -- -- -- -- -- -- -- -- 
    10: -- -- -- -- 14 -- -- -- -- -- -- -- -- -- -- -- 
    20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    70: -- -- -- -- -- -- -- -- 
    ```
6. 当出现上面步骤**5**的情况(驱动异常)，可以尝试以下操作：
    1. 将config.txt中添加的 `dtoverlay=fly-tft-v2` 修改为 `dtoverlay=fly-tft-v2-0x14`
    2. 重启设备
7. 如果出现上述步骤**4**的情况(驱动正常)，可以尝试以下操作：
   1. 执行 `hexdump /dev/input/event0` 命令
   2. 手指触摸屏幕，观察命令行输出
   3. 如果命令行输出中出现大量字符则驱动正常，请检查你的KlipperScreen的触摸配置
   4. 如果命令行中无任何输出，说明驱动异常，请联系我们进行解决
   5. `CTRL + C` 退出命令行

</details>