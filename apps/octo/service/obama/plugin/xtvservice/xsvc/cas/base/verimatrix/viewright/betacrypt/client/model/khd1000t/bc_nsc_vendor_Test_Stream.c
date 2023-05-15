/*          Software_Version: BC_2_1.0

            Vendor_Name: Test-Stream

            Description: Vendor Identification and Data

            generated: Tue Mar 01 17:38:17 2005

            Copyright: COMVENIENT  2005 (c)

********************************************************************************
********************************************************************************/
/**************************** CVS Infos *****************************************                                                                                
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
unsigned char NSc_VendorId[8]=
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char NSc_VendorData[]=
{
	0x60, 0x08, 0x00, 0x00, 0x96, 0x76, 0xc5, 0x65, 
	0x8a, 0xe7, 0xc6, 0x50, 0xd1, 0x46, 0x5a, 0xed, 
	0xac, 0x04, 0x47, 0x32, 0x99, 0x67, 0x2e, 0x4c, 
	0xc0, 0x32, 0x88, 0x02, 0x9a, 0x36, 0x0d, 0x13, 
	0xbb, 0x54, 0x18, 0x75, 0x4c, 0x64, 0x9f, 0xf9, 
	0x60, 0xea, 0xfa, 0xde, 0x18, 0x9b, 0xe9, 0x0c, 
	0x23, 0x05, 0x8c, 0x07, 0x5c, 0xd2, 0x30, 0x97, 
	0x43, 0xbb, 0x9c, 0x44, 0xff, 0xab, 0xfb, 0x99, 
	0xf0, 0x7f, 0x55, 0xc9, 0xb6, 0x6a, 0xa5, 0x9d, 
	0x76, 0x64, 0xf1, 0x5b, 0xa1, 0xe9, 0x41, 0x3a, 
	0xc3, 0x4c, 0x83, 0xed, 0x62, 0xd8, 0x37, 0x1e, 
	0x90, 0x58, 0x79, 0xfc, 0x73, 0xfc, 0x4e, 0xfc, 
	0x1a, 0x9b, 0x86, 0xdb, 0x18, 0xd8, 0x91, 0x39, 
	0xbd, 0x8f
};

HERROR vmx_vr_bc_indiv_GetVendorId(HUINT8 *pucVendorId)
{
	if(pucVendorId == NULL)
		return ERR_FAIL;

	VMX_memcpy(pucVendorId, NSc_VendorId, sizeof(NSc_VendorId));
	return ERR_OK;
}

HERROR vmx_vr_bc_indiv_GetVendorData(HUINT8 *pucVendorData)
{
	if(pucVendorData == NULL)
		return ERR_FAIL;

	VMX_memcpy(pucVendorData, NSc_VendorData, sizeof(NSc_VendorData));
	return ERR_OK;
}
#else
unsigned char NSc_VendorId[8]=
{
	// TODO_VMX : for manufacture
};

unsigned char NSc_VendorData[]=
{
	// TODO_VMX : for manufacture
};

HERROR vmx_vr_bc_indiv_GetVendorId(HUINT8 *pucVendorId)
{
	// TODO_VMX : for manufacture
	return ERR_FAIL;
}

HERROR vmx_vr_bc_indiv_GetVendorData(HUINT8 *pucVendorData)
{
	// TODO_VMX : for manufacture	
	return ERR_FAIL;
}
#endif
