/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:	template.h
// Original Author: Full Name!!
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__TEMPLATE_H__
#define	__TEMPLATE_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files

// End Including Headers


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee

// End Extern variable


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
// Start Macro definition

// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
void CH_MON_PrintTypeUnitId( DI_CH_Type_e etChannelType, int nUnitId );
void CH_MON_PrintAllState( CH_Status_e etCurrentStatus, CH_TsStatus_e etTsOnOff, CH_Active_e	etChannelAction );
void CH_MON_PrintStrenthQuality( int nChannelId, DI_CH_Type_e etChannelType );
void CH_MON_PrintSatTuneParam (CH_SAT_TuneParam_t* pstInputParam );
void CH_MON_PrintTerTuneParam (CH_TER_TuneParam_t* pstInputParam );
void CH_MON_PrintCabTuneParam (CH_CAB_TuneParam_t* pstInputParam );
void CH_MON_PrintAnaTuneParam (CH_ANA_TuneParam_t* pstInputParam );
// End global function prototypes

#endif /* !__TEMPLATE_H__ */
