FVP-5000T OAD ���� ���� ���. (2017/07/14 ����ȣ)

0. OpenCaster�������� �����Ͽ� �ʿ��� ������ ��ġ�Ѵ�.

http://svn.humaxdigital.com/drr_octo/base/utility/OpenCaster3.1.4

   ��Ÿ �ʿ��� package�� �Ʒ��� ����.
   apt-get install --no-install-recommends binutils gcc gcc-4.3 libc6-dev libgomp1 linux-libc-dev make python-dev python2.6-dev zlib1g-dev python-dateutil


1. OTA\hdf ������ OAD�� hdf ������ ������ �̸����� �Ͽ� copy �Ѵ�.
 ���� ���� : fvp5000t.hdf
 ���� ���� : fvp5000t_next.hdf

2. ���̽� ��ũ��Ʈ�ȿ� �����ִ� ������Ʈ�� ������ �����Ѵ�.
  ssu_version.py

���� ��ũ��Ʈ�� version�� �Ʒ��� ���� OAD�� SW�� �������� �����Ѵ�.

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

3. make_oad_all.sh �� ����.

- ���� ���� section ���� ����
  2_ota_FVP-5000T_single_UKTFAE.sec

- ���� ���� ts ���� ����
  3_ota_FVP-5000T_single_UKTFAE.sec

- next ���� ts ���� ����
  4_ota_FVP-5000T_single_UKTFAE_next.sec

- (optional) next ���� section ���� ����
  4_ota_FVP-5000T_single_UKTFAE_next.sec

