/**************************************************************************************/
/**
 * @file opldiscinfo.c
 *
 * Details of storage usage
 *
 **************************************************************************************
 **/

#include "opldiscinfo.h"
#include <hlib.h>

/******************************************************************
	Interfaces
******************************************************************/
BPLStatus OPL_Discinfo_GetFree(unsigned int *pulFree)
{
	if ( pulFree == NULL )
	{
		HxLOG_Error("The buffer is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	*pulFree = 0;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Discinfo_GetTotal(unsigned int *pulTotal)
{
	if ( pulTotal == NULL )
	{
		HxLOG_Error("The buffer is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	*pulTotal = 0;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Discinfo_GetReserved(unsigned int *pulReserved)
{
	if ( pulReserved == NULL )
	{
		HxLOG_Error("The buffer is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	*pulReserved = 0;

	return BPL_STATUS_OK;
}
