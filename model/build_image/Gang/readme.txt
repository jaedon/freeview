1. default 디렉토리에 nvram, db, dbbackup 영역 데이터를 zip으로 압축한 상태로 복사.
  db.bin.zip
  dbbackup.bin.zip
  nvram.bin.zip

2. images 디렉토리에 커널 & 루트파일시스템 이미지 복사
  vmlinuz
  root.squa

3. makegang.bat 실행

4. 3번 과정 완료 후 2개의 이미지가 생성됩니다. (gang.hdf, ubiarea_gang.bin)
  * gang.hdf는 USB 다운로드용 이미지입니다. (loader영역은 포함되어 있지 않음)
  * ubiarea_gang.bin 파일은 ubi_img_gen 디렉토리로 옮긴 후, 해당 디렉토리의 readme.txt 파일을 참조하여 gang writer용 이미지를 생성할 수 있습니다.
