/**************************************************************************************/
/**
 * @file oploperator.c
 *
 * Defines the query structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oploperator_freesat.h"

#include <hlib.h>
#include <apk.h>
#include <appkit_freesat.h>





#define	____OPL_OPERATOR_PRIVATE_____________________________________________________________________
static	void	(*_opl_operator_callback)(int eType, int subParam) = NULL;
static	void	(*_opl_op_mheg_callback)(int eType, int Param) = NULL;

static	void	local_Operator_FSAT_MetaEventListner(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static	void	local_Operator_FSAT_MhegEventListner(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);



#define	____OPL_OPERATOR_PUBLIC______________________________________________________________________
OPL_BOOL	OPL_Operator_FSAT_InitMetaEventListner()
{
	APK_OPERATOR_FSAT_RegisterNotifier(local_Operator_FSAT_MetaEventListner);
	APK_MHEG_RegisterNotifier(local_Operator_FSAT_MhegEventListner);

	return OPL_TRUE;
}

OPL_BOOL	OPL_Operator_FSAT_RegisterMetaEventListener(void (*cbFunc)(int eType, int subParam))
{
	/* type : OPL_OPERATOR_FSAT_MetaEventType_e = eOpFSAT_MetaEventType_Xml, eOpFSAT_MetaEventType_Img */
	_opl_operator_callback = cbFunc;

	return OPL_TRUE;
}

OPL_BOOL	OPL_Operator_FSAT_RegisterMhegEventListener(void (*cbFunc)(int eType, int Param))
{
	_opl_op_mheg_callback = cbFunc;

	return OPL_TRUE;
}

static	void	local_Operator_FSAT_MetaEventListner(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	APKE_FreesatMeta_EventType eType = (APKE_FreesatMeta_EventType)ulParam1;
	APKE_FreesatMeta_EventUpdatedFile	eFile = 0;

	if(_opl_operator_callback == NULL)
		return;

	switch(eType)
	{
		case eAkeFsatMeta_EventType_Xml:
			_opl_operator_callback(eOpFSAT_MetaEventType_Xml, (int)ulParam2);
			break;

		case eAkeFsatMeta_EventType_Img:
			eFile = (APKE_FreesatMeta_EventUpdatedFile)ulParam2;
			switch(eFile)
			{
				case eAkeFsatMeta_EventUpdatedFile_ChIcon:
					_opl_operator_callback((int)eOpFSAT_MetaEventType_Img, (int)eOpFSAT_MetaFileType_ChIcon);
					break;

				case eAkeFsatMeta_EventUpdatedFile_Images:
					_opl_operator_callback((int)eOpFSAT_MetaEventType_Img, (int)eOpFSAT_MetaFileType_Image);
					break;

				case eAkeFsatMeta_EventUpdatedFile_All:
					_opl_operator_callback((int)eOpFSAT_MetaEventType_Img, (int)eOpFSAT_MetaFileType_All);
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
}

static	void	local_Operator_FSAT_MhegEventListner(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HUINT8 *pucUrl = (HUINT8 *)ulParam2;

	if(_opl_op_mheg_callback == NULL || pucUrl == NULL)
		return;

	_opl_op_mheg_callback(0, (int)pucUrl);
}



