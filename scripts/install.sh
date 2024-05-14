#!/bin/bash

logi() {
    echo -e "\e[32m[INFO] $1\e[0m"
}

loge() {
    echo -e "\e[31m[ERROR] $1\e[0m"
}

CDIR=$(pwd)

install_pack() {
    REQUIRED_PACKAGES=(
        git
        dialog
        bc
        device-tree-compiler
        build-essential
        libncurses5-dev
        flex
        bison
        openssl
        libssl-dev
        dkms
        raspberrypi-kernel-headers
        i2c-tools
    )
    MISSING_PACKAGES=()

    for package in "${REQUIRED_PACKAGES[@]}"; do
        if ! dpkg -s "$package" >/dev/null 2>&1; then
            MISSING_PACKAGES+=("$package")
        fi
    done

    if [ ${#MISSING_PACKAGES[@]} -ne 0 ]; then
        logi "The following packages are missing, they will be installed :${MISSING_PACKAGES[@]}"
        sudo apt update >/dev/null 2>&1
        sudo apt install -y "${MISSING_PACKAGES[@]}" >/dev/null 2>&1
    fi
}

install_drive() {
    cd $CDIR/tiny
    make >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        loge "Driver build failed!!!"
        exit 1
    fi

    logi "Driver compilation completed"

    sudo cp st7796.ko /lib/modules/$(uname -r)/kernel/drivers/gpu/drm/tiny/ >/dev/null 2>&1

    sudo depmod >/dev/null 2>&1
    sudo modprobe st7796 >/dev/null 2>&1

    sudo dtc -I dts -O dtb -o $CDIR/rpi-overlays/fly-tft-v2.dtbo $CDIR/rpi-overlays/fly-tft-v2.dts >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        loge "Device tree overlay build failed!!!"
        exit 1
    fi
    logi "Device tree overlay compilation completed"

    sudo dtoverlay $CDIR/rpi-overlays/fly-tft-v2.dtbo >/dev/null 2>&1
    sudo cp $CDIR/rpi-overlays/*.dtbo /boot/overlays/ >/dev/null 2>&1

    sudo cp $CDIR/scripts/99-fbdev.conf /etc/X11/xorg.conf.d/99-fbdev.conf >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        logi "You may not have services like Xorg and KlipperScreen installed yet!!!"
    fi
    sudo chmod +x /etc/X11/xorg.conf.d/99-fbdev.conf >/dev/null 2>&1
}

set_configs() {
    CONFIG_FILE="/boot/firmware/config.txt"
    PARAM_LINE="dtparam=i2c_arm=on"
    FLYTFT_LINE="dtoverlay=fly-tft-v2"

    if [ ! -f "$CONFIG_FILE" ]; then
        loge "The $CONFIG_FILE file does not exist, please use the latest version of the system (bookworm)"
        exit 1
    fi
    if grep -q "^#${PARAM_LINE}" "$CONFIG_FILE"; then
        sudo sed -i "s/^#${PARAM_LINE}/${PARAM_LINE}/" "$CONFIG_FILE"
        logi "i2c_arm enabled"
    elif ! grep -q "^${PARAM_LINE}" "$CONFIG_FILE"; then
        echo "${PARAM_LINE}" | sudo tee -a "$CONFIG_FILE" >/dev/null
        logi "Added ${PARAM_LINE} to ${CONFIG_FILE}."
    else
        logi "i2c_arm enabled"
    fi

    if grep -q "^#${FLYTFT_LINE}" "$CONFIG_FILE"; then
        sudo sed -i "s/^#${FLYTFT_LINE}/${FLYTFT_LINE}/" "$CONFIG_FILE"
        logi "fly-tft-v2 enabled"
    elif ! grep -q "^${FLYTFT_LINE}" "$CONFIG_FILE"; then
        echo "${FLYTFT_LINE}" | sudo tee -a "$CONFIG_FILE" >/dev/null
        logi "Added ${FLYTFT_LINE} to ${CONFIG_FILE}."
    else
        logi "fly-tft-v2 enabled"
    fi
}

ask_for_reboot() {
    read -p "Want to restart the system? (Enter Yes/Y, enter No/N, default is No): " answer
    case "$answer" in
    [Yy] | [Yy][Ee][Ss])
        logi "Reboot..."
        sudo reboot
        ;;
    [Nn] | [Nn][Oo] | "")
        logi "The driver has been installed and needs to be restarted to take effect."
        ;;
    *)
        loge "Driver installation completed"
        ;;
    esac
}

main() {
    install_pack
    install_drive
    set_configs
    ask_for_reboot
}

main
