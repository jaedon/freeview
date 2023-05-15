/**
	@file     cmd_mw_casmgr.c
	@brief    pvr test command tool

	Description: 		\n
	Module: 			 	\n
	Remarks : 										\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <_svc_cas_mgr_main.h>

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___

/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___


/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___


/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrMain" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___


/********************************************************************
* For Debugging
********************************************************************/
#define ___09_Debugging_APIs___


void CMD_Register_MW_CAS(void)
{
#define	hCmdHandle		"svc_cas"
	SVC_CAS_CmdInit(hCmdHandle);

	return;
}


