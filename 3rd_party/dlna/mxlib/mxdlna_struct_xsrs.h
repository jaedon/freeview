


#ifndef MXDLNA_STRUCT_XSRS_H__
#define MXDLNA_STRUCT_XSRS_H__

// ---------------------------------------
#include "mxlib_config.h"
// ---------------------------------------

#include "mxapi_define.h"


#ifdef __cplusplus
extern "C" 
{
#endif

	
#include "mxtypes.h"
#include "mxdlna_define.h"


	typedef unsigned int XSRS_BOOL;


	// ##############################################################################
	// HDRL 
	// ##############################################################################
	// Refers to AVTransport::RecordStorageMedium state variable for allowed values
	typedef enum _MXDLNA_RECORDDEST_ALLOWED_TYPE
	{
		MXDLNA_RECORDDEST_NONE		= 1,		// "NONE"		O  No medium present
		MXDLNA_RECORDDEST_UNKNOWN	= 2,		// "UNKNOWN"	O  Unknown medium 
		MXDLNA_RECORDDEST_HDD		= 4,		// "HDD"		O  Hard Disk Drive medium
		MXDLNA_RECORDDEST_NETWORK	= 8,		// "NETWORK"	O  Network Interface medium
		MXDLNA_RECORDDEST_SD		= 16,		// "SD"			O  SD (Secure Digital) Memory Card medium
		MXDLNA_RECORDDEST_PC_CARD	= 32,		// "PC-CARD"	O  PC Card medium
		MXDLNA_RECORDDEST_MMC		= 64,		// "MMC"		O  MultimediaCard medium
		MXDLNA_RECORDDEST_CF		= 128,		// "CF"			O  Compact Flash medium
	}MXDLNA_RECORDDEST_ALLOWED_TYPE;



	typedef struct _MXDLNA_RECORDDEST_INFO
	{
		// GetRecordDestinations
		// -------------------------------------------
		char * destName;	// For X_HDLnkGetRecordDestination.	It MUST be set unique value.	
		char * ID;			// For X_HDLnkGetRecordDestination.	It MUST be set unique value.
		// -------------------------------------------
		

		// GetRecordDestinationInfo
		// -------------------------------------------
		MX_UINT64 totalCapacity_bytes;			// For X_HDLnkGetRecordDestinationInfo.
		MX_UINT64 availableCapacity_bytes;		// For X_HDLnkGetRecordDestinationInfo.
		int recordable;							// For X_HDLnkGetRecordDestinationInfo.
		int bDtcpSupport;						// For X_HDLnkGetRecordDestinationInfo.
		MXDLNA_RECORDDEST_ALLOWED_TYPE	allowedTypes;	// For X_HDLnkGetRecordDestinationInfo.
		// -------------------------------------------

		struct _MXDLNA_RECORDDEST_INFO * pNext;
		//void* userValue;	
	}MXDLNA_RECORDDEST_INFO;
	// ##############################################################################
	// HDRL
	// ##############################################################################








	
	// ##############################################################################
	// XSRS
	// ##############################################################################

	// ---------------------------
	enum MXDLNAXSRS_DESIREDQUALITYMODE					//enum MXDLNAXSRS_DESIREDQUALITYMODE
	// ---------------------------
	{
		MXDLNAXSRS_DESIREDQUALITYMODE_XP	= 1,		//  1 : XP	(Xcellent Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_XSP	= 2,		//  2 : XSP	(Xcellent Standard Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_SP	= 3,		//  3 : SP	(Standard Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_LSP	= 4,		//  4 : LSP	(Long Standard Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_ESP	= 5,		//  5 : ESP	(Extended Standard Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_LP	= 6,		//  6 : LP	(Long Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_EP	= 7,		//  7 : EP	(Extended Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_SLP	= 8,		//  8 : SLP	(Super Long Play)
		MXDLNAXSRS_DESIREDQUALITYMODE_DR	= 100,		//100 : DR	(Digital broadcasting's Direct recording)
		MXDLNAXSRS_DESIREDQUALITYMODE_XR	= 101,		//101 : XR	(Xcellent Rec)
		MXDLNAXSRS_DESIREDQUALITYMODE_SR	= 102,		//102 : SR	(Standard Rec)
		MXDLNAXSRS_DESIREDQUALITYMODE_LR	= 103,		//103 : LR	(Long Rec)
		MXDLNAXSRS_DESIREDQUALITYMODE_ER	= 104,		//104 : ER	(Extended Rec)
	};

	// ---------------------------
	enum MXDLNAXSRS_GENRETYPE
	// ---------------------------
	{
		MXDLNAXSRS_GENRETYPE_NOT_USE			=0,
		MXDLNAXSRS_GENRETYPE_GROUND_DIGITAL_BS	=2,		// Ground Digital BS genre
		MXDLNAXSRS_GENRETYPE_BS_CS110			=3,		// BS, CS 110 Digital BS genre
		MXDLNAXSRS_GENRETYPE_CS110				=4,		// CS 110 Digital BS genre
		MXDLNAXSRS_GENRETYPE_SKAPA				=5,		// Sky perfact TV Broadcast genre
	};


	// -----------------------------------------------
	typedef struct mxdlna_xsrsObject
	// -----------------------------------------------
	{
		char * id;
		char * title;
		int titleLength;

		/*	1) YYYY-MM-DDTHH:MM:SS
			2) YYYY-MM-DDTHH:MM:SSTZD		ex) 2007-04-01T13:45:30+09:00
			2) YYYY-MM-DDTHH:MM:SS.sTZD		ex) 2007-04-01T13:45:30.85+09:00		*/
		char * scheduledStartDateTime;

		unsigned long scheduledDuration;	// 0~359999999 unit is sec.
		
		char * scheduledChannelID;							// 4 digit hex value. (ex. 0x0420 - case insensitive)
		unsigned long scheduledChannelID_channelType;		// 999: receive channel or external input or record device internal content
		unsigned long scheduledChannelID_broadcastingType;	// 200: Network input, 0: Not use, 1~199,201~
		
		char * desiredMatchingID;
		char * desiredMatchingID_type;						// Allowed Value: "SI_PROGRAMID"
		
		enum MXDLNAXSRS_DESIREDQUALITYMODE desiredQualityMode;	// Allowed Value: 100: DR(Digital broadcasting's Direct recording), Reserved: 0~99, 101~

		char * genreID;		
		enum MXDLNAXSRS_GENRETYPE genreID_type;

		XSRS_BOOL priorityFlag;								// 0 : if conflict , error return. 1 : if conflict , just insert recordschedule list
		char * reservationCreatorID;						// 2000: user rec, 2220: user rec reservation/remote reservation/reservation from the home network 
		char * recordDestinationID;							// 20120417 defined by JEITA and Using IO DATA NAS

		// --------------------------------------
		int scheduledConditionID;							// range is 0~999 number.
		unsigned long scheduledChannelID_branchNum;			// use condition: if( xsrs:scheduledChannelID@channelType == 999(receive channel) )
		char * conflictID;									// 0~3
		char * mediaRemainAlertID;							// 0: enough
		XSRS_BOOL recordingFlag;							// 0: Not Recording, 1: Recording

		struct mxdlna_xsrsObject *pNext;
	}MXDLNA_XSRSOBJECT;



	// API

__MXAPI	const char * mxDlnaXSrs_GetRecordDestAllowedTypeAsString(MXDLNA_RECORDDEST_ALLOWED_TYPE allowedType);

__MXAPI MXDLNA_RECORDDEST_ALLOWED_TYPE  mxDlnaXSrs_GetRecordDestAllowedTypeAsInteger(char * allowdType);
	// ##############################################################################
	// XSRS
	// ##############################################################################
	



	
#ifdef __cplusplus
}
#endif


#endif // MXDLNA_STRUCT_XSRS_H__

