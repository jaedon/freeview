/*          Software_Version: BC_2_1.0

            Provider_name: Test-Stream

            Description: Provider Identification and Data

            generated: Mon Mar 1 12:32:12 2005

            Copyright: COMVENIENT  2005 (c)

********************************************************************************
*******************************************************************************/
/**************************** CVS Infos ****************************************
*

*  $Source:$

*  $Date:$

*  $Revision:$

*

***************************** CVS Infos ***************************************/
#include "_xsvc_cas_vmx_bc_main.h"
#include "_xsvc_cas_vmx_os.h"
#include "vmx_vr_bc_Individualization.h"

/*
	- 여기서 사용되는 NSc_ProviderId, NSc_ProviderData 는 only 개발용으로만 사용되는 id임
	- 양산에는 절대 사용해서는 아됨.
*/

#if defined(CONFIG_DEBUG)
unsigned char NSc_ProviderId[8]=
{
  0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

unsigned char NSc_ProviderData[] =
{
  0x60, 0x08, 0x00, 0x00, 0xAB, 0x17, 0xDF, 0x9B,
  0x07, 0xBA, 0x1C, 0x7B, 0xBD, 0xB6, 0xE7, 0x4A,
  0xA3, 0x5A, 0x61, 0x49, 0x4B, 0xDA, 0x31, 0xFA,
  0xD1, 0xFB, 0x8F, 0x5B, 0xDA, 0xCD, 0x50, 0x1B,
  0xC8, 0x21, 0x7B, 0xB3, 0x1D, 0x16, 0x37, 0x2F,
  0xB2, 0x99, 0x22, 0x03, 0xF5, 0xE3, 0xA1, 0x87,
  0x40, 0xB5, 0xC1, 0xAF, 0xCF, 0x7E, 0xCB, 0x1B,
  0x09, 0x47, 0x7E, 0x7D, 0xE8, 0xB1, 0x0A, 0x23,
  0x84, 0xFE, 0x08, 0x1A, 0x7B, 0x8C, 0x29, 0xBA,
  0x7E, 0xFE, 0x89, 0xCC, 0x73, 0x80, 0x90, 0x9E,
  0x40, 0x9A, 0x40, 0x3D, 0x61, 0x6F, 0x3A, 0xAB,
  0xE9, 0xB4, 0xF6, 0x42, 0x88, 0x96, 0xB5, 0xF1,
  0xE5, 0x0B, 0x9E, 0x7C, 0xE3, 0x27, 0x70, 0x6A,
  0x52, 0x77
};

HERROR vmx_vr_bc_indiv_GetProviderId(HUINT8 *pucProviderId)
{
	if(pucProviderId == NULL)
		return ERR_FAIL;

	VMX_memcpy(pucProviderId, NSc_ProviderId, sizeof(NSc_ProviderId));
	return ERR_OK;
}

HERROR vmx_vr_bc_indiv_GetProviderData(HUINT8 *pucProviderData)
{
	if(pucProviderData == NULL)
		return ERR_FAIL;

	VMX_memcpy(pucProviderData, NSc_ProviderData, sizeof(NSc_ProviderData));
	return ERR_OK;
}
#else
unsigned char NSc_ProviderId[8]=
{
	// TODO_VMX : for manufacture
};

unsigned char NSc_ProviderData[] =
{
	// TODO_VMX : for manufacture
};

HERROR vmx_vr_bc_indiv_GetProviderId(HUINT8 *pucProviderId)
{
	// TODO_VMX : for manufacture
	return ERR_FAIL;
}

HERROR vmx_vr_bc_indiv_GetProviderData(HUINT8 *pucProviderData)
{
	// TODO_VMX : for manufacture
	return ERR_FAIL;
}
#endif