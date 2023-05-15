/**
	@file     _xom_cas.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/17		initiated					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XOM_CAS_INT_H__
#define	__XOM_CAS_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA)
HERROR xom_cas_nagra_Init_BASE(void);
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
HERROR xom_cas_irdeto_Init_BASE(void);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX)
HERROR xom_cas_verimatrix_Init_BASE(void);
#endif

#if defined(CONFIG_MW_CI)
HERROR xom_cas_ci_Init_BASE(void);
#endif


#endif /* __XOM_CAS_INT_H__ */


