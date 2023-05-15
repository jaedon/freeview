/*
	Viaccess NVM API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.33
*/

#ifndef VA_NVM_API_H
#define VA_NVM_API_H

// VA���� �� 16KB�� ����� ���̰�..
//
//  offset: 0     |     system data ��   (size: 16K) |
//  offset: 16K |   Viaccess - Ext ���� (size:8K)  |  ==> VA MW���� message ���� �� ��Ÿ �뵵�� ��� (see VA_EMULATE_FLASH_BY_EEPROM)
//  offset: 24K |   Viaccess - ACS ���� (size:8K)| ==> VA Library���� ���

#if 0
#define VA_NVM_ACS_DATA_SIZE		(kVA_SETUP_NVM_SIZE_IN_BYTE)		// 8192 (8K)
#define VA_NVM_ACS_DATA_OFFSET	(E2P_EXT_ZONE_SIZE - VA_NVM_ACS_DATA_SIZE)	// ext ������ �����ٰ� ������ ����... eeprom�� �ǳ�..
#define VA_NVM_EXT_DATA_SIZE		8192
#define VA_NVM_EXT_DATA_OFFSET	(E2P_EXT_ZONE_SIZE - VA_NVM_ACS_DATA_SIZE - VA_NVM_EXT_DATA_SIZE)	// ext ������ �����ٰ� ������ ����... eeprom�� �ǳ�..
#else
#define VA_NVM_ACS_DATA_SIZE	 (kVA_SETUP_ACS_DATA_SEGMENT_SIZE)		// 24K
#define VA_NVM_ACS_DATA_OFFSET	 (VA_NVM_ACS_DATA_SIZE)
#define VA_NVM_EXT_DATA_SIZE	 8192
#define VA_NVM_EXT_DATA_OFFSET	 (VA_NVM_ACS_DATA_SIZE - VA_NVM_EXT_DATA_SIZE)
#define VA_NVM_ACS_LOADER_SIZE	 (kVA_SETUP_LOADER_DATA_SEGMENT_SIZE)		// 8K
#define VA_NVM_ACS_SOFTWARE_SIZE (kVA_SETUP_SOFTWARE_SEGMENT_SIZE)			//8M
#endif

void VA_NVM_Init();

int VA_INT_NVM_ClearAll();

#endif //VA_NVM_API_H

