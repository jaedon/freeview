
#ifndef __SILIB_TABLE_H__
#define __SILIB_TABLE_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_table.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define SILIB_TABLE_EXT_ID_ALL				((HUINT32)0xFFFFFFFF)
#define SILIB_TABLE_EXT_ID_NOT				((HUINT32)0xFFFFFFFE)

#define SILIB_TABLE_DESTAG_ALL				((HUINT32)0xFFFFFFFF)

/*******************************************************************/
/********************      Enumerations         *************************/
/*******************************************************************/
// Table Type
typedef enum
{
	eSIxTABLETYPE_Pat,
	eSIxTABLETYPE_Pmt,
	eSIxTABLETYPE_NitAct,
	eSIxTABLETYPE_NitOth,
	eSIxTABLETYPE_Bat,
	eSIxTABLETYPE_SdtAct,
	eSIxTABLETYPE_SdtOth,
	eSIxTABLETYPE_EitActPf,
	eSIxTABLETYPE_EitOthPf,
	eSIxTABLETYPE_EitActSc,
	eSIxTABLETYPE_EitOthSc,
	eSIxTABLETYPE_Tdt,
	eSIxTABLETYPE_Tot,
	eSIxTABLETYPE_Cat,
	eSIxTABLETYPE_Unt,
	eSIxTABLETYPE_Dsi,
	eSIxTABLETYPE_Ait,
	eSIxTABLETYPE_Ict,
	eSIxTABLETYPE_Sgt,
	eSIxTABLETYPE_Dit,
	eSIxTABLETYPE_Sit,
	eSIxTABLETYPE_Rct,

	// ....
	eSIxTABLETYPE_MAX,

	eSIxTABLETYPE_ALL,			// Delete 등을 원활하게 하기 위한

	eSIxTABLETYPE_EndOfCase
} SIxTable_Type_e;

// Table ID (8 bits value)
enum
{
	eSIxTABLEID_PAT						= 0x00,
	eSIxTABLEID_CAT						= 0x01,
	eSIxTABLEID_PMT						= 0x02,
	eSIxTABLEID_TSDT						= 0x03,
	eSIxTABLEID_NIT_ACT					= 0x40,
	eSIxTABLEID_NIT_OTH					= 0x41,
	eSIxTABLEID_SDT_ACT					= 0x42,
	eSIxTABLEID_SDT_OTH					= 0x46,
	eSIxTABLEID_EIT_ACT_PF				= 0x4e,
	eSIxTABLEID_EIT_OTH_PF				= 0x4f,
	eSIxTABLEID_EIT_ACT_SC_0				= 0x50,
	eSIxTABLEID_EIT_ACT_SC_1				= 0x51,
	eSIxTABLEID_EIT_ACT_SC_2				= 0x52,
	eSIxTABLEID_EIT_ACT_SC_3				= 0x53,
	eSIxTABLEID_EIT_ACT_SC_4				= 0x54,
	eSIxTABLEID_EIT_ACT_SC_5				= 0x55,
	eSIxTABLEID_EIT_ACT_SC_6				= 0x56,
	eSIxTABLEID_EIT_ACT_SC_7				= 0x57,
	eSIxTABLEID_EIT_ACT_SC_8				= 0x58,
	eSIxTABLEID_EIT_ACT_SC_9				= 0x59,
	eSIxTABLEID_EIT_ACT_SC_10			= 0x5a,
	eSIxTABLEID_EIT_ACT_SC_11			= 0x5b,
	eSIxTABLEID_EIT_ACT_SC_12			= 0x5c,
	eSIxTABLEID_EIT_ACT_SC_13			= 0x5d,
	eSIxTABLEID_EIT_ACT_SC_14			= 0x5e,
	eSIxTABLEID_EIT_ACT_SC_15			= 0x5f,
	eSIxTABLEID_EIT_OTH_SC_0				= 0x60,
	eSIxTABLEID_EIT_OTH_SC_1				= 0x61,
	eSIxTABLEID_EIT_OTH_SC_2				= 0x62,
	eSIxTABLEID_EIT_OTH_SC_3				= 0x63,
	eSIxTABLEID_EIT_OTH_SC_4				= 0x64,
	eSIxTABLEID_EIT_OTH_SC_5				= 0x65,
	eSIxTABLEID_EIT_OTH_SC_6				= 0x66,
	eSIxTABLEID_EIT_OTH_SC_7				= 0x67,
	eSIxTABLEID_EIT_OTH_SC_8				= 0x68,
	eSIxTABLEID_EIT_OTH_SC_9				= 0x69,
	eSIxTABLEID_EIT_OTH_SC_10			= 0x6a,
	eSIxTABLEID_EIT_OTH_SC_11			= 0x6b,
	eSIxTABLEID_EIT_OTH_SC_12			= 0x6c,
	eSIxTABLEID_EIT_OTH_SC_13			= 0x6d,
	eSIxTABLEID_EIT_OTH_SC_14			= 0x6e,
	eSIxTABLEID_EIT_OTH_SC_15			= 0x6f,

	eSIxTABLEID_BAT						= 0x4a,
	eSIxTABLEID_RST						= 0x71,
	eSIxTABLEID_TDT						= 0x70,
	eSIxTABLEID_TOT						= 0x73,
	eSIxTABLEID_AIT						= 0x74,

	eSIxTABLEID_DIT_ARIB					= 0x7E,		/* Already defined DIT_TABLE_ID */
	eSIxTABLEID_SIT						= 0x7F,

	eSIxTABLEID_DIT						= 0xc5,
	eSIxTABLEID_DIT_EXT					= 0xc5c5,
	eSIxTABLEID_DGT						= 0xc6,
	eSIxTABLEID_DBT						= 0xc7,

	eSIxTABLEID_DSI						= 0x3b,		/* dsm-cc data carousel */
	eSIxTABLEID_DII						= 0x3b,		/* dsm-cc data carousel */
	eSIxTABLEID_DDB						= 0x3c,		/* dsm-cc data carousel */
	eSIxTABLEID_DESC						= 0x3d,		/* dsm-cc data carousel stream description*/
	eSIxTABLEID_UNT						= 0x4b,		/* dvb-ssu enhanced profile */
	eSIxTABLEID_RCT						= 0x76,		/* related content table TS 102-323 & 300-468 */
	eSIxTABLEID_CIT						= 0xa0,
	eSIxTABLEID_CPT						= 0xa1,
	eSIxTABLEID_SGT						= 0x91,		/* from SUPPORT_ASTRA_LCN */
	eSIxTABLEID_RPD						= 0xe0,		/* from TNTSat - Regionalization Pool desiptor Section Table */
	eSIxTABLEID_DSC						= 0xf0,
	eSIxTABLEID_ICT						= 0xfe,

	eSIxTABLEID_EIT_ACT_SC_BASIC_0		= eSIxTABLEID_EIT_ACT_SC_0,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_BASIC_1		= eSIxTABLEID_EIT_ACT_SC_1,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_BASIC_2		= eSIxTABLEID_EIT_ACT_SC_1,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_BASIC_3		= eSIxTABLEID_EIT_ACT_SC_1,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_BASIC_4		= eSIxTABLEID_EIT_ACT_SC_1,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_BASIC_5		= eSIxTABLEID_EIT_ACT_SC_1,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_BASIC_6		= eSIxTABLEID_EIT_ACT_SC_6,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_BASIC_7		= eSIxTABLEID_EIT_ACT_SC_7,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_8	= eSIxTABLEID_EIT_ACT_SC_8,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_9	= eSIxTABLEID_EIT_ACT_SC_9,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_10	= eSIxTABLEID_EIT_ACT_SC_10,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_11	= eSIxTABLEID_EIT_ACT_SC_11,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_12	= eSIxTABLEID_EIT_ACT_SC_12,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_13	= eSIxTABLEID_EIT_ACT_SC_13,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_14	= eSIxTABLEID_EIT_ACT_SC_14,	/* Japan, ARIB */
	eSIxTABLEID_EIT_ACT_SC_EXTENDED_15	= eSIxTABLEID_EIT_ACT_SC_15,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_0			= eSIxTABLEID_EIT_OTH_SC_0,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_1			= eSIxTABLEID_EIT_OTH_SC_1,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_2			= eSIxTABLEID_EIT_OTH_SC_2,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_3			= eSIxTABLEID_EIT_OTH_SC_3,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_4			= eSIxTABLEID_EIT_OTH_SC_4,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_5			= eSIxTABLEID_EIT_OTH_SC_5,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_6			= eSIxTABLEID_EIT_OTH_SC_6,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_BASIC_7			= eSIxTABLEID_EIT_OTH_SC_7,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_8		= eSIxTABLEID_EIT_OTH_SC_8,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_9		= eSIxTABLEID_EIT_OTH_SC_9,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_10		= eSIxTABLEID_EIT_OTH_SC_10,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_11		= eSIxTABLEID_EIT_OTH_SC_11,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_12		= eSIxTABLEID_EIT_OTH_SC_12,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_13		= eSIxTABLEID_EIT_OTH_SC_13,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_14		= eSIxTABLEID_EIT_OTH_SC_14,	/* Japan, ARIB */
	eSIxTABLEID_EIT_OTH_EXTENDED_15		= eSIxTABLEID_EIT_OTH_SC_15,	/* Japan, ARIB */
	eSIxTABLEID_SDTT						= 0xc3,						/* Japan, ARIB */
	eSIxTABLEID_BIT						= 0xc4,						/* Japan, ARIB */
	eSIxTABLEID_CDT						= 0xc8,						/* Japan, ARIB */

	eSIxTABLEID_FSAT_EITPF_PP_OTH		= 0xd1,						/* Freesat Tunneld Eit pf++ Oth */

	eSIxTABLEID_ETC						= 0xf1
};


/*******************************************************************/
/********************      Data Type         *************************/
/*******************************************************************/
// -- General PSI Section -- //
// PSI Table Section 구조체들은 반드시 SIxTable_GenSec_t 와 겹치는 형태를 유지해야만 한다.
typedef struct
{
	SIxTable_Type_e	 eTableType;			// Table Type
	HUINT16			 usExtId;				// EXTID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32
} SIxTable_GenSec_t;

// - PAT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Pat

	HUINT16			 usProgNum;
	HUINT16			 usProgMapPid;
} SIxTable_PatProgram_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Pat
	HUINT16			 usTsId;				// EXTID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32
} SIxTable_PatSec_t;

// - CAT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Cat
	HUINT16			 usExtId;				// EXTID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32
} SIxTable_CatSec_t;

// - PMT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Pmt

	HUINT8			 ucStreamType;
	HUINT16			 usElemPid;
} SIxTable_PmtElement_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Pmt
	HUINT16			 usProgNum;				// ExtID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32

	HBOOL			 bCurNextInd;
	HUINT16			 usPcrPid;
} SIxTable_PmtSec_t;

// - NIT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_NitAct, eSIxTABLETYPE_NitOth

	HUINT16			 usTsId;
	HUINT16			 usOrgNetId;
} SIxTable_NitTs_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_NitAct, eSIxTABLETYPE_NitOth
	HUINT16			 usNetId;				// ExtID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32
} SIxTable_NitSec_t;

// - BAT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Bat

	HUINT16			 usTsId;
	HUINT16			 usOrgNetId;
} SIxTable_BatTs_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Bat
	HUINT16			 usBqId;				// ExtID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32
} SIxTable_BatSec_t;

// - SDT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_SdtAct, eSIxTABLETYPE_SdtOth

	HUINT16			 usSvcId;
	HBOOL			 bEitSched;
	HBOOL			 bEitPreFollow;
	HUINT8			 ucRunningStatus;
	HBOOL			 bCaFree;
} SIxTable_SdtService_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_SdtAct, eSIxTABLETYPE_SdtOth
	HUINT16			 usTsId;				// ExtID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32

	HUINT16 		 usOrgNetId;
} SIxTable_SdtSec_t;

// - EIT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_EitActPf, eSIxTABLETYPE_EitOthPf, eSIxTABLETYPE_EitActSc, eSIxTABLETYPE_EitOthSc

	HUINT16			 usEvtId;
	UNIXTIME		 utStartTime;
	UNIXTIME		 utDuration;
	HUINT8			 ucRunStatus;
	HBOOL			 bCaFree;
} SIxTable_EitEvent_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_EitActPf, eSIxTABLETYPE_EitOthPf, eSIxTABLETYPE_EitActSc, eSIxTABLETYPE_EitOthSc
	HUINT16			 usSvcId;				// ExtID:
	HUINT8			 ucVer;					// Section Version:
	HUINT32			 ulCrc32;				// CRC 32

	HUINT16 		 usTsId;
	HUINT16			 usOrgNetId;
} SIxTable_EitSec_t;

// - TDT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Tdt
	HUINT16			 usExtId;				// ExtID: N/A
	HUINT8			 ucVer;					// Section Version: N/A
	HUINT32			 ulCrc32;				// CRC 32: N/A

	UNIXTIME		 utUtcTime;
} SIxTable_TdtSec_t;

// - TOT -//
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Tot
	HUINT16			 usExtId;				// ExtID: N/A
	HUINT8			 ucVer;					// Section Version: N/A
	HUINT32			 ulCrc32;				// CRC 32

	UNIXTIME		 utUtcTime;
} SIxTable_TotSec_t;

// - DSI - //
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_DSI

	HUINT32			 ulGroupId;
	HUINT32			 ulGroupSize;
	HxList_t		*pstGroupCompList;		// HUINT8 *, GroupCompatibility는 Descriptor 형태로 간다.

	// Group Info Byte는 2ndLoop Desc로 간다.
} SIxTable_DsiGroup_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_DSI
	HUINT16			 usExtId;				// ExtID: N/A
	HUINT8			 ucVer;					// Section Version: N/A
	HUINT32			 ulCrc32;				// CRC 32

	// Server ID
	HUINT8			 aucServerId[20];
} SIxTable_DsiSec_t;

// - UNT - //
#if 0
// 현재는 comp desc parsing을 si lib에서 하지 않아서 막아둠
typedef struct
{
	HUINT8			 ucDescType;
	HUINT8			 ucSpecifierType;
	HUINT32			 ulSpecifierData;
	HUINT16			 usModel;
	HUINT16			 usVersion;
	HxList_t		*pstSubDesc;	// sub descriptor
} SIxTable_UntCompDes_t;			// UNT compatibility desc.
#endif

/**
  *	UNT table 구조를 현재 구현된 si table 구조에 큰 수정없이 추가하기 위해
  *	1st desc. loop에는 common desc.를 넣고
  *	2nd loop > unInfo쪽에 compatiblity desc. loop + taget desc. loop + operational desc. loop를 넣고,
  *	2nd loop > desc. loop엔 아무것도 넣지 않는다...!!
**/
typedef struct
{
	SIxTable_Type_e	 	 eTableType;			// eSIxTABLETYPE_Unt
	HxList_t			*pstCompDesLoop;		// HUINT8*, compatibility raw descriptor loop
	HxList_t			*pstTargetDesLoop;		// NULL (TBD)
	HxList_t			*pstOperationalDesLoop;	// HUINT8 *, operational raw descriptor loop
} SIxTable_UntInfo_t;		// UNT compatibility desc.

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Unt
	HUINT16			 usExtId;				// ExtID:
	HUINT8			 ucVer;					// Section Version: N/A
	HUINT32			 ulCrc32;				// CRC 32

	HUINT8			 ucActionType;
	HUINT8			 ucOuiHash;
	HUINT32			 ulOui;
	HUINT8			 ucProcessingOrder;
} SIxTable_UntSec_t;

// - RCT - //
/**
  *	ETSI TS 102 323
  *
  *	RCT table 구조를 현재 구현된 si table 구조에 큰 수정없이 추가하기 위해
  *	1st desc. loop에는 원래 RCT table의 2nd desc. loop를 넣고
  *	2nd loop엔  원래 RCT table의 1st desc.loop에 해당하는 Link info + Link info desc. loop를 넣는다 !!
  *   즉, table parsing을 하면서 1st loop와 2nd loop를 바꿔서 넣는 셈이다.
**/
typedef struct
{
	SIxTable_Type_e 	 eTableType;			// eSIxTABLETYPE_Rct

	HUINT8				 ucIdType;				// identifier type
	HUINT8				 ucSchTimeReliability;	// scheduled time reliability
	HUINT8				 ucInlineService;
	HUINT16				 usStartDate;

	HUINT16				 usDvbSvcTripletId;		// dvb service triplet id, (ucInlineService == 0)
	HUINT16				 usTsId;				// (ucInlineService != 0)
	HUINT16				 usOnId;				// (ucInlineService != 0)
	HUINT16				 usSvcId;				// (ucInlineService != 0)

	HUINT16				 usStartTime;
	HUINT16				 usDuration;

	HUINT16				 usEventId;				// (ucIdType == 1)
	HUINT16				 usTvaId;				// (ucIdType == 2) or (ucIdType == 3)
	HUINT8				 ucComponent;			// (ucIdType == 3)

	HUINT8				 ucEarlyStartWindow;	// (ucIdType == 0) && (ucSchTimeReliability == 1)
	HUINT8				 ucLateEndWindow;		// (ucIdType == 0) && (ucSchTimeReliability == 1)
}SIxTable_RctDvbBinLocator_t;		// dvb binary locator

typedef struct
{
	SIxTable_Type_e 	 eTableType;			// eSIxTABLETYPE_Rct

	HUINT8				szIso639LangCode[4];
	HUINT8				ucTxtLen;
	HUINT8				*pszPromoText;
}SIxTable_RctPromoText_t;					// promotional text

typedef struct
{
	SIxTable_Type_e	 	 eTableType;			// eSIxTABLETYPE_Rct

	HUINT8				 ucLinkType;
	HUINT8				 ucHowSchemeId;			// how related classification scheme id
	HUINT16				 usTermId;
	HUINT8				 ucGroupId;
	HUINT8				 ucPrecedence;

	HUINT8				 ucMediaUriLen;					// (link type == 0x00) || (link type == 0x02)
	HUINT8				*pucMediaUriByte;				// (link type == 0x00) || (link type == 0x02)

	SIxTable_RctDvbBinLocator_t		*pstDvbBinLocator;	// dvb binary locator, (link type == 0x01) || (link type == 0x02)

	HxList_t			*pstPromoTextLoop;				// SIxTable_RctPromoText_t *, promotional text

	HUINT8				 ucDefaultIconFlag;
	HUINT8				 ucIconId;
} SIxTable_RctLinkInfo_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Rct
	HUINT16			 usSvcId;				// ExtID:
	HUINT8			 ucVer;					// Section Version: N/A
	HUINT32			 ulCrc32;				// CRC 32

	HUINT8			 ucTableIdExtFlag;		// table id extension flag
	HUINT16			 usYearOffset;
	HUINT8			ucLinkCount;			// link info count
} SIxTable_RctSec_t;

// - AIT - //
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Ait

	HUINT32			 ulOrganizaionId;		// application identifier
	HUINT16			 ulApplicationId;		// application identifier
	HUINT8			 ucApplControlCode;		// 1 = auto start
} SIxTable_AitApplication_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Ait
	HUINT16			 usExtId;				// ExtID: ucTestAppFlag + usAppicationType
	HUINT8			 ucVer;					// Section Version: N/A
	HUINT32			 ulCrc32;				// CRC 32

	HUINT16			 usAppicationType;		// ExtID:
	HUINT8			 ucTestAppFlag;
} SIxTable_AitSec_t;

// - ICT - //
typedef struct
{
	SIxTable_Type_e	 	eTableType;				// eSIxTABLETYPE_Ict
	HUINT16				usExtId;				// ExtID:
	HUINT8			 	ucVer;					// Section Version: N/A
	HUINT32			 	ulCrc32;				// CRC 32

	HUINT8				ucManufacturerCode;
	HUINT8				ucHardwareCode;
	HUINT8				aucSeqNumber[255];
	HUINT8				aucTableId[255];
	HUINT16 			ausDownloadTime[255];
} SIxTable_IctSec_t;

// - SGT - //
typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Sgt

	HUINT16			 usServiceId;
	HUINT16			 usTsId;
	HUINT16			 usOnId;
	HUINT16			 usLcn;
	HUINT8			 ucVisibleSvcFlag;
	HUINT8			 ucNewSvcFlag;
	HUINT16			 usGenreCode;
} SIxTable_SgtService_t;

typedef struct
{
	SIxTable_Type_e	 eTableType;			// eSIxTABLETYPE_Sgt
	HUINT16			 usServiceListId;		// ExtID:
	HUINT8			 ucVer;					// Section Version: N/A
	HUINT32			 ulCrc32;				// CRC 32
} SIxTable_SgtSec_t;


// ....

// - General PSI Table - //
typedef union
{
	SIxTable_Type_e			 eTableType;
	SIxTable_GenSec_t		 stTemplate;

	SIxTable_PatSec_t		 stPat;
	SIxTable_PmtSec_t		 stPmt;
	SIxTable_NitSec_t		 stNit;
	SIxTable_BatSec_t		 stBat;
	SIxTable_SdtSec_t		 stSdt;
	SIxTable_EitSec_t		 stEit;
	SIxTable_TdtSec_t		 stTdt;
	SIxTable_TotSec_t		 stTot;
	SIxTable_DsiSec_t		 stDsi;
	SIxTable_UntSec_t		 stUnt;
	SIxTable_RctSec_t		 stRct;
	SIxTable_AitSec_t		 stAit;
	SIxTable_IctSec_t		 stIct;
	SIxTable_SgtSec_t		 stSgt;
	SIxTable_CatSec_t		 stCat;

	// .... : 추가되어야 한다 ...
} SIxTable_Info_t;

typedef union
{
	SIxTable_Type_e				 eTableType;
	SIxTable_PatProgram_t		 stPat;
	SIxTable_PmtElement_t		 stPmt;
	SIxTable_NitTs_t			 stNit;
	SIxTable_BatTs_t			 stBat;
	SIxTable_SdtService_t		 stSdt;
	SIxTable_EitEvent_t			 stEit;
	SIxTable_DsiGroup_t			 stDsi;
	SIxTable_UntInfo_t			 stUnt;
	SIxTable_RctLinkInfo_t		 stRct;
	SIxTable_AitApplication_t	 stAit;
	SIxTable_SgtService_t	 	 stSgt;

	// .... : 추가되어야 한다 ...
} SIxTable_SubInfo_t;

typedef struct
{
	SIxTable_SubInfo_t		 unInfo;				// 2nd Loop에 들어가는 각 SI Table 내의 SubItem 의 union

	// 2nd Loop Descriptors
	HxList_t				*pstDesLoop;			// Raw Descriptor: HUINT8 (Do not de-allocate!)
} SIxTable_2ndLoop_t;


typedef struct _silib_table_pstTable_t
{
	SIxTable_Type_e			 eTableType;
	HBOOL					 bRawDup;

	// Table 관련 Table Info:
	SIxTable_Info_t			 unInfo;

	// 1st Loop Descriptor
	HxList_t				*pstDesLoop;			// Raw Descriptor: HUINT8 (Do not de-allocate!)

	// 2nd Sub-item Loop
	HxList_t				*pst2ndLoop;			// SIxTable_2ndLoop_t

	struct _silib_table_pstTable_t	*pstNextTable;
} SIxTable_PsiTable_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HUINT8				SILIB_TABLE_GetTableIdByTableType (SIxTable_Type_e eTable);
extern SIxTable_Type_e		SILIB_TABLE_GetTableTypeByTableId (HUINT8 ucTableId);
extern SIxTable_PsiTable_t*	SILIB_TABLE_ParseRawSection (HUINT8 *pucRawSec, SIxTextEncoding_e eTextEnc, HBOOL bRawDup);
extern SIxTable_PsiTable_t*	SILIB_TABLE_Duplicate (SIxTable_PsiTable_t *pstSrcTable);
extern void					SILIB_TABLE_Free (SIxTable_PsiTable_t *pstTable);
extern HUINT8*				SILIB_TABLE_Get1stLoopDescriptor (SIxTable_PsiTable_t *pstTable, HUINT32 ulDesTag, HUINT32 ulIndex);
extern HUINT8*				SILIB_TABLE_Get2ndLoopDescriptor (SIxTable_2ndLoop_t *pst2ndLoop, HUINT32 ulDesTag, HUINT32 ulIndex);

#ifdef __cplusplus
}
#endif

#endif // __SILIB_TABLE_H__


