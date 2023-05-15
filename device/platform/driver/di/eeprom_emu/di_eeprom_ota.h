#ifndef __DI_EEPROM_OTA_H__
#define __DI_EEPROM_OTA_H__

#define HUMAX_OUI			0x000378
#define DVB_OUI				0x00015A

#define PAT_PID				0x00
#define NIT_PID				0x10

#define PAT_TABLE_ID		0x00
#define PMT_TABLE_ID		0x02
#define SDT_TABLE_ID		0x42	// Actual
#define UNT_TABLE_ID		0x4B

#define DSMCC_UN_MSG_TABLE_ID		0x3B
#define DSMCC_DD_MSG_TABLE_ID		0x3C

typedef struct 
{
	HUINT32	ulFrequency;
	HUINT32	ulSymbolRate;
	HUINT8 	etFecCodeRate;
	HUINT8 	etPolarization;
	HUINT8 	etTransSpec;
	HUINT8 	etModulation;
	HUINT8 	etPilot;
	HUINT8 	etRollOff;
	HUINT8 	etLnbVoltage;
	HUINT8 	etDiseqcVersion;
	HUINT8 	etDiseqcInput;
	HUINT8 	etToneBurst;
	HUINT8 	b22kTone;
	HUINT8	etAntennaType;
	HUINT32	ulScdMasterUnitId;
	HUINT32	ulScdUserBandNum;
	HUINT32	ulScdUserBandFreq;
} SRV_SAT_CHANNEL_PARAM_t;


typedef struct  { //Warning: shuld be redefined before use!!!
	HUINT32	ulFrequency;
	HUINT8	dummy1[4];
 	HUINT8 etBandWidth;
	HUINT8 etModulation;
	HUINT8 etHierachy;
	HUINT8 etCodeRate;
	HUINT8 etTransMode;
	HUINT8 etGuardInterval;
	HUINT8 etStream;
	HUINT8 etOffset; 
	HUINT8 dummy2[16];
} SRV_TER_CHANNEL_PARAM_t;

typedef struct  {//Warning: shuld be redefined before use!!
	HUINT32	ulFrequency;
	HUINT32 ulSymbolRate;
	HUINT8 etModulation;
	HUINT8 etSpectrum;
	HUINT8	dummy1[22];
} SRV_CAB_CHANNEL_PARAM_t;


typedef union  {
	SRV_SAT_CHANNEL_PARAM_t Sat;
	SRV_CAB_CHANNEL_PARAM_t Cab;
	SRV_TER_CHANNEL_PARAM_t Ter;
} SRV_TUNER_INFO_t;

#define	srv_tuneInfo SRV_TUNER_INFO_t

typedef struct tagSysInfo 
{
	HUINT32			ulSystemId;
	HUINT16			usPanelId;
	HUINT8			ucDwnType;
	HUINT8   		ucChannelType;
	SRV_TUNER_INFO_t	sTuning;
	HUINT8			dummy1[4];
	HUINT16			usEsPid;
	HUINT8			dummy2[2];
	HUINT16			usLoaderVersion;
	HUINT8			ucHdfVersion;
	HUINT8			ucSwUpgrade;
	HUINT32			ulAppVersion;
	HUINT16			ulOtaLoaderVersion;
	HUINT8			dummy3[2];
	HUINT32			ulStringVersion;
}stOtaSysInfo_t;


typedef struct _OTA_INFO {
	HUINT32	swVersion;
	HUINT32	systemId;	
	HBOOL	otaDetected;

	HUINT32 ulGroupID;
	HUINT32 ulGroupSize;	/* application's size */

	HUINT32 downloadID;
	HUINT16 moduleId;
	HUINT32 moduleSize;		/* application's size */
	HUINT16 blockSize;		/* ddb block size */
	HUINT32 NumOfblocks;	/* ddb block number */

	HUINT8 *pModule;          /* Module buffer */
	HUINT8 cached;

	HUINT32 NumOfDownloadedBlock;  /* downloaded ddb block number */
	HUINT8 *pucDownloadedBlock;  /* memory for check downloaded block HBOOL HBOOL HBOOL HBOOL */
} OTAInfo_t;


#endif /* __DI_EEPROM_OTA_H__ */
