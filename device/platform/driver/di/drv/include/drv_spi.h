/*******************************************************************************
* File name : drv_spi.h
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/
 
#ifndef _DRV_SPI_H_
#define _DRV_SPI_H_

/*******************************************************************************
* Headers
*******************************************************************************/


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
typedef enum
{
	DRV_SPI_SDCARD = 0,
}DRV_SPI_DEVICE_e;

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
* function : DRV_SPI_Init
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_Init (void);

/*******************************************************************************
* function : DRV_SPI_SetVccCtrlMode
* description : auto <-> static(by gpio) vcc control of 8024
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_SPI_SetVccCtrlMode(unsigned char ucUseGpio);

/*******************************************************************************
* function : DRV_SPI_SetPower
* description : Drive Power of TDA8024
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_SetPower (unsigned char ucOn);

/*******************************************************************************
* function : DRV_SPI_SelectDevice
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_SPI_SelectDevice(unsigned char ucSelect);

/*******************************************************************************
* function : DRV_SPI_SetBaudRate
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_SetBaudRate(DRV_SPI_DEVICE_e eDevice, unsigned int unBaudRate);

/*******************************************************************************
* function : DRV_SPI_TransmitPacket
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_TransmitPacket(DRV_SPI_DEVICE_e eDevice, unsigned char mode, unsigned char *pucTxData, int nTxDataSize);

/*******************************************************************************
* function : DRV_SPI_ReceivePacket
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_ReceivePacket (DRV_SPI_DEVICE_e eDevice, unsigned char mode,unsigned char *pucRxData, int nRxDataSize);

DRV_Error DRV_SPI_SetSpiMode(HBOOL bCPHA, HBOOL bCPOL);

#endif /* _DRV_SPI_H_ */


/* end of file */
