/**************************************************************************************/
/**
 * @file bplpmt.h
 *
 * PMT Interfaces
 *
 **************************************************************************************
 **/

#ifndef __BPL_PMT_H__
#define __BPL_PMT_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/

/******************************************************************
	Typedef
******************************************************************/
typedef struct _BplPmt_t {
	unsigned short      usPID;         		   	/** PID of PMT */
	unsigned char       ucStreamType;   	  	/** stream type of PMT */
	unsigned char       ucComponentTag;  	 	/** component tag of PMT */
	unsigned short      usDataBroadcastTag; 	/** broadcast tag of PMT */
	unsigned int        ulCarouselId;			/** carousel ID */
} BplPmt_t;

/******************************************************************
	Interfaces	
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

BPLStatus BPL_PMT_GetCount(BplPmt_t **ppPmtInfo);

BPLStatus BPL_PMT_GetData(BplPmt_t **ppPmtInfo, unsigned int ulIndex);

BPLStatus BPL_PMT_FreeData(BplPmt_t *pPmtInfo);

#ifdef __cplusplus
};

/******************************************************************
	class BplPmtManager
******************************************************************/
typedef BplPmt_t	BplPmt;

class BplPmtManager {
public:
	BplPMT() {}
	virtual ~BplPMT() {}

public:
	BplPmt *getData(unsigned int ulIndex);
};

#endif

#endif
