#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_DMS
#define __NATIVE_JLABS_DMS

#include "NativeOOIFTypes.h"
#include "NativeJlabsCDSRecording.h"
#include "NativeJlabsContentSearch.h"
#include "NativeJlabsRecordDestination.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef void    (*DlnaDmrEventListener)(unsigned int event, unsigned int result, void *cdsItem);
typedef enum
{
	eNATIVE_DMR_REQ_PLAY,
	eNATIVE_DMR_PLAY_STOP,
} NativeMRCP_Request_e;

OOIFReturnCode  native_jlabsDMRControllerStop(OOIFBoolean *retval);
OOIFReturnCode  native_jlabsDMRControllerStart(OOIFBoolean *retval);
OOIFReturnCode 	native_jlabsDMRControllerIsPlay(OOIFBoolean *retval);
OOIFReturnCode 	native_jlabsDMRControllerPlayingStop(OOIFBoolean *retval);

OOIFReturnCode  native_jlabsDMRControllerEventListener(DlnaDmrEventListener listener);
OOIFReturnCode  native_jlabsDMRControllerUnRegEventListener();


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif 

#endif 



