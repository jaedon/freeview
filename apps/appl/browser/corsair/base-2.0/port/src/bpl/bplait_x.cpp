/**************************************************************************************/
/**
 * @file bplait_x.cpp
 *
 * AIT Interfaces
 *
 **************************************************************************************
 **/

#include <hlib.h>
#include "bplait.h"

#include <cstring>

/******************************************************************
	class BplAitManager
******************************************************************/
unsigned int BplAitManager::getCount( void )
{
	unsigned int count = 0;

	if ( BPL_AIT_GetCount(&count) != BPL_STATUS_OK )
	{
		return 0;
	}

	return count;
}

BplAitSectionData *BplAitManager::getData( void )
{
	BplAitSectionData *pAit = NULL;

	if ( BPL_AIT_GetSectionData(&pAit) != BPL_STATUS_OK )
	{
		return NULL;
	}

	BplAitSectionData *ait = new BplAitSectionData();
	if ( !ait )
	{
		BPL_AIT_FreeSectionData(pAit);
		return NULL;
	}

	memcpy(ait, pAit, sizeof(*pAit));
#if 1//defined(SDK_3_2)
	HxSTD_MemSet(&pAit->stSiTable,0,sizeof(SiTable_t));
#endif
	BPL_AIT_FreeSectionData(pAit);

	return ait;
}
