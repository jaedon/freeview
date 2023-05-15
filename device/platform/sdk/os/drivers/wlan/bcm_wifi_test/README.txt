1. Tools download.
�� down script�� �Ʒ� ���� �߰�.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test drr_package/trunk/wireless/broadcom/wifi_test HEAD

2. WI-FI drivers.
MFG & Production driver�� ��� download�ؾ� ��.
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

4. install test images. (Firmware�� NVRAM downloading�� ���� �ʴ� ��� 3)&4) Skip)
1) drivers�� �Ʒ� ��ο� copy ��.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/driver/[driver_name]_mfg.ko
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/driver/[driver_name]_prd.ko

2) wl app 
wl�� Kernel dependency�� ����, prd/mfg �����̹Ƿ�, prd/mfg �߿� �Ѱ��� �����ص� ������.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/bin

3)firmware�� ���� ��ο� copy ��.
�߿��� ���� firmware�� nodis �ɼ��� ���� ���� ���� �ؾ� ��.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/firmware/mfg
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/firmware/prd

4) nvrams
�� ���� NVRAM�� copy ��.
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/nvrams

5. env.sh ����.
�Ʒ� ���Ͽ��� WIFI_TYPE�� �𵨿� �°� ���� �ʿ�.(set to BCM43236B_USB or BCM43228_PCIe)
device/platform/sdk/os/drivers/wlan/bcm_wifi_test/env.sh

6. Test
1) booting.
2) inset USB
3) sh
4) cd /media/bcm_wifi_test
6) ./eth.sh
7) ifconfig (IP �ּ� Ȯ��)
8) UART port ������ ���� ��.
9) telnet ���� (ID :wifi)
10) cp -rf /media/bcm_wifi_test /tmp/
11) chmod -R 777 /tmp/bcm_wifi_test
11) cd /tmp/bcm_wifi_test
12) ./wifi_hmx.sh init [mfg or prd]
13) etc tests.
