/*
 * (c) 2011-2012 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/*
	Viaccess Demux API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.7
*/

#ifndef VA_DMX_API_H
#define VA_DMX_API_H

#include "va_dmx.h"
#include "va_setup.h"

#define VA_DMX_MAX_FILTER				kVA_SETUP_NBMAX_SECTIONFILTERS		// 16
#define VA_DMX_FILTER_LENGTH			kVA_DMX_MAX_FILTER_SIZE			// 10
#define VA_DMX_MAX_BUFFER				kVA_SETUP_NBMAX_SECTIONBUFFERS	// 64

#define VA_DMX_ILLEGAL_FILTER			(0xffff)
#define VA_DMX_ILLEGAL_PID				(0xffff)
#define VA_DMX_FILTER_LOADID_INVALID	(0xffff)

#define VA_ECM_0		0x80
#define VA_ECM_1		0x81
#define VA_EMMU			0x88
#define VA_EMMGA_0		0x8a
#define VA_EMMGA_1		0x8b
#define VA_EMMGH_0		0x8c
#define VA_EMMGH_1		0x8d
#define VA_EMMS			0x8e

typedef enum
{
	VA_DMX_FILTER_FREE = 0,
	VA_DMX_FILTER_ALLOCATED,
	VA_DMX_FILTER_RUNNING,
	VA_DMX_FILTER_STOPPED
} VA_DMX_FILTER_STATUS_ENUM;

typedef enum
{
	VA_DMX_FILTER_MODE_UNKNOWN = 0,
	VA_DMX_FILTER_MODE_ECM_0,
	VA_DMX_FILTER_MODE_ECM_1,
	VA_DMX_FILTER_MODE_EMMU,
	VA_DMX_FILTER_MODE_EMMGA_0,
	VA_DMX_FILTER_MODE_EMMGA_1,
	VA_DMX_FILTER_MODE_EMMGH_0,
	VA_DMX_FILTER_MODE_EMMGH_1,
	VA_DMX_FILTER_MODE_EMMS,
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	VA_DMX_FILTER_MODE_PVR,
#endif
	VA_DMX_FILTER_MODE_INVALID
} VA_DMX_FILTER_MODE_ENUM;

typedef struct {
	/*--- set by VA_DMX_AllocateSectionFilter() ---*/
	HUINT32 ulAcsId;
	HUINT32 ulVaFilterHandle;
	HUINT16 usNumBuffer;
	HUINT16 usPid;
	/*--- VA_DMX_FILTER_FREE by VA_DMX_Init() and VA_DMX_FreeSectionFilter(),
	           VA_DMX_FILTER_ALLOCATED by VA_DMX_AllocateSectionFilter(),
	           VA_DMX_FILTER_RUNNING by VA_DMX_StartSectionFilter(),
	           VA_DMX_FILTER_STOPPED by VA_DMX_StopSectionFilter() ---*/
	VA_DMX_FILTER_STATUS_ENUM eStatus;
	/*--- set by VA_DMX_SetSectionFilterParameters() ---*/
	VA_DMX_FILTER_MODE_ENUM eMode;
	HUINT8 ucFilterLength;
	HUINT8 aucValue[VA_DMX_FILTER_LENGTH];
	HUINT8 aucMask[VA_DMX_FILTER_LENGTH];
	/*--- set by VA_DMX_RetrieveEcmSection() or VA_DMX_RetrieveEmmSection() through VA_DMX_SetSectionFilterParameters() or VA_DMX_StartSectionFilter() ---*/
	HUINT16 usLoadId;
	/*--- VA_DMX_BUFFER_UNLOCKED by VA_DMX_UnlockSectionFilter(),
	           VA_DMX_BUFFER_LOCKED by VA_DMX_UnlockSectionFilter(), VA_DMX_PutEcmToCa() or VA_DMX_PutEmmToCa() after VA_DMX_NotifyFilteredSection() ---*/
	HBOOL  bBufferLocked;
	/*--- allocated by VA_DMX_AllocateSectionFilter() and freed by VA_DMX_FreeSectionFilter() ---*/
	HUINT8 *apSectionBuffer[VA_DMX_MAX_BUFFER]; // this is a ring buffer (FIFO), (head == tail) means empty
	HUINT32 aulSectionLength[VA_DMX_MAX_BUFFER];
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	HUINT32 ulTimeStamp[VA_DMX_MAX_BUFFER];	// for PVR
#endif
	HUINT8 ucBufferWritePos; // ucBufferHead, head : where new data will be written
	HUINT8 ucBufferReadPos; // ucBufferTail, tail : where stored data will be read
	HUINT8 ucBufferCnt; // number of stored data : 0 <= ucBufferCnt <= usNumBuffer <= VA_DMX_MAX_BUFFER (kVA_SETUP_NBMAX_SECTIONBUFFERS)
#ifdef __VIACCESS50__
	tVA_DMX_NotifyFilteredSection pfVa_DMX_NotifyFilteredSection;
#endif
} VA_DMX_Filter_t;

void VA_DMX_Init();

HERROR VA_DMX_PutEcmToCa(HUINT8 *pusRaw, HUINT16 usLoadId);
HERROR VA_DMX_PutEmmToCa(HUINT8 *pusRaw, HUINT16 usLoadId);
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HERROR VA_DMX_PutEcmFromPVR(HUINT32 ulAcsId, HUINT8 *pusRaw, HUINT16 usPid);
#endif

#endif //VA_DMX_API_H

