/*********************************************************************

DI Demux Extension module

Module : DI Demux Extension
File name : di_demux_ext.h

**********************************************************************/

#ifndef __DI_DEMUX_EXT_H__
#define __DI_DEMUX_EXT_H__

#include "di_err.h"

#define	DI_DEMUX_EXT_INVALID_PID			0x2000
#define	DI_DEMUX_EXT_INVALID_CHANNEL_ID		0xFF
#define DI_DEMUX_EXT_MANAGED_FILTER_SIZE	(64*1024)	/* was 32*1024 */

typedef enum
{
	DI_DEMUX_EXT_CAP_LIVE= 0x00000001, 						/**< demux capability : support live decoding  */
	DI_DEMUX_EXT_CAP_PLAYBACK = 0x00000004,					/**< demux capability : support playback  */
	DI_DEMUX_EXT_CAP_SECTIONFILTER = 0x00000008, 			/**< demux capability : support section filting */
	DI_DEMUX_EXT_CAP_DUPLICATION = 0x00000010,				/**< demux capability : support duplicate pid channel \n example send ES to decoder and hdd with one pid channel */
	DI_DEMUX_EXT_CAP_PESFILTER= 0x00000020, 				/**< demux capability : support PES filtering for Teletext or Subtitle etc */
	DI_DEMUX_EXT_CAP_DESCRAMBLE = 0x00000040,				/**< demux capability : support Descrambe with it's own descrambler  */
	DI_DEMUX_EXT_CAP_DESCRAMBLE_CI= 0x00000080,				/**< demux capability : support Descrambe with CI  */
	DI_DEMUX_EXT_CAP_CI_ROUTING	 = 0x00000100, 				/**< demux capability : support CI routing by using remux*/
	DI_DEMUX_EXT_CAP_NOTIFY_SCRAMBLE_TS = 0x00000200, 		/**< demux capability : support Scramble or clean TS notification  */
	DI_DEMUX_EXT_CAP_RECORD = 0x00000400,					/**< demux capability : support Recording */
	DI_DEMUX_EXT_CAP_EXTERNAL = 0x00000800					/** < demux capability : support external input or output or both. support only output in current */
}DI_DEMUX_EXT_CAP;

/**
@brief Demux's cababliity
*/
typedef struct DI_DEMUX_EXT_capability
{
	HUINT32 ulCapability;						/**< logical demux Capability \n example DI_DEMUX_EXT_CAP_PESFILTER | DI_DEMUX_EXT_CAP_LIVE */
	HUINT32 NumOfPESFilter;						/**< the number of PES filter */
	HUINT32 NumOfSectoinFilter;					/**< the number of Section filter */
	HUINT32 NumOfSectoinFilterPerPid;			/**< the number of Section filter per a PID */
	HUINT32 NumOfMaximumFilterLength;
} DI_DEMUX_EXT_CAPABILITY;

/**
@brief Demux's total cababliity
*/
typedef struct DI_DEMUX_EXT_totalcapability
{
	HUINT32 NumOfDemux;
	HUINT32 NumOfTotalSectoinFilter;			/**< the number of Section filter for all demux ids. */
} DI_DEMUX_EXT_TOTALCAPABILITY;

/**
@brief Demux's Input device types
*/
typedef enum DI_DEMUX_EXT_InputDeviceType
{
	DI_DEMUX_EXT_INPUT_NONE = 0,    			/**< Input device type : Not connected */
	DI_DEMUX_EXT_INPUT_CURRENT ,    			/**< Input device type : Use previsous setting*/
	DI_DEMUX_EXT_INPUT_TUNER,					/**< Input device type : tuner */
	DI_DEMUX_EXT_INPUT_RECORDER,				/**< Input device type : recorder: play file that is currently recording. */
	DI_DEMUX_EXT_INPUT_SCRAMBLED_FILE_RECORDER, /**< Input device type : play scrambled file that is currently recording. */
	DI_DEMUX_EXT_INPUT_PLAYER,					/**< Input device type : player: play file in storage media. */
	DI_DEMUX_EXT_INPUT_SCRAMBLED_FILE_PLAYER,	/**< Input device type : play scrambled file */
	DI_DEMUX_EXT_INPUT_MEDIA_PLAYER,			/**< Input device type : used to set demux path for media module.>*/
	DI_DEMUX_EXT_INPUT_EXTERNAL,				/**< Input device type : external input */
	DI_DEMUX_EXT_INPUT_MAX						/**< Input device type : wrong Input type*/
}DI_DEMUX_EXT_INPUTDEVICETYPE;

/**
@brief Demux's Output device types
*/
typedef enum DI_DEMUX_EXT_OutputDeviceType
{
	DI_DEMUX_EXT_OUTPUT_NONE = 0,				/**< Output device type : Not specefic - initial value */
	DI_DEMUX_EXT_OUTPUT_CURRENT,    			/**< Output device type : Use previsous setting*/
	DI_DEMUX_EXT_OUTPUT_RECORDER,				/**< Output device type : recorder */
	DI_DEMUX_EXT_OUTPUT_DECODER,				/**< Output device type : decoder */
	DI_DEMUX_EXT_OUTPUT_EXTERNAL,				/**< Output device type : output stream without modification.*/
	DI_DEMUX_EXT_OUTPUT_TRANSCODER,				/**< Output device type : Transcoder.*/
	DI_DEMUX_EXT_OUTPUT_MAX						/**< Output device type : Wrong Output type */
}DI_DEMUX_EXT_OUTPUTDEVICETYPE;

/**
@brief Pid channel type
*/
typedef enum DI_DEMUX_EXT_channelType
{
	DI_DEMUX_EXT_CHANNELTYPE_SECTION,
	DI_DEMUX_EXT_CHANNELTYPE_PES,
	DI_DEMUX_EXT_CHANNELTYPE_MAX
} DI_DEMUX_EXT_CHANNELTYPE;

/**
@brief PES Types
*/
typedef enum DI_DEMUX_EXT_PESTYPE
{
	DI_DEMUX_EXT_PESTYPE_SUBTITLE = 0,
	DI_DEMUX_EXT_PESTYPE_TELETEXT,
	DI_DEMUX_EXT_PESTYPE_USERDEFINE,
	DI_DEMUX_EXT_PESTYPE_MAX
} DI_DEMUX_EXT_PESTYPE;

typedef DI_ERR_CODE (*pfnDI_DEMUX_EXT_Notify)(HUINT32 ulDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId);
typedef DI_ERR_CODE	(*pfnDI_DEMUX_EXT_AllocBuffer)(HUINT32 usBufLen, HUINT8 **ppucBuf);


/********************************************************************************************/
/********************************************************************************************
						FOR MW LAYER FUNCTION
********************************************************************************************/
/********************************************************************************************/

DI_ERR_CODE DI_DEMUX_EXT_Init(void);
DI_ERR_CODE DI_DEMUX_EXT_GetCapability(HUINT32 ulDemuxID, DI_DEMUX_EXT_CAPABILITY * pstCapability);
DI_ERR_CODE DI_DEMUX_EXT_GetTotalCapability(DI_DEMUX_EXT_TOTALCAPABILITY * ptotalstCapability);
DI_ERR_CODE DI_DEMUX_EXT_RegisterSectionCallback (HUINT32 ulDemuxId, pfnDI_DEMUX_EXT_Notify pfnNotify, pfnDI_DEMUX_EXT_AllocBuffer pfnAllocBuffer);
DI_ERR_CODE DI_DEMUX_EXT_SetSectionFilter (HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 ulRequestID, HBOOL bIsOnce,
											HUINT32 ulFilterLen, HUINT8 * pucValue,HUINT8 * pucMask,HUINT8 * pucExclusion,
											HBOOL bCheckCRC);
DI_ERR_CODE DI_DEMUX_EXT_ReleaseSectionFilter (HUINT32 ulDemuxId, HUINT32	ulRequestID);

#endif

