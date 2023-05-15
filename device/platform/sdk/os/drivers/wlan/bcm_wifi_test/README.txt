1. Tools download.
모델 down script에 아래 라인 추가.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test drr_package/trunk/wireless/broadcom/wifi_test HEAD

2. WI-FI drivers.
MFG & Production driver를 모두 download해야 함.
ex) BCM43236
CONFIG_BCM43236=y
CONFIG_BCM43236_MFG=y

3. HW-Test SW Config & Build.
1) Disable config
# CONFIG_DINETWORK is not set
# CONFIG_WPA_SUPPLICANT is not set

2) Enable config
CONFIG_ROOTFS_TELNETD=y
CONFIG_WIFI_SCRIPT_TEST=y

4) build
make [model]_hwtest_release_defconfig.
make build  

4. install test images. (Firmware나 NVRAM downloading을 하지 않는 경우 3)&4) Skip)
1) drivers를 아래 경로에 copy 함.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/driver/[driver_name]_mfg.ko
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/driver/[driver_name]_prd.ko

2) wl app 
wl은 Kernel dependency가 없고, prd/mfg 공통이므로, prd/mfg 중에 한개만 복사해도 무관함.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/bin

3)firmware를 다음 경로에 copy 함.
중요한 점은 firmware가 nodis 옵션이 없는 것을 선택 해야 함.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/firmware/mfg
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/firmware/prd

4) nvrams
각 모델의 NVRAM을 copy 함.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/nvrams

5. env.sh 변경.
아래 파일에서 WIFI_TYPE을 모델에 맞게 변경 필요.(set to BCM43236B_USB or BCM43228_PCIe)
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/env.sh

6. Test
1) booting.
2) inset USB
3) sh
4) cd /media/bcm_wifi_test
6) ./eth.sh
7) ifconfig (IP 주소 확인)
8) UART port 연결을 빼도 됨.
9) telnet 접속 (ID :wifi)
10) cp -rf /media/bcm_wifi_test /tmp/
11) chmod -R 777 /tmp/bcm_wifi_test
11) cd /tmp/bcm_wifi_test
12) ./wifi_hmx.sh init [mfg or prd]
13) etc tests.
