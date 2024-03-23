# 常见问题

> [中文 FAQ](./FAQ-ZH.md)
<details>
     <summary><font size="4">Why does the screen display normally but capacitive touch cannot be used? </font></summary>

1. Confirm whether the driver is installed correctly
2. Confirm whether the correct configuration parameters are added to config.txt
3. Enter `i2cdetect -y -r 1` to check the touch device status
4. If the following situation occurs, it means the touch driver is normal
    ```bash
    pi@mainsailos:~ $ i2cdetect -y -r 1
        0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    00:                         -- -- -- -- -- -- -- -- 
    10: -- -- -- -- UU -- -- -- -- -- -- -- -- -- -- -- 
    20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    70: -- -- -- -- -- -- -- -- 
    ```
5. If the following situation occurs, it means that the touch driver is not installed correctly.
    ```bash
    pi@mainsailos:~ $ i2cdetect -y -r 1
        0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    00:                         -- -- -- -- -- -- -- -- 
    10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    50: -- -- -- -- -- -- -- -- -- -- -- -- -- 5d -- -- 
    60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    70: -- -- -- -- -- -- -- -- 
    ```
6. When the above step **5** occurs (driver exception), you can try the following operations:
     1. Modify the `dtoverlay=fly-tft-v2` added in config.txt to `dtoverlay=fly-tft-v2-0x5d`
     2. Restart the device
7. If the above step **4** occurs (the driver is normal), you can try the following operations:
    1. Execute `hexdump /dev/input/event0` command
    2. Touch the screen with your finger and observe the command line output
    3. If a large number of characters appear in the command line output, the driver is normal. Please check the touch configuration of your KlipperScreen.
    4. If there is no output in the command line, it means the driver is abnormal. Please contact us for resolution.
    5. `CTRL + C` Exit the command line

</details>