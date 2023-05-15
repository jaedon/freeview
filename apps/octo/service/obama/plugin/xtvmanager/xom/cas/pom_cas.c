/**
	@file     pom_cas.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include "./local_include/_xom_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/





HERROR pom_cas_subcas_Init(void)
{
#if defined(CONFIG_MW_CAS_NAGRA)
		xom_cas_nagra_Init_BASE();
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
	xom_cas_irdeto_Init_BASE();
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX)
	xom_cas_verimatrix_Init_BASE();
#endif

#if defined(CONFIG_MW_CI)
	xom_cas_ci_Init_BASE();
#endif

	return ERR_OK;
}

