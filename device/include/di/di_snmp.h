/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_snmp.h
// Original Author:
// File Description:
// Remarks: DI_SNMP	SNMP Private MIB for JC-6500

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/

#ifndef __DI_SNMP_H___
#define __DI_SNMP_H___

/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"

/* di headers */
#include "di_err.h"
#include "di_channel.h"


/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/
	
#define SEMKEYPATH "/dev/null"  // Path used on for ftok. Actual path is irrelevant.
#define SEMKEYID 1              // Id used on ftok for semget key

#define SHMKEYPATH "/dev/null"  // Path used on ftok. Actual path is irrelevant.
#define SHMKEYID 1              // Id used on ftok for shmget key		
#define SIZEOFSHMSEG sizeof(DI_SNMP_Information_t)	// Size of the shared mem segment

union semun    
{    
    int val;    
    struct semid_ds *buf;    
    unsigned short int *array;    
};    

#define SNMP_TASK_PRIORITY			61
#define SNMP_TASK_STACK_SIZE			(1024*2)//(1024*2)


/**
 * Define Stb Power Status
 */
typedef	enum
{
	STB_POWER_STATE_OFF = 0,
	STB_POWER_STATE_ON,
}DI_SNMP_StbPowerStatus_e;

/**
 * Define Stb Play Status
 */
typedef enum
{
	STB_PLAY_STATE_NO_WTV,
	STB_PLAY_STATE_WTV,
	STB_PLAY_STATE_HDD_PLAY,
	STB_PLAY_STATE_VOD,
	STB_PLAY_STATE_ETC
}DI_SNMP_StbPlayStatus_e; 


/**
 * Define SNMP Event 
 */

typedef enum
{
	PRIV_stbDevVerHardware=1,
	PRIV_stbDevVerSoftVender=2,
	PRIV_stbDevVerSoftMiddle=3,
	PRIV_stbDevVerSoftBrowser=4,
	
	PRIV_stbDevIdStb=5,
	PRIV_stbDevIdBcas=6,
	PRIV_stbDevIdCcas =7,
	
	PRIV_stbDevNetMacaddress=8,
	PRIV_stbDevNetIpaddress=9,
	PRIV_stbDevNetNetmask=10,
	
	PRIV_stbIfDownChannelPower=11,
	PRIV_stbIfDownChannelRxFrequency=12,
	
	PRIV_stbIfChannelIdService=13,
	PRIV_stbIfChannelIdOriginalnet=14,
	
	PRIV_stbIfSigQUnerroreds=15,
	PRIV_stbIfSigQCorrecteds=16,
	PRIV_stbIfSigQUncorrecteds=17,
	PRIV_stbIfSigQBER =18,
	
	PRIV_stbInfoPowerStatus=19,
	PRIV_stbInfoPlayStatus=20,
	
	PRIV_stbInfoTuner1ServiceId=21,
	PRIV_stbInfoTuner1NetworkId=22,
	PRIV_stbInfoTuner1Frequency=23,
	
	PRIV_stbInfoTuner2ServiceId=24,
	PRIV_stbInfoTuner2NetworkId=25,
	PRIV_stbInfoTuner2Frequency=26
}DI_SNMP_Event_e;


/**
 * Define SNMP Info which is set by MW.
 */

typedef struct
{
	HUINT8 szSystemId[10];			/* SYSTEM ID, stbDevVerHardware */
	HUINT8 szApplicationVer[10];	/* APP Version, stbDevVerSoftVender */
	HUINT8 szMiddleWareVer[10];		/* M/W Version, stbDevVerSoftMiddle */
	HUINT8 szSoftBrowserVer[10];	/* Browser Version, stbDevVerSoftBrowser */

	HUINT8 szStbID[6];				/* StbID, stbDevIdStb */
	HUINT8 szBcasCardId[32];		/* B CAS Card ID, stbDevIdBcas */
	HUINT8 szCcasCardId[32];		/* C CAS Card ID, stbDevIdCcas */

	HUINT32 ulDownFrequency;      /* Down Frequency, stbIfDownChannelRxFrequency */

        DI_CH_Type_e etWatchingChannelType; /* Watching Channel Type, for stbIfSignalQuailty MIB */
        HUINT32 ulWatchingTunerGroupId;     /* Watching TunerGroupId, for stbIfSignalQuality MIB */

	HUINT32 ulWatchingChannelId;	/* Watching Channel ID, stbIfChannelIdService */
	HUINT32 ulWatchingNetworkId;/* Watching Netwrok ID, stbIfChannelIdOriginalnet */
	
	DI_SNMP_StbPowerStatus_e etPowerStatus;	/*	stbInfoPowerStatus*/
	DI_SNMP_StbPlayStatus_e etPlayStatus;	/*	stbInfoPlayStatus*/
		
	HUINT32 ulTuner1ChannelId; 		/*stbInfoTuner1ServiceId*/
	HUINT32 ulTuner1NetworkId;		/*stbInfoTuner1NetworkId*/
	HUINT32 ulTuner1Frequency; 		/*stbInfoTuner1Frequency*/

	HUINT32 ulTuner2ChannelId; 		/*stbInfoTuner2ServiceId*/
	HUINT32 ulTuner2NetworkId;		/*stbInfoTuner2NetworkId*/
	HUINT32 ulTuner2Frequency; 		/*stbInfoTuner2Frequency*/
	
}DI_SNMP_MWInformation_t;

/**
 * Define SNMP Info which is set by DI
 */

typedef struct
{
	DI_SNMP_Event_e etSnmpEvent;

	HUINT32 ulDownChannelPower;	/* Down Channel Power, stbIfDownChannelPower */

	HUINT32	ulUnErroredNo;		/* Unerrored #, stbIfSigQUnerroreds */
	HUINT32 ulCorrectedNo;		/* Corected #, stbIfSigQCorrecteds */
	HUINT32 ulUnCorrectedNo; 	/* Uncorrected #, stbIfSigQUncorrecteds */
	HUINT32 ulInbandBer;	     /* InbandBer(Error rate * 10E-8) stbIfSigQBER */
		
}DI_SNMP_DIInformation_t;

/**
 * Define SNMP Info 
 */

typedef struct
{
	DI_SNMP_MWInformation_t stSnmpMWInfo;
	DI_SNMP_DIInformation_t stSnmpDIInfo;
		
}DI_SNMP_Information_t;


/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/

DI_ERR_CODE DI_SNMP_Init(void);
DI_ERR_CODE DI_SNMP_SetMWPrivateMIBInfo( DI_SNMP_MWInformation_t  *stSnmpMWInfo );



#endif /* __DI_SNMP_H__ */


