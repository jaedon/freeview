[UMMA_FS Profile 사용법]

작성자 : 김정연(jykim2@humaxdigital.com)
작성일 : 2014.01.10

1. 파일생성
  - 이름 : "모델명_storage_profile" 형식으로 파일을 생성합니다. 
  - "모델명"은 CONFIG_PRODUCT_NAME의 정보와 동일 합니다. 
  - 파일 형식은 JSON으로, 기존 모델의 파일을 참고하여 작성합니다. 
  - 참고로 해당 파일은 rootfs 상에 /usr/etc/storage_profile.json 으로 저장됩니다. (Makefile이 자동으로 해줍니다.) 

2. JSON 필드 설명
 - README : 설명을 위한 필드로 본 문서의 의도와 동일합니다. 
 - DeviceCount : 사용하는 storage device 타입의 개수를 기입합니다.
 - Profile : 각 device의 profile 정보를 기입합니다.
    > TYPE : Device의 타입을 기입합니다. 가능한 타입은 아래 6개 입니다. 
        1) Internal : 내장형 HDD
        2) Detachable : 탈착 가능한 HDD (예, HMS 모델에서 사용)
        3) eSATA_HDD : eSATA 인터페이스를 지원하는 외장형 HDD
        4) USB_HDD : USB 인터페이스를 지원하는 외장형 HDD
        5) USB_Flash : USB 메모리 스틱
        6) MMC : SD 카드 
    > PVR : PVR의 타입을 기입합니다. 
        1) Main : 기본적인 PVR로 사용이 가능합니다. 일반적인 PVR 기능을 의미합니다.
        2) Sub : HMS의 Detachable HDD와 같이 Main HDD의 용량이 모자르는 등의 예외사항에서만 PVR용 HDD로서 동작 합니다. 
	3) Tsr : TSR은 가능하지만 Recording은 지원하지 않는 기기로 TSR 전용 Device를 의미 합니다.
        3) None : PVR로 동작할수 없는 Device 입니다. 
    > FS : format 시 지원하는 file system을 의미합니다.
        1) EXT2
        2) EXT3
        3) EXT4
        4) FAT16
        5) FAT32 
        6) NTFS
        7) XFS 
        8) None : format을 지원하지 않는 등의 예외적인 device 

