/**
	@file	  port_itk_dvr.h													\n
	@brief	  itk nvs header file for DVR porting layer						\n

	Description: ITK에서 NVS Porting 에 사용되는 기능을 정의한 header 파일  	\n
	Module: mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n
*/


#ifndef _PORT_ITK_DVR_H
#define _PORT_ITK_DVR_H

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include "itk_dvr.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


extern void	PORT_ITK_Dvr_ExecuteEventCallback(void);
extern int	PORT_ITK_Dvr_CancelBookingByBookingId(const char *bookingId);
extern void	PORT_ITK_Dvr_Reset(void);
extern void	PORT_ITK_Dvr_CreateObject(void);


#endif 	/* end of _PORT_ITK_DVR_H */



