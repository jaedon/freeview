/*******************************************************************************
* File name : drv_hilcrest.h
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/
 
#ifndef _DRV_HILCREST_H_
#define _DRV_HILCREST_H_

/*******************************************************************************
* Headers
*******************************************************************************/


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define SUPPORT_HILCREST_ALLFUNCS	0
typedef void (*pfnDRV_HILCREST_RF4CEMotion_Notify)(int nXDisplacement, int nYDisplacement, int nZDisplacement);

/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/


/*******************************************************************************
* Static function prototypes
*******************************************************************************/


/*******************************************************************************
* function : DRV_HILCREST_InitHilCresetLib
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_HILCREST_InitHilCresetLib (void);

/*******************************************************************************
* function : DRV_HILCREST_RegisterRelativeMotionCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_HILCREST_RegisterRelativeMotionCallback (pfnDRV_HILCREST_RF4CEMotion_Notify pfnNotify);

#if (SUPPORT_HILCREST_ALLFUNCS)
/*******************************************************************************
* function : DRV_HILCREST_EnableRelativeMotion
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_HILCREST_EnableRelativeMotion (unsigned char ucEnable);

/*******************************************************************************
* function : DRV_HILCREST_EnableRawSensorData
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_HILCREST_EnableRawSensorData (unsigned char ucEnable);

/*******************************************************************************
* function : DRV_HILCREST_ChangeRawSensorDataSpeed
* description : This function changes "polling rate of UIE lib not RCU.  that means RCU still sends packet every 10ms 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_HILCREST_ChangeRawSensorDataSpeed (unsigned char ucMultiPlier);

#endif
#endif /* _DRV_HILCREST_H_ */


/* end of file */
