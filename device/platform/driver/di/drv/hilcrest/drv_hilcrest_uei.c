/*******************************************************************************
* File name : drv_hilcrest_uei.c
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */
#include "callbacks.h"
#include "uapi.h"

/* humax header files */
#include "drv_err.h"
#include "vkernel.h"
#include "di_uart.h"
#include "di_rf4ce.h"
#include "di_rf4ce_priv.h"
#include "drv_micom.h"
#include "drv_hilcrest.h"


/*******************************************************************************
* Debug level
*******************************************************************************/
#ifdef DRV_HILCREST_UEI_DEBUG
#define PrintInfo		DI_UART_Print
#define PrintDebug	DI_UART_Print
#define PrintError	DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug	while (0) ((int (*)(char *, ...)) 0)
#define PrintError	DI_UART_Print
#define PrintInfo		DI_UART_Print
#endif


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
extern RF4CE_INSTANCE_t g_tRF4CEInstance;
extern unsigned char DRV_RF4CE_GetCurrentPairingTableIndex(void);
#endif


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static pfnDRV_HILCREST_RF4CEMotion_Notify pfnRelativeMotionCallback = NULL;


/*******************************************************************************
* Static function prototypes
*******************************************************************************/
#if (SUPPORT_HILCREST_ALLFUNCS)
static unsigned char P_HILCREST_GetCurrentPairingTableIndex (void);
#endif
static void P_HILCREST_RelativeMotionUeiCallback (void* pvdata);
#if (SUPPORT_HILCREST_ALLFUNCS)
static void P_HILCREST_AbsoluteMotionUeiCallback (void* pvdata);
static void P_HILCREST_TextUeiCallback (void* pvdata);
static void P_HILCREST_RawSensorUeiCallback (void* pvdata);
#endif
static void P_HILCREST_BatteryStatusUeiCallback (void* pvdata);

static void P_HILCREST_GeneralParameters (UAPI_configParamStruct_t* ptMyThing);
static void P_HILCREST_RelativeMotionSettings ( UAPI_configParamStruct_t* ptMyThing );
static void P_HILCREST_AbsolutePositionSettings (UAPI_configParamStruct_t* ptMyThing);
static void P_HILCREST_SettingsUeiCallback (void* pvdata);


/*******************************************************************************
* function : DRV_HILCREST_InitUEIHilCresetModule
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_HILCREST_InitHilCresetLib (void)
{
	DI_UART_Print("UEI Initialization Start~~~\n");
	
	/* Initialize UEI Lib */	
	UAPI_Init();

	/* Register User Callback to UEI Lib */
	UAPIRegisterCallback( UAPI_RELATIVEMOTION_NAME, P_HILCREST_RelativeMotionUeiCallback); 
	UAPIRegisterCallback( UAPI_BATTERYSTATUS_NAME, P_HILCREST_BatteryStatusUeiCallback);
	UAPIRegisterCallback( UAPI_CONFIGURATION_NAME, P_HILCREST_SettingsUeiCallback);

#if (SUPPORT_HILCREST_ALLFUNCS)
	UAPIRegisterCallback( UAPI_ABSOLUTEPOSITION_NAME, P_HILCREST_AbsoluteMotionUeiCallback);
	UAPIRegisterCallback( UAPI_TEXTENTRY_NAME, P_HILCREST_TextUeiCallback);
	UAPIRegisterCallback( UAPI_RAWSENSORDATA_NAME, P_HILCREST_RawSensorUeiCallback);
#endif

	return DRV_OK;
}

#if (SUPPORT_HILCREST_ALLFUNCS)
/*******************************************************************************
* function : DRV_HILCREST_EnableRelativeMotion
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_HILCREST_EnableRelativeMotion (unsigned char ucEnable)
{
	UAPI_configParamStruct_t* ptRelativeMotionParams = NULL;
	unsigned char ucCurrentPairingTableIndex=0;

	PrintDebug("[DRV_HILCREST_EnableRelativeMotion] Enable Relative Motion(%d)\n", ucEnable);

	ptRelativeMotionParams = UAPI_createParamStruct(ptRelativeMotionParams, 1, UAPIFeature_RelativeMotion);
	UAPI_setParam(ptRelativeMotionParams, 0, RelMotionEnable, (void*)&ucEnable, sizeof(ucEnable));

	ucCurrentPairingTableIndex = P_HILCREST_GetCurrentPairingTableIndex();
	UAPIConfigureFeature((void*)ptRelativeMotionParams, UAPI_RF4CE_NIF, ucCurrentPairingTableIndex); 
	
	PrintDebug("[DRV_HILCREST_EnableRelativeMotion] press ok/select on remote\n");
	return;
}

/*******************************************************************************
* function : DRV_HILCREST_EnableRawSensorData
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_HILCREST_EnableRawSensorData (unsigned char ucEnable)
{
	UAPI_configParamStruct_t* rawSensorDataParams = NULL;
	unsigned char ucCurrentPairingTableIndex=0;

	PrintDebug("[DRV_HILCREST_EnableRawSensorData] Enable Raw Sendor Data(%d)\n", ucEnable);
	
	rawSensorDataParams = UAPI_createParamStruct(rawSensorDataParams, 1, UAPIFeatureIDRawSensorData);
	UAPI_setParam(rawSensorDataParams, 0, RawSensorDataEnable, (void*)&ucEnable, sizeof(ucEnable));
	
	ucCurrentPairingTableIndex = P_HILCREST_GetCurrentPairingTableIndex();
	UAPIConfigureFeature((void*) rawSensorDataParams, UAPI_RF4CE_NIF, ucCurrentPairingTableIndex); 

	VK_TASK_Sleep(5); /* from SDK code */

	return;
}

/*******************************************************************************
* function : DRV_HILCREST_ChangeRawSensorDataSpeed
* description : This function changes "polling rate of UIE lib not RCU.  that means RCU still sends packet every 10ms 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_HILCREST_ChangeRawSensorDataSpeed (unsigned char ucMultiPlier)
{
	UAPI_configParamStruct_t* ptRelativeMotionParams = NULL;
	unsigned char ucCurrentPairingTableIndex=0;

	PrintDebug("[DRV_HILCREST_ChangeRawSensorDataSpeed] Touch Pad Polling Multipiler(%d)\n", ucMultiPlier);

	ptRelativeMotionParams = UAPI_createParamStruct(ptRelativeMotionParams, 1, 0);
	UAPI_setParam(ptRelativeMotionParams, 0, 3, (void*)&ucMultiPlier, sizeof(ucMultiPlier));

	ucCurrentPairingTableIndex = P_HILCREST_GetCurrentPairingTableIndex();
	UAPIConfigureFeature((void*)ptRelativeMotionParams, UAPI_RF4CE_NIF, ucCurrentPairingTableIndex); 
	UAPI_destroyParamStruct(ptRelativeMotionParams);
	
	PrintDebug("[DRV_HILCREST_ChangeRawSensorDataSpeed] press ok/select on remote\n");
    	return;
}

/*******************************************************************************
* function : P_HILCREST_GetCurrentPairingTableIndex
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static unsigned char P_HILCREST_GetCurrentPairingTableIndex (void)
{
#if defined(CONFIG_RF4CE)
	return DRV_RF4CE_GetCurrentPairingTableIndex();
#else
#error "P_HILCREST_GetCurrentPairingTableIndex: Invalid Configuration"
#endif
}
#endif
/*******************************************************************************
* function : DRV_HILCREST_RegisterRf4ceMotionEventCallback
* description : 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_HILCREST_RegisterRelativeMotionCallback (pfnDRV_HILCREST_RF4CEMotion_Notify pfnNotify)
{
	pfnRelativeMotionCallback = pfnNotify;

	return DRV_OK;
}

/*******************************************************************************
* function : P_HILCREST_RelativeMotionUeiCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_RelativeMotionUeiCallback (void* pvdata)
{
	UAPIFeatureData_t* ptRelativeData = (UAPIFeatureData_t*)pvdata;
	UAPI_RelativeMotion_t* ptRelativeMotionData = (UAPI_RelativeMotion_t*)ptRelativeData->featureData;

	PrintDebug("[P_HILCREST_RelativeMotionUeiCallback] button1 = %02X, button2 = %02X, button3 = %02X, fingerPresent = %02X\n",  
	                  ptRelativeMotionData->buttonInfo.button1, ptRelativeMotionData->buttonInfo.button2, ptRelativeMotionData->buttonInfo.button3,ptRelativeMotionData->buttonInfo.fingerPresent);

	if((ptRelativeMotionData->xDisplacement || ptRelativeMotionData->yDisplacement) && (ptRelativeMotionData->verticalScroll == 0) && (ptRelativeMotionData->horizontalPan == 0))
	{
		PrintDebug("[P_HILCREST_RelativeMotionUeiCallback] xDisplacement = %d, yDisplacement = %d\n", ptRelativeMotionData->xDisplacement, ptRelativeMotionData->yDisplacement);
		if(pfnRelativeMotionCallback != NULL)
		{
			pfnRelativeMotionCallback(ptRelativeMotionData->xDisplacement, ptRelativeMotionData->yDisplacement, 0);
		}
	}
	else if((ptRelativeMotionData->verticalScroll || ptRelativeMotionData->horizontalPan) && (ptRelativeMotionData->xDisplacement == 0) && (ptRelativeMotionData->yDisplacement == 0))
	{
		PrintDebug("[P_HILCREST_RelativeMotionUeiCallback] vertical scroll = %d, horizontal pan = %d\n", ptRelativeMotionData->verticalScroll, ptRelativeMotionData->horizontalPan);
	}

	return;
}

/*******************************************************************************
* function : P_HILCREST_BatteryStatusUeiCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_BatteryStatusUeiCallback (void* pvdata)
{
	UAPIFeatureData_t* ptBatteryData = (UAPIFeatureData_t*)pvdata;
	UAPI_BatteryStatus_t* ptBatteryStatus = (UAPI_BatteryStatus_t*)ptBatteryData->featureData;
	
	switch (ptBatteryStatus->status)
	{
		case 0:
			PrintDebug("[P_HILCREST_BatteryStatusUeiCallback] OK state\n");
			break;
		case 1 :	
			PrintDebug("[P_HILCREST_BatteryStatusUeiCallback] WARNING!!! state\n");
			break;
		case 2: 	
			PrintDebug("[P_HILCREST_BatteryStatusUeiCallback] CRITICAL!!! state\n");
			break;
		default:
			PrintDebug("[P_HILCREST_BatteryStatusUeiCallback] UNKNOWN!!! state\n");
			break;
	}
	
	PrintDebug("[P_HILCREST_BatteryStatusUeiCallback] battery level = %d percentage\n", ptBatteryStatus->level);
	PrintDebug("[P_HILCREST_BatteryStatusUeiCallback] battery value = %x\n", ptBatteryStatus->voltage);

	return;
}

/*******************************************************************************
* function : P_HILCREST_SettingsUeiCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_SettingsUeiCallback (void* pvdata)
{
	int i;
	UAPI_remoteParams_t* ptConfigData = (UAPI_remoteParams_t*)pvdata;
	UAPI_configParamStruct_t* ptMyThing = (UAPI_configParamStruct_t*)ptConfigData->remoteParams;

	PrintDebug("[P_HILCREST_SettingsUeiCallback] param struct is for feature %02X\n", ptMyThing->featureId);
	PrintDebug("[P_HILCREST_SettingsUeiCallback] param struct has %d parameters\n", ptMyThing->numParams);
	
	switch(ptMyThing->featureId)
	{
		case UAPIFeatureIDGeneralServices:
			P_HILCREST_GeneralParameters(ptMyThing);
			break;
		case UAPIFeatureIDRelativeMotion:
			P_HILCREST_RelativeMotionSettings(ptMyThing);
			break;
		case UAPIFeatureIDAbsolutePosition:
			P_HILCREST_AbsolutePositionSettings(ptMyThing);
			break;
	}
	
	for(i=0;i<ptMyThing->numParams;i++)
	{
		switch(ptMyThing->featureId)
		{
			case UAPIFeatureIDBatteryStatus:
				switch(ptMyThing->param[i].attributeId )
				{
					case BatteryStatusEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] Battery Status Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;
				}
		   		break;
		   		
			case UAPIFeatureIDOrientation:
				switch(ptMyThing->param[i].attributeId )
				{
					case OrientationEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] Orientation Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;
				}
				break;
			
			case UAPIFeatureIDUserActivity:
				switch(ptMyThing->param[i].attributeId )
				{
					case UserActivityEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] User Activity Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;
				}
				break;
				
			case UAPIFeatureIDText:
				switch(ptMyThing->param[i].attributeId )
				{
					case TextEntryEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] Text Entry Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;
					case SymShiftModeEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] Text Entry Symbol Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;

					case CapsModeEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] Text Entry CapsLock Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;

					case AltModeEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] Text Entry Alt Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;
				}
				break;
			
			case UAPIFeatureIDUrcKey:
				switch(ptMyThing->param[i].attributeId )
				{
					case URCKeyEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] URC Key Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;
				}
				break;
			
			case UAPIFeatureIDRemoteAPI:
				switch(ptMyThing->param[i].attributeId)
				{
					case RemoteAPIEnable:
						PrintDebug("[P_HILCREST_SettingsUeiCallback] RemoteAPI Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
						break;
				}
				break;
		}
	}
	PrintDebug("\n");
}

#if (SUPPORT_HILCREST_ALLFUNCS)
/*******************************************************************************
* function : P_HILCREST_AbsoluteMotionUeiCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_AbsoluteMotionUeiCallback (void* pvdata)
{
	UAPIFeatureData_t* ptAbsoluteData = (UAPIFeatureData_t*)pvdata;
	UAPI_AbsoluteMotion_t* ptAbsoluteMotionData = (UAPI_AbsoluteMotion_t*)ptAbsoluteData->featureData;

	PrintDebug("[P_HILCREST_AbsoluteMotionUeiCallback] button1 = %02X, button2 = %02X, button3 = %02X, fingerPresent = %02X\n",
	                   ptAbsoluteMotionData->buttonInfo.button1, ptAbsoluteMotionData->buttonInfo.button2, ptAbsoluteMotionData->buttonInfo.button3, ptAbsoluteMotionData->buttonInfo.fingerPresent);

	PrintDebug("[P_HILCREST_AbsoluteMotionUeiCallback] xPosition = %d\n", ptAbsoluteMotionData->xPosition);
	PrintDebug("[P_HILCREST_AbsoluteMotionUeiCallback] yPosition = %d\n", ptAbsoluteMotionData->yPosition);

	return;
}

/*******************************************************************************
* function : P_HILCREST_TextUeiCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_TextUeiCallback (void* pvdata)
{
    UAPIFeatureData_t* ptTextData = (UAPIFeatureData_t*)pvdata;
    UAPI_TextEntry_t* ptTextInput = (UAPI_TextEntry_t*)ptTextData->featureData;
    
	switch(ptTextInput->charType)
	{
		case 4:
		case 1:
			PrintDebug("[P_HILCREST_TextUeiCallback] key [%c] was pressed\n", ptTextInput->value.UTF8char[0]);
			break;
		case 2:
		case 3:
			break;
	}

	return;
}

/*******************************************************************************
* function : P_HILCREST_RawSensorUeiCallback
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_RawSensorUeiCallback (void* pvdata)
{
	 int i;
	 UAPIFeatureData_t* ptUapi_data = (UAPIFeatureData_t*)pvdata;
	 UAPI_RawSensorData_t* ptRawSensor_data = (UAPI_RawSensorData_t*)ptUapi_data->featureData;
	 
	PrintDebug("[P_HILCREST_RawSensorUeiCallback] Sequence Number = %d\n", ptRawSensor_data->seqNum);
	PrintDebug("[P_HILCREST_RawSensorUeiCallback] Button			= %d, %d\n", ptRawSensor_data->button.button_pressed, ptRawSensor_data->button.button_released);
	PrintDebug("[P_HILCREST_RawSensorUeiCallback] length of data is %d\n", ptRawSensor_data->length);

	for(i=0;i<ptRawSensor_data->length;i++)
	{
		PrintDebug("0x%x ", ptRawSensor_data->sensorData[i]);
	}	
	PrintDebug("\n");

	return;
}
#endif


/*******************************************************************************
* function : P_HILCREST_GeneralParameters
* description : used by P_HILCREST_SettingsUeiCallback()
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_GeneralParameters (UAPI_configParamStruct_t* ptMyThing)
{
	int j;
	UINT32 unCpuData = 0;
	UINT8 ucUapiEnable = 0;
	UINT32 unVersion = 0;
	UINT32 unPollRate = 0;
	UINT8 ucPollState = 0;
	UAPI_productInfo_t tProductInfo;

	if(UAPI_getParam(ptMyThing, UapiEnbState, (void*)&ucUapiEnable) == UAPI_SUCCESS)
	{
		PrintDebug("[P_HILCREST_GeneralParameters] UAPI ENABLE = %d\n", ucUapiEnable);
	}
        
	if(UAPI_getParam(ptMyThing, UapiVersionInfo, (void*)&unVersion) == UAPI_SUCCESS)
	{
		PrintDebug("[P_HILCREST_GeneralParameters] Version: %d.%d.%d\n", ((unVersion>>0)&0xFF), ((unVersion>>8)&0xFF), ((unVersion>>16)&0xFF)|((unVersion>>16)&0xFF));
	}

	if(UAPI_getParam(ptMyThing,UapiProductInfo, (void*)&tProductInfo) == UAPI_SUCCESS)
	{
		PrintDebug("[P_HILCREST_GeneralParameters] UAPI Version: %d.%d.%d\n", ((tProductInfo.swVersion>>0 )&0xFF), ((tProductInfo.swVersion>>8 )&0xFF), ((tProductInfo.swVersion>>16)&0xFF)|((tProductInfo.swVersion>>16)&0xFF));
		PrintDebug("[P_HILCREST_GeneralParameters] UAPI Product Name: ");

		for(j=0;j<MAX_PRODUCT_NAME;j++)
		{
			PrintDebug("%c", tProductInfo.productName[j]);
		}	
	        PrintDebug("\n");
	}
       
	if(UAPI_getParam(ptMyThing, UapiPollingRate, (void*)&unPollRate) == UAPI_SUCCESS)
	{
		PrintDebug("[P_HILCREST_GeneralParameters] UAPI Polling Rate: %02X%02X%02X%02X\n", ((unPollRate>>24)&0xFF), ((unPollRate>>16)&0xFF), ((unPollRate>>8)&0xFF), ((unPollRate)&0xFF));
	}

	if(UAPI_getParam(ptMyThing, UapiAutoPollState, &ucPollState) == UAPI_SUCCESS)
	{
		PrintDebug("[P_RF4CE_GeneralParameters] UAPI Auto Poll State : %d\n", ucPollState);
	}

	if(UAPI_getParam(ptMyThing, UapiCPUHwInfo, &unCpuData) == UAPI_SUCCESS)
	{
		PrintDebug("[P_HILCREST_GeneralParameters] HW INFO- REV: %02X\n", (unCpuData>>24)&0xFF);
		PrintDebug("[P_HILCREST_GeneralParameters] HW INFO- IR:  %02X\n",((unCpuData>>16)&0xFF));
		PrintDebug("[P_HILCREST_GeneralParameters] HW INFO- RF:  %02X\n",((unCpuData>>8)&0xFF));
		PrintDebug("[P_HILCREST_GeneralParameters] HW INFO- CPU: %02dMHz\n",((unCpuData)&0xFF)*4);
	}
}

/*******************************************************************************
* function : P_HILCREST_RelativeMotionSettings
* description : used by P_HILCREST_SettingsUeiCallback()
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_RelativeMotionSettings ( UAPI_configParamStruct_t* ptMyThing )
{
	int i;
	
	for(i=0;i<ptMyThing->numParams;i++)
	{
		switch( ptMyThing->param[i].attributeId )
		{
			case RelMotionFirstPhysicalToLogical:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] First Physical to Logical : %d\n", ptMyThing->param[i].value.UINT16_data);
				break;

			case RelMotionDecelPhysicalToLogical:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Decel Physical to Logical : %d\n", ptMyThing->param[i].value.UINT16_data);
				break;

			case RelMotionEnable:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Enable State : %d\n", ptMyThing->param[i].value.UINT8_data);
				break;

			case RelMotionAccelEnable:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Acceleration Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
				break;

			case RelMotionDecelEnable:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Deceleration Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
				break;

			case RelMotionFingerPresenceNotifyEnable:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Finger Present Notification Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
				break;

			case RelMotionPhysicalToLogical:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Physical To Logical: %d\n", ptMyThing->param[i].value.UINT16_data);
				break;

			case RelMotionTraverseTime:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Traverse Time (in ms): %d\n", ptMyThing->param[i].value.UINT16_data);
				break;

			case RelMotionDecelCoefficient:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Deceleration Coefficient: %d\n", ptMyThing->param[i].value.UINT16_data);
				break;

			case RelMotionDecelSampleTime:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Deceleration Sample Time (in ms): %d\n", ptMyThing->param[i].value.UINT16_data);
				break;

			case RelMotionTxIntervalMs:
				PrintDebug("[P_HILCREST_RelativeMotionSettings] Relative Motion Transmit Interval Time (in ms): %d\n", ptMyThing->param[i].value.UINT16_data);
				break;
		}
	}
}

/*******************************************************************************
* function : P_HILCREST_AbsolutePositionSettings
* description : used by P_HILCREST_SettingsUeiCallback()
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_HILCREST_AbsolutePositionSettings (UAPI_configParamStruct_t* ptMyThing)
{
	int i;
	
	for(i=0;i<ptMyThing->numParams;i++)
	{
		switch(ptMyThing->param[i].attributeId )
		{
			case AbsMotionEnable:
				PrintDebug("[P_HILCREST_AbsolutePositionSettings] Absolute Motion Enable State: %d\n", ptMyThing->param[i].value.UINT8_data);
				break;

			case AbsMotionTxIntervalMs:
				PrintDebug("[P_HILCREST_AbsolutePositionSettings] Absolute Motion Transmit Interval Time (in ms): %d\n", ptMyThing->param[i].value.UINT16_data);
				break;
		}
	}
}


/*******************************************************************************
* function : UAPI_RF4CESend
* description : this api is used by uei hilcrest lib
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
INT16 UAPI_RF4CESend(UAPIDataPkt *uapiDataPkt)
{
	int i,j;
	unsigned char aucUapiPacket[DRV_MICOM_MESSAGE_BYTES-5];
	
	PrintDebug("[UAPI_RF4CESend] Tx Data : Device ID(0x%x) RemoteID(0x%x), Length(%d)\n", uapiDataPkt->deviceID, uapiDataPkt->remoteID, uapiDataPkt->dataLength);
	PrintDebug("[UAPI_RF4CESend] Tx UAPI Data : Option(0x%x), SeqNum(%d), appID(0x%x), Opcode(0x%x)\n", uapiDataPkt->uapiHdr.UAPIOptions, uapiDataPkt->uapiHdr.seqNum, uapiDataPkt->uapiHdr.appID, uapiDataPkt->uapiHdr.opCode);

	for (j=0,i=0; i<uapiDataPkt->dataLength; i++)
	{
		if (j>10)
		{
			PrintDebug("\n");
		}
		PrintDebug("0x%2x ", uapiDataPkt->data[i]);
	}
  
	/* UAPI Header */
	VK_MEM_Memcpy(&aucUapiPacket[0], &uapiDataPkt->uapiHdr, sizeof(UAPIHdr_t));

	/* UAPI data */
	VK_MEM_Memcpy(&aucUapiPacket[0+sizeof(UAPIHdr_t)], uapiDataPkt->data, uapiDataPkt->dataLength);

	/* call MICOM command to send UAPI data to RCU */
	DRV_MICOM_TransmitZrcUserData(uapiDataPkt->remoteID ,&aucUapiPacket[0], sizeof(sizeof(UAPIHdr_t))+uapiDataPkt->dataLength);
	
	return uapiDataPkt->dataLength;
}

/* end of file */
