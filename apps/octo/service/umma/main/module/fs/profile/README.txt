[UMMA_FS Profile ����]

�ۼ��� : ������(jykim2@humaxdigital.com)
�ۼ��� : 2014.01.10

1. ���ϻ���
  - �̸� : "�𵨸�_storage_profile" �������� ������ �����մϴ�. 
  - "�𵨸�"�� CONFIG_PRODUCT_NAME�� ������ ���� �մϴ�. 
  - ���� ������ JSON����, ���� ���� ������ �����Ͽ� �ۼ��մϴ�. 
  - ����� �ش� ������ rootfs �� /usr/etc/storage_profile.json ���� ����˴ϴ�. (Makefile�� �ڵ����� ���ݴϴ�.) 

2. JSON �ʵ� ����
 - README : ������ ���� �ʵ�� �� ������ �ǵ��� �����մϴ�. 
 - DeviceCount : ����ϴ� storage device Ÿ���� ������ �����մϴ�.
 - Profile : �� device�� profile ������ �����մϴ�.
    > TYPE : Device�� Ÿ���� �����մϴ�. ������ Ÿ���� �Ʒ� 6�� �Դϴ�. 
        1) Internal : ������ HDD
        2) Detachable : Ż�� ������ HDD (��, HMS �𵨿��� ���)
        3) eSATA_HDD : eSATA �������̽��� �����ϴ� ������ HDD
        4) USB_HDD : USB �������̽��� �����ϴ� ������ HDD
        5) USB_Flash : USB �޸� ��ƽ
        6) MMC : SD ī�� 
    > PVR : PVR�� Ÿ���� �����մϴ�. 
        1) Main : �⺻���� PVR�� ����� �����մϴ�. �Ϲ����� PVR ����� �ǹ��մϴ�.
        2) Sub : HMS�� Detachable HDD�� ���� Main HDD�� �뷮�� ���ڸ��� ���� ���ܻ��׿����� PVR�� HDD�μ� ���� �մϴ�. 
	3) Tsr : TSR�� ���������� Recording�� �������� �ʴ� ���� TSR ���� Device�� �ǹ� �մϴ�.
        3) None : PVR�� �����Ҽ� ���� Device �Դϴ�. 
    > FS : format �� �����ϴ� file system�� �ǹ��մϴ�.
        1) EXT2
        2) EXT3
        3) EXT4
        4) FAT16
        5) FAT32 
        6) NTFS
        7) XFS 
        8) None : format�� �������� �ʴ� ���� �������� device 

