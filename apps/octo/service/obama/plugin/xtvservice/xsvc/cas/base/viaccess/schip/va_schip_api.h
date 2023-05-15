/*
	Viaccess Secured Chipset API
		Secured Chipset Driver API Document Supplement ACS 3.0 - Doc. Ref. 2316, p.3
*/

#ifndef VA_SCHIP_API_H
#define VA_SCHIP_API_H

void VA_SCHIP_Init();

int VA_SCHIP_GetCwMode();

#if defined(CONFIG_PROD_CPU_BCM)
#if defined(CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW)
int VA_SCHIP_SetProtectKey(HUINT32 ulDscIndex);
#endif
#endif

#endif //VA_SCHIP_API_H


