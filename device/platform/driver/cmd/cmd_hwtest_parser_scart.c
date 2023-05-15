/*----------------------------------------------------------------------------
** Include
** --------------------------------------------------------------------------*/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include "htype.h"
#include "di_err.h"
#include "vkernel.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_mmc.h"
#include "drv_scart.h"
#include "di_scart.h"

/*----------------------------------------------------------------------------
** Define
** --------------------------------------------------------------------------*/
#define HWTEST_CMD          szCmd
#define HWTEST_PARAM        szParam
#define HWTEST_PARAM1       szParam1
#define HWTEST_PARAM2       szParam2
#define HWTEST_PARAM3       szParam3
#define GET_ARGS            int iResult=CMD_ERR; \
                            char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)           (HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)         (HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)        (HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)        (HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)        (HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)

#define READABLE_IN_HEX(sz,val)    CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val)    CMD_ReadAsDec(sz, &val)

/*----------------------------------------------------------------------------
** Variable
** --------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------*/
int CMD_HwTest_Parser_Scart(void *szArgs)
{
	GET_ARGS;
    szParam1=szParam1;
	szParam2=szParam2;

	DRV_SCART_OPERATION* pstScartOp;
	pstScartOp = DRV_SCART_GetOpHandle();

	// Slow Blanking : 12V(4:3), 6V(16:9), 0V(Int. TV)
	if CMD_IS("ar")
	{
		if PARAM_IS("0")
		{
			CMD_Printf("Command : scart ar 0 -> AR-4:3, Slow Blanking-12V\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eTvAspectRatioOutput=DI_SCART_AR_4_3;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else if PARAM_IS("1")
		{
			CMD_Printf("Command : scart ar 1 -> AR-16:9, Slow Blanking-6V\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eTvAspectRatioOutput=DI_SCART_AR_16_9;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else if PARAM_IS("2")
		{
			CMD_Printf("Command : scart ar 2 -> AR-Int. TV, Slow Blanking-0V, scart switch off \r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eTvAspectRatioOutput=DI_SCART_NO_SIGNAL;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Scart] Error, unknown action: right action -> scart ar [0(12V_4:3)/1(6V_16:9)/2(0V_Int TV)]\r\n");
			return CMD_ERR;
		}

	}
	// Fast Blanking : 0V(CVBS & S-VIDEO), 3.5V(RGB)
	else if CMD_IS("tv_vid")
	{
		if PARAM_IS("0")		// CVBS, FB = 0V
		{
			CMD_Printf("Command : scart tv_vid 0 -> TV Scart-CVBS, Fast Blanking-0V\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eTvVideoOutput=DI_SCART_OUTPUT_CVBS;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else if PARAM_IS("1")	//RGB, FB = 3.5V
		{
			CMD_Printf("Command : scart tv_vid 1 -> TV Scart-RGB, Fast Blanking-3.5V\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eTvVideoOutput=DI_SCART_OUTPUT_RGB;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else if PARAM_IS("2")	//SVHS(S-Video), FB = 0V
		{
			CMD_Printf("Command : scart tv_vid 2 -> TV Scart-S-Video, Fast Blanking-0V\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eTvVideoOutput=DI_SCART_OUTPUT_SVIDEO;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Scart] Error, unknown action: right action -> scart tv_vid [0(CVBS,FB-0V)/1(RGB,FB-3.5V)/2(S-VIDEO,FB-0V)]\r\n");
			return CMD_ERR;
		}

	}
	else if CMD_IS("vcr_vid")
	{
		if PARAM_IS("0")		// CVBS, FB = 0V
		{
			CMD_Printf("Command : scart vcr_vid 0 -> VCR Scart-CVBS, Fast Blanking-0V\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eVcrVideoOutput=DI_SCART_OUTPUT_CVBS;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
// VCR SCART do not exist "RGB"
#if	0
		else if PARAM_IS("1")	//RGB, FB = 3.5V
		{
			iResult=CMD_OK;
		}
#endif
		else if PARAM_IS("2")	// S-VIDEO, FB = 0V
		{
			CMD_Printf("Command : scart vcr_vid 2 -> VCR Scart-S-Video, Fast Blanking-0V\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eVcrVideoOutput=DI_SCART_OUTPUT_SVIDEO;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else if PARAM_IS("3")
		{
			CMD_Printf("Command : scart vcr_vid 3 -> VCR Scart : No Signal\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.stNormalMode.eVcrVideoOutput=DI_SCART_NO_SIGNAL;
			DI_SCART_SetSettings(&stScartSettings);
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Scart] Error, unknown action: right action -> scart vcr_vid [0(CVBS,FB-0V)/2(S-VIDEO,FB-0V)]\r\n");
			return CMD_ERR;
		}

	}
	else if CMD_IS("bypass")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : scart bypass on : VCR -> TV Path\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.eMode=DI_SCART_MODE_BYPASS;
			DI_SCART_SetSettings(&stScartSettings);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("VCR -> TV Path\n");
            }

			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : scart bypass off : IRD(STB) ->TV/VCR Path\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.eMode=DI_SCART_MODE_NORMAL;
			DI_SCART_SetSettings(&stScartSettings);
            if ( CMD_GetMsgUI() != NULL )
            {
                (int *)(CMD_GetMsgUI())("IRD(STB) ->TV/VCR Path\n");
            }

			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Scart] Error, unknown action: right action -> scart bypass [on/off]\r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("hwbypass")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command : scart hwbypass on : VCR <-> TV Path\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.eMode=DI_SCART_MODE_HW_BYPASS_NO_DET;
			DI_SCART_SetSettings(&stScartSettings);
            if ( CMD_GetMsgUI() != NULL )
			{
                (int *)(CMD_GetMsgUI())("VCR <-> TV Path\n");
			}
			iResult=CMD_OK;
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : scart hwbypass off : IRD(STB) ->TV/VCR Path\r\n");
			DI_SCART_SETTINGS stScartSettings;
			DI_SCART_GetSettings(&stScartSettings);
			stScartSettings.eMode=DI_SCART_MODE_NORMAL;
			DI_SCART_SetSettings(&stScartSettings);
            if ( CMD_GetMsgUI() != NULL )
			{
                (int *)(CMD_GetMsgUI())("IRD(STB) ->TV/VCR Path\n");
			}
			iResult=CMD_OK;
		}
		else
		{
			CMD_Printf("[CMD_HwTest_Parser_Scart] Error, unknown action: right action -> scart bypass [on/off]\r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("vol")
	{
		HINT32 vol;

		vol = VK_atoi((char *)szParam);
		CMD_Printf("SCART volume:%ld\n", vol);
		// STV6417 has 20 stems of volume level.
		// Therefore, divide the input volume with 5/
		if(DRV_SCART_SetVolume(vol/5) == DRV_OK)
			return 0;
		else
			CMD_Printf("%s failed!\n", __FUNCTION__);

		iResult=CMD_OK;
	}
	else if CMD_IS("mute")
	{
		DRV_SCART_SetTVMute(TRUE);
		CMD_Printf("SCART mute\n");
		iResult=CMD_OK;
	}
	else if CMD_IS("unmute")
	{
		DRV_SCART_SetTVMute(FALSE);
		CMD_Printf("SCART un-mute\n");
		iResult=CMD_OK;
	}
	else if CMD_IS("w")
	{
		unsigned int 	uiOffset, uiData;

        if ( (READABLE_IN_HEX(HWTEST_PARAM, uiOffset) != 1) ||
             (READABLE_IN_HEX(HWTEST_PARAM1, uiData) != 1) )
		{
			return CMD_ERR;
		}

		CMD_Printf("Command : scart w 0x%x 0x%x -> SCART Register Write function Test \r\n", uiOffset, uiData);
		pstScartOp->pfWrite_Reg(uiData, uiOffset);
		iResult=CMD_OK;
	}
	else if CMD_IS("reg")
	{
		CMD_Printf("Command : scart reg -> Print SCART Register\r\n");
		pstScartOp->pfDump_Reg();
		iResult=CMD_OK;
	}
	else if CMD_IS("info")
	{
		DRV_SCART_PrintStatus();
		iResult=CMD_OK;
	}
	else
	{
		CMD_Printf("[CMD_HwTest_Parser_Scart] Error, unknown scart command: right command -> scart [ar/tv_vid/vcr_vid/bypass/w/reg] [xxx]\r\n");
		return CMD_ERR;
	}

	return iResult;
}
