/**************************************************************
 *	@file		luna_test.c
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <directfb.h>
#include <sawman.h>

#include "vkernel.h"
#include "luna_test.h"
#include "hlib.h"

#define	DFBCHECK(x...)				\
	{								\
		DFBResult err = x;			\
		if (err != DFB_OK)			\
		{							\
			HxLOG_Print ("n\t\t ===========> %s <%d>:\n\t", __FILE__, __LINE__);	\
			DirectFBErrorFatal(#x, err);				\
		}							\
	}

#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)

/**************************************************************
 internal structures and variables
 **************************************************************/
static	IDirectFB				*s_pDirectFB = NULL;
static	IDirectFBDisplayLayer	*s_pLayer = NULL;

static HxInputRetType_e local_perform_KeyEvent(HxInputEvent_t *pstEvt, int mWindowId)
{
	int ulKeyCode = 0;
	int ulKeyMode = 0;

	ulKeyCode = pstEvt->key_symbol;
	ulKeyMode = pstEvt->type;

	{
		HxInputRetType_e eConsumed = exHINPUTRET_CONSUMED;

		/* something to do with key-code and key-mode */
	#if (0)
		eConsumed = do_someting(ulKeyCode, ulKeyMode);
	#endif

		if (eConsumed == exHINPUTRET_CONSUMED)
			return exHINPUTRET_CONSUMED;
		else
			return exHINPUTRET_PASS;
	}
}

static void local_perform_dfb_EventTask(void* args)
{
   	int					mWindowId;
   	HxInputEvent_t		stEvt;
   	HxInputRetType_e		eConsumed = exHINPUTRET_CONSUMED;

   	IDirectFBWindow		*dfbWindow;
	DFBWindowDescription	wdesc;
	IDirectFBSurface		*dfbSurface;

   	/* set a unique window_id for LUNA process */
   	mWindowId = LAPI_GetWindowId();
	HxLOG_Print ("\n\t\t ====> LAPI_GetWindowId (%d)....\n", LAPI_GetWindowId());

	wdesc.flags = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_OPTIONS | DWDESC_PIXELFORMAT;
	wdesc.caps  = DWCAPS_ALPHACHANNEL;
	wdesc.options = DWOP_ALPHACHANNEL | DWOP_SCALE ;
	wdesc.posx  = 0;
	wdesc.posy  = 0;
	wdesc.width  = SCREEN_WIDTH;
	wdesc.height = SCREEN_HEIGHT;
	wdesc.pixelformat = DSPF_ARGB;

	/* create a DFB window instance with the window_id */
	DFBCHECK(s_pLayer->CreateWindow(s_pLayer, &wdesc, &dfbWindow));
	DFBCHECK(dfbWindow->SetApplicationID(dfbWindow, mWindowId));
	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
	DFBCHECK(dfbSurface->Clear(dfbSurface, 0, 0, 0, 0));

	/* register the window_id to HAMA process via such a below api */
   	LAPI_SetWindowStyle(mWindowId, exHWS_WindowOwnControl | exHWS_WindowInputAlways, FALSE);

   	while(TRUE)
   	{
   		memset(&stEvt, 0, sizeof(HxInputEvent_t));
	     	/* get input key-event from HAMA via such a below api */
	    	if (LAPI_GetInputEvent(mWindowId, &stEvt, 0) == TRUE)
	    	{
			HxLOG_Print ("\n\t\t =======> Window ID[0x%x] KeyInfo:[code=0x%x, type=0x%x]\n", mWindowId, stEvt.key_symbol, stEvt.type);
			/* preform something to do and return a value(type of HInputRetType_e, ref. blow reference */
			eConsumed = local_perform_KeyEvent(&stEvt, mWindowId);
			/* notify a result of the input-key-event processing */
			LAPI_ReturnKeyEvent(mWindowId, eConsumed, &stEvt);
	    	}
   	}
}

int main(int argc, char **argv)
{
	#define	SIZE_64K	(0x00010000)
	unsigned long tid;

	/* init the LAPI */
	LAPI_Init();

	DFBCHECK(DirectFBInit(0, NULL));
	DFBCHECK(DirectFBCreate(&s_pDirectFB));
	DFBCHECK(s_pDirectFB->SetCooperativeLevel(s_pDirectFB, DFSCL_NORMAL));
	DFBCHECK(s_pDirectFB->GetDisplayLayer(s_pDirectFB, DLID_PRIMARY, &s_pLayer));
	DFBCHECK(s_pLayer->SetCooperativeLevel(s_pLayer, DLSCL_ADMINISTRATIVE));

	(void)VK_TASK_Create(local_perform_dfb_EventTask, 70, SIZE_64K, "LunaTest", (void*)NULL, (unsigned long*)&tid, 0);
	VK_TASK_Start(tid);

	while(TRUE)
	{
		VK_TASK_Sleep(100000);
	}

	return (1);
}

