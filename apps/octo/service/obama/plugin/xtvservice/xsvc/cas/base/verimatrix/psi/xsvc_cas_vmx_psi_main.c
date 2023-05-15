/* (c) 2011-2012 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/**
	@file     xsvc_cas_vmx_psi_main.c
	@brief    xsvc_cas_vmx_psi_main.c (Verimatrix PSI MAIN)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2005 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptv_main.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#endif
#include "_xsvc_cas_vmx_psi.h"
#include "_xsvc_cas_vmx_os.h"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*	
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

// VMX_TODO : bc psi로 이동해서 진행하자.

HERROR xsvc_vmx_psi_Init(void)
{
	HUINT32	nResult = 0;

	if(xsvc_vmx_psi_session_Init() != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}
	
	if(xsvc_vmx_psi_stream_InitCore() != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}
	
	if(xsvc_vmx_psi_ecm_InitCore() != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}

	if(xsvc_vmx_psi_emm_InitCore() != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}

	return nResult;	
}

/*********************** End of File ******************************/

