#include <nx_core_player.h>

typedef struct _NxPlayShahidContext
{
	NxPlayerContentInform_t	*pstShahidContentInform;
} Nx_PlayShahidContext_t;


ONDK_Result_t	nx_shahid_stream_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext);
ONDK_Result_t	nx_shahid_stream_MakeContentInform(Nx_PlayMgrContext_t * pstContents,HCHAR * szStreamURL);


