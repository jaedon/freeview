/**************************************************************************************/
/**
 * @file oplswupdate.h
 *
 * SWUPDATE Interfaces
 *
 **************************************************************************************
 **/

#ifndef __OPL_SWUPDATE_H__
#define __OPL_SWUPDATE_H__

#include "bpltype.h"
#include <dlib_swupdate.h>

#define SWUPDATE_STRING_MAX 128
#define SWUPDATE_VER_LEN    64 // from Sqc_scenario.c


typedef void *	OPL_SwupdateHandle;
typedef void (*OplSwupdateEvent_t)(DxSwUpdate_EventData_t *pstData);

typedef struct
{
	OPL_SwupdateHandle				 hSwupdate;
	DxSwUpdate_Source_e				 eDetectedSource;			// m/w event notify 문제로 , opl위에서 관리.
	HUINT32							 ulSessionId;
} _OPL_SWUPDATE_Info_t;


#ifdef __cplusplus
extern "C" {
#endif

OPL_SwupdateHandle OPL_Swupdate_New_withType(int otaType);
OPL_SwupdateHandle OPL_Swupdate_New(void);

void	OPL_SwupdateInit(OplSwupdateEvent_t listener);
void 	OPL_StartSwupdateDetect(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle);
void	OPL_StartSwupdateConnect(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle);
void	OPL_StartSwupdateDownload(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle);
void	OPL_StartSwupdateInstall(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle);
BPLStatus	OPL_StartSwupdateGetVersionBootupHumaxOta( char *pszVersion);
OPL_SwupdateHandle	OPL_StartSwupdateManualDetectForSat(
							HINT32 	frequency ,		/* Frequency */
							HINT32 	polarization ,	/* Polarization */
							HINT32	symbolrate , 	/* Symbol Rate */
							HINT32	transport , 	/* Transmission */
							HINT32	pskmode,		/* psk Mode */
							HINT32	fecrate 		/* FEC Code Rate */
							);
OPL_SwupdateHandle	OPL_StartSwupdateManualDetectForCab(
							HINT32 	frequency ,		/* Frequency */
							HINT32 	constellation ,	/* Constellation */
							HINT32	symbolrate  	/* Symbol Rate */
							);
void	OPL_StopSwupdateUpdate(OPL_SwupdateHandle hSwupHandle);
void	OPL_StopSwupdateDetect(OPL_SwupdateHandle hSwupHandle);
void	OPL_StopSwupdateConnect(OPL_SwupdateHandle hSwupHandle);
void	OPL_StopSwupdateDownload(OPL_SwupdateHandle hSwupHandle);
void	OPL_StopSwupdateInstall(OPL_SwupdateHandle hSwupHandle);
void	OPL_SystemRestartSwUpdate(void);
BPLStatus	OPL_SwUpdateGetNetworkId(int *networkId);
BPLStatus	OPL_SwUpdateSetNetworkId(int networkId);


#ifdef __cplusplus
};

#endif

#endif


