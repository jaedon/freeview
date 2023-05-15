FVP-5000T OAD 파일 생성 방법. (2017/07/14 진광호)

0. OpenCaster툴사용법을 참조하여 필요한 파일을 설치한다.

http://svn.humaxdigital.com/drr_octo/base/utility/OpenCaster3.1.4

   기타 필요한 package는 아래와 같다.
   apt-get install --no-install-recommends binutils gcc gcc-4.3 libc6-dev libgomp1 linux-libc-dev make python-dev python2.6-dev zlib1g-dev python-dateutil


1. OTA\hdf 폴더에 OAD할 hdf 파일을 다음의 이름으로 하여 copy 한다.
 현재 버전 : fvp5000t.hdf
 이후 버전 : fvp5000t_next.hdf

2. 파이썬 스크립트안에 적혀있는 업데이트할 버전을 수정한다.
  ssu_version.py

위의 스크립트의 version을 아래와 같이 OAD할 SW의 버전으로 수정한다.

(ex : 1.00.03)
#
# OTA test version of FVP-5000T
#
cur_hdr_ver_hi = 0x0001
cur_hdr_ver_lo = 0x0003

(ex : 2.00.03)
#
# OTA next test version of FVP-5000T
#
next_hdr_ver_hi = 0x0002
next_hdr_ver_lo = 0x0003

3. make_oad_all.sh 을 실행.

- 현재 버전 section 파일 생성
  2_ota_FVP-5000T_single_UKTFAE.sec

- 현재 버전 ts 파일 생성
  3_ota_FVP-5000T_single_UKTFAE.sec

- next 버전 ts 파일 생성
  4_ota_FVP-5000T_single_UKTFAE_next.sec

- (optional) next 버전 section 파일 생성
  4_ota_FVP-5000T_single_UKTFAE_next.sec

