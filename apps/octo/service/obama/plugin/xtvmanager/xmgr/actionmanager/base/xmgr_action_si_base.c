/****************************************************************************
*
* File Name   : manual_action_si_base.c
*
* Description :
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <db_param.h>

#include <svc_pipe.h>
#include <svc_resources.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>

#include "../local_include/_xmgr_action.h"

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

#define	_______GLOBAL_API_______

#define SI_TABLE_DEFAULT_TIMEOUT			10000
HERROR xmgr_action_SiGetDefaultTimeout_Base(SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	switch(eTableType)
	{
		case eSI_TableType_PAT:			*pulTimeout = 10000;					break;
		case eSI_TableType_CAT:			*pulTimeout = 10000;					break;
		case eSI_TableType_PMT:			*pulTimeout = 10000;					break;
		case eSI_TableType_NIT_ACT:		*pulTimeout = 25000;					break; /* SAT model의 경우 motor의 최대 동작 시간으로 정의 한다. */
		case eSI_TableType_NIT_OTH:		*pulTimeout = 25000;					break;
		case eSI_TableType_SDT_ACT:		*pulTimeout = 35000;					break;
		case eSI_TableType_SDT_OTH:		*pulTimeout = 35000;					break;
		case eSI_TableType_BAT:			*pulTimeout = 15000;					break; //20000	//mininam reducted waiting time(it seemed too long)
		case eSI_TableType_TDT:			*pulTimeout = 30000;					break;
		case eSI_TableType_TOT:			*pulTimeout = 30000;					break;
		case eSI_TableType_DSI:			*pulTimeout = 100*1000;					break;
		case eSI_TableType_DDB:			*pulTimeout = 20*1000;					break;
		case eSI_TableType_EIT_ACTPF:	*pulTimeout = 5000;						break; // 5 sec
		case eSI_TableType_EIT_OTHPF:	*pulTimeout = 30000;					break; // 30 sec
		case eSI_TableType_EIT_ACTSC:	*pulTimeout = 900000;					break; // 15 min
		case eSI_TableType_EIT_OTHSC:	*pulTimeout = 3600000;					break; // 60 min
		case eSI_TableType_UNT:			*pulTimeout = 25000;					break;
		case eSI_TableType_CIT:			*pulTimeout = 30000;					break;
		case eSI_TableType_CPT:			*pulTimeout = 20000;					break;
		case eSI_TableType_SGT:			*pulTimeout = 15000;					break;
		case eSI_TableType_AIT:			*pulTimeout = 2000;						break;
		case eSI_TableType_PMT_N:		*pulTimeout = 5000;						break; // (pmt timeout / 2)
		case eSI_TableType_ICT:			*pulTimeout = 30000;					break; //50000  //mininam reducted waiting time(it seemed too long)
		case eSI_TableType_DIT:			*pulTimeout = 25000;					break;
		case eSI_TableType_BIT:			*pulTimeout = 11000;					break;
		case eSI_TableType_SIT:			*pulTimeout = 30000;					break;
		default:						*pulTimeout = SI_TABLE_DEFAULT_TIMEOUT;	break;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_IRDETO)
HERROR xmgr_action_SiGetDefaultTimeout_Irdeto(SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	switch(eTableType)
	{
	case eSI_TableType_PAT:			*pulTimeout = 1200;			return ERR_OK;
	case eSI_TableType_CAT:			*pulTimeout = 2000;			return ERR_OK;
	case eSI_TableType_PMT:			*pulTimeout = 1200;			return ERR_OK;
	case eSI_TableType_SDT_ACT:		*pulTimeout = 10000;		return ERR_OK;
	case eSI_TableType_SDT_OTH:		*pulTimeout = 10000;		return ERR_OK;
	case eSI_TableType_DSI:			*pulTimeout = 30000;		return ERR_OK;
	default:
		/* timeout is not determined */
		return ERR_FAIL;
	}
}
#endif

