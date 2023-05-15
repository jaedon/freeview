/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2014/11/22
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <htype.h>
#include <stdarg.h>
#include <vkernel.h>

#include <drv_err.h>
#include <flash_map.h>

#include <drv_nvram.h>
#include <nvram_map.h>

#include <drv_bootloader.h>

#if defined(CONFIG_UOCTO_OTATYPE_IN_REGISTER)
#include <di_loader.h>
#endif
#include "di_fs.h"

#include "cmdtool.h"
#include "humax_bsu.h"
/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/

#if defined(BSU)
#else
	#define BSU
#endif


#if defined(CONFIG_ARM)
#define BOLT_BSU
#elif defined(CONFIG_MIPS)
#define CFE_BSU
#endif

#if defined (CONFIG_DEBUG)
#define PrintInfo		VK_printf
#define PrintDebug	VK_DBG_Print
#define PrintError		VK_printf

static HINT32 local_MAC_str2bin(HCHAR *srcptr, HUINT8 *dstptr);
#else
#define PrintInfo(...)
#define PrintDebug(...)
#define PrintError(...)
#endif

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/
#define MAX_MAC_BIN_SIZE         6
#define MAX_MAC_STR_SIZE         18


#define SHORT_BUFFER_SIZE		32
#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
#define LONG_BUFFER_SIZE		768
#define RSAKEY_SIZE			(256+5+4)		/*Default : RSA 2048 bits(256) + HUMAX string("HKS" + length)(5) + CRC32(4) */
#else
#define LONG_BUFFER_SIZE		256
#endif
#define MAX_TOKENS			64
#define MAX_SWITCHES		16
#define MAX_TOKEN_SIZE  		1000
#define DEFAULT_MAC  		"00:01:02:03:00:FF"
#define ENV_FLG_NORMAL		0x00	/* normal read/write */


#define UNUSED(x)		(void)(x)


typedef struct cmdsw_s {
	HINT32 swidx;
	HCHAR *swname;
	HCHAR *swvalue;
} cmdsw_t;

typedef struct cmdline_s {
	HINT32 argc;
	HCHAR *argv[MAX_TOKENS];
	HINT32 swc;
	cmdsw_t swv[MAX_SWITCHES];
} cmdline_t;

struct lexstate {
	const HCHAR *rawline;
	const HCHAR *lineptr;
	const HCHAR *saveptr;
	HINT32 curpos;
	HINT32 expand;
};

static HCHAR *varchars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_?";


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#define put32bit(p, data) *((HUINT8*)p + 0) = (HUINT8)(data >> 24), *((HUINT8*)p + 1) = (HUINT8)(data >> 16), *((HUINT8*)p + 2) = (HUINT8)(data >> 8), *((HUINT8*)p + 3) = (HUINT8)(data)

typedef struct tagDRVBootloader_Context_t
{
	DRV_LOADER_BootOption_t stBootOpt;

	HBOOL		bIsMac2Addr;
	HCHAR		acMac2String[SHORT_BUFFER_SIZE];

	HBOOL		bIsSecureRegionRam;
#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
	HCHAR		acRSA2String[LONG_BUFFER_SIZE];
	HCHAR		acRsakey[RSAKEY_SIZE];
#endif

} DRV_Bootloader_Context_t;

static DRV_Bootloader_Context_t s_stDrvBloaderContext;

static void local_cmd_dbgCommand(void);
static DRV_Error	local_bootloader_Setenv(const HCHAR	*pszName, const HCHAR *pszValue, HINT32 flags);


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
static DRV_Error local_bloader_SetEnv(void)
{
	DRV_Error	ret = ERR_FAIL;
	HCHAR * envdevice = NULL;
#if defined(CONFIG_DEBUG)
    HCHAR       env[LONG_BUFFER_SIZE];
    HCHAR       envOpt[LONG_BUFFER_SIZE];
#endif

	BSU_free_envdevname();

#if defined(CONFIG_ARM) && defined(CONFIG_USB)
	// CFE don't have this command.
	ret = VK_SYSTEM_Command ("usb init");
	if(ERR_OK != ret)
	{
		PrintError("usb init error");
		return ret;
	}
#endif

#ifndef N_PART_NVRAM_ENV_DEVICE
#error "Please define N_PART_NVRAM_ENV_DEVICE"
#endif
	envdevice = N_PART_NVRAM_ENV_DEVICE;
	PrintInfo("Set %s as envdevice\n", envdevice);

	ret = DRV_BLOADER_set_envdevice(envdevice);
	if(ERR_OK != ret)
	{
		PrintError("nvram set error");
		return ret;
	}

#if defined(CONFIG_DEBUG)
    /* check env : ETH0_HWADDR */
    ret = BSU_getenv("ETH0_HWADDR", env, LONG_BUFFER_SIZE);
    if (ret == DRV_OK && (VK_strcmp(env, "00:00:00:00:00:00") != 0))
    {
        //PrintInfo("ETH0_HWADDR is [%s]\n",env);
    }
    else
    {
		PrintInfo("Set DEFAULT_MAC(%s)\n", DEFAULT_MAC);

		ret = local_bootloader_Setenv("ETH0_HWADDR", DEFAULT_MAC, ENV_FLG_NORMAL);
		if(ERR_OK != ret)
		{
			PrintError("[%s:%d] error (%d)\r\n", __FUNCTION__, __LINE__, ret);
			return ret;
		}
    }
#endif

	return ret;
}

static HINT32 local_getvarname(const HCHAR **ptr, HCHAR *vname, HINT32 vmax)
{
	const HCHAR *p;
	HCHAR *v;

	p = *ptr;
	v = vname;

	vmax--;

	if (*p == '{') {
		p++;
		while (*p && (*p != '}'))
			*v++ = *p++;
		if (*p == '}')
			p++;
	}
	else {
		while (*p) {
			if (VK_strchr(varchars, *p)) {
				*v++ = *p++;
				vmax--;
				if (vmax == 0)
					break;
				continue;
			}
			break;
		}
	}

	*v = 0;
	*ptr = p;

	return v != vname;	/* nonzero if we got something */
}

static void local_lex_init(struct lexstate *lex, const HCHAR *line, HINT32 expand)
{
	lex->rawline = line;
	lex->lineptr = line;
	lex->saveptr = NULL;
	lex->expand = expand;
	lex->curpos = 0;
}

static HINT32 local_lex_gettoken(struct lexstate *lex, HCHAR *token, HINT32 tmax)
{
	const HCHAR *p;
	HCHAR *t;
	HINT32 inquote_d = 0;
	HINT32 inquote_s = 0;
	HINT32 inbracket = 0;
	HINT32 inexpansion;
	static HCHAR *brkchars = " ;{}";

	t = token;
	inexpansion = (lex->saveptr != NULL);
	p = lex->lineptr;

	while (*p) {
		if (*p == ' ') {
			p++;
			continue;
		}
		break;
	}

	if (inexpansion)
		lex->curpos = lex->lineptr - lex->rawline;

	lex->lineptr = p;

	if (!*p && !inexpansion)
		return 0;

	tmax--;

	for (;;) {
		if (tmax == 0)
			break;

		if( p == NULL )
			break;

		if (!*p && inexpansion) {
			p = lex->saveptr;
			inexpansion = 0;
			lex->saveptr = NULL;
		}

		if (!*p)
			break;

		if (t == token) {
			if (VK_strchr(brkchars, *p)) {
				*t++ = *p++;
				break;
			}
		}

		if ((*p == '\\') && !inquote_s) {
			if (VK_strchr("\\\"$", *(p + 1))) {
				p++;
				*t++ = *p;
				tmax--;
				if (*p)
					p++;
				continue;
			}
		}

		if (lex->expand && !inexpansion && (*p == '$') && !inquote_s) {
			HCHAR vname[100];

			p++;
			if (local_getvarname(&p, vname, sizeof(vname))) {
				lex->saveptr = p;
				inexpansion = 1;
				p = NULL;
			}
			continue;
		}

		if ((*p == '[') && !inquote_d && !inquote_s) {
			p++;
			inbracket = 1;
			continue;
		}

		if (inbracket && (*p == ']')) {
			p++;
			inbracket = 0;
			continue;
		}

		if (*p == '\'') {
			/* but only if not in double quotes */
			if (inquote_d == 0) {
				p++;
				inquote_s = !inquote_s;
				continue;
			}
		}

		if ((*p == '"') && !inbracket) {
			/* but only if not in single quotes */
			if (inquote_s == 0) {
				p++;
				inquote_d = !inquote_d;
				continue;
			}
		}

		if (!inbracket && !inquote_s && !inquote_d &&
		    (VK_strchr(brkchars, *p))) {
			break;
		}

		*t++ = *p++;
		tmax--;
	}

	lex->lineptr = p;
	*t = 0;
	return (t != token);
}

HINT32 local_cmd_sw_value(cmdline_t *cmd, HCHAR *swname, HCHAR **swvalue)
{
	HINT32 idx;

	for (idx = 0; idx < cmd->swc; idx++) {
		if (VK_strcmp(swname, cmd->swv[idx].swname) == 0) {
			*swvalue = cmd->swv[idx].swvalue;
			return 1;
		}
	}

	return 0;
}

HCHAR *local_cmd_sw_name(cmdline_t *cmd, HINT32 swidx)
{
	if ((swidx < 0) || (swidx >= cmd->swc))
		return NULL;

	return cmd->swv[swidx].swname;
}

HINT32 local_cmd_sw_isset(cmdline_t *cmd, HCHAR *swname)
{
	HINT32 idx;

	for (idx = 0; idx < cmd->swc; idx++) {
		if (VK_strcmp(swname, cmd->swv[idx].swname) == 0)
			return 1;
	}

	return 0;
}

void local_bloader_cmd_parse(cmdline_t *cmd, const HCHAR *line)
{
	HCHAR *cldup;
	HINT32 res, cnt;
	HCHAR *tokens[MAX_TOKENS + MAX_SWITCHES];
	HCHAR *x;
	HINT32 tmax;
	HINT32 idx;
	HINT32 len;
	struct lexstate lex;

	/*
	 * Start with nothing.
	 */

	cmd->argc = 0;
	cmd->swc = 0;

	tmax = MAX_TOKEN_SIZE;
	cldup = VK_MEM_Alloc(tmax);
	/*
	 * Chop up the command line into tokens.
	 */

	cnt = 0;
	local_lex_init(&lex, line, 1);

	while ((res = local_lex_gettoken(&lex, cldup, tmax))) {
		len = VK_strlen(cldup);
		tokens[cnt] = VK_MEM_Alloc(len+1);
		VK_strcpy(tokens[cnt], cldup);
		cnt++;
		if (cnt == (MAX_TOKENS + MAX_SWITCHES))
			break;
	}
	/*
	 * Make a pass over the accumulated tokens and
	 * weed out the ones that look like switches
	 */

	for (idx = 0; idx < cnt; idx++) {
		if (tokens[idx][0] != '-') {
			if (cmd->argc < MAX_TOKENS) {
				cmd->argv[cmd->argc] = tokens[idx];
				cmd->argc++;
			} else {
				VK_MEM_Free(tokens[idx]);
			}
		} else {
			if (cmd->swc < MAX_SWITCHES) {
				cmd->swv[cmd->swc].swname = tokens[idx];
				x = VK_strchr(cmd->swv[cmd->swc].swname, '=');
				if (x) {
					*x++ = '\0';
					cmd->swv[cmd->swc].swvalue = x;
				} else {
					cmd->swv[cmd->swc].swvalue = NULL;
				}
				/*
				 * swidx is the index of the argument that this
				 * switch precedes.  So, if you have
				 * "foo -d bar", swidx for "-d" would be 1.
				 */
				cmd->swv[cmd->swc].swidx = cmd->argc;
				cmd->swc++;
			} else {
				VK_MEM_Free(tokens[idx]);
			}
		}
	}

	VK_MEM_Free(cldup);
}

void local_bloader_cmd_free(cmdline_t *cmd)
{
	HINT32 idx;

	for (idx = 0; idx < cmd->argc; idx++)
		VK_MEM_Free(cmd->argv[idx]);
	for (idx = 0; idx < cmd->swc; idx++)
		VK_MEM_Free(cmd->swv[idx].swname);

	cmd->swc = 0;
}

static HERROR	local_bootloader_GetNvramMacStr(HCHAR	*pszMac, HINT32 length)
{
	HUINT8		readbuffer[32] = {0, };

#if defined(CONFIG_DEBUG)
	HUINT8		readbufferTemp[32] = {0, };
#endif

	DI_ERR_CODE diErr = DI_ERR_OK;

	diErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_MAC_ADDR, 0, readbuffer, MAX_MAC_BIN_SIZE);
	if(DI_ERR_OK != diErr)
	{
		PrintError("DI_NVRAM_GetField Error");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	if(VK_memcmp(readbuffer, readbufferTemp, 32) == 0)
	{
		PrintError("DI_NVRAM_Read Error");
		return ERR_FAIL;
	}
#endif

	VK_snprintf(pszMac, length, "%02X:%02X:%02X:%02X:%02X:%02X", readbuffer[0], readbuffer[1],readbuffer[2],readbuffer[3],readbuffer[4],readbuffer[5]);
	length--;
	pszMac[length] = 0;

	PrintInfo("Mac Address is %s\n", pszMac);

	return ERR_OK;

}

static DRV_Error	local_bootloader_Setenv(const HCHAR	*pszName, const HCHAR *pszValue, HINT32 flags)
{
	HINT32	res = -1;

	res = BSU_setenv(pszName, pszValue, flags);
	if(ERR_OK != res)
	{
		PrintError("Could not set environment variable (%s)", pszName);
		return DRV_ERR;
	}

	res = BSU_saveenv();
	if(ERR_OK != res)
	{
		PrintError("Could not save environment variable (%s)", pszName);
		return DRV_ERR;
	}

	return DRV_OK;
}

static HERROR	local_bootloader_Selectboot_BootRam(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HCHAR	*bootaddr = NULL;
	HCHAR	*bootsize = NULL;
	HBOOL	bNormalBoot = FALSE;
	HINT32 		res = -1;

	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

	local_cmd_sw_value(pCmdLine, "-bootaddr", &bootaddr);
	local_cmd_sw_value(pCmdLine, "-bootsize", &bootsize);
	bNormalBoot = local_cmd_sw_isset(pCmdLine, "-normal");

	if( bootaddr == NULL || bootsize == NULL )
	{
		PrintInfo("invailed address & size \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_ARM)
	VK_sprintf(pszAutoBootCmd, "boot -zimg -rawfs mem0:%s,%s",bootaddr, bootsize);
#else
	// cfe 에서 테스트 하지 않은 코드 입니다. 기존 cfe 코드에서 devname == NULL 인 경우에 대한 HUMAX modify를 확인해야 합니다.
	VK_sprintf(pszAutoBootCmd, "boot -elf -rawfs %s,%s ",bootaddr, bootsize);
#endif

	if(bNormalBoot==TRUE)
	{
		if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
		{
			VK_sprintf(pszAutoBootCmd, "%s \"%s %s",pszAutoBootCmd,N_IMAGE_KERNEL_BOOT_OPT,pContext->acMac2String);
		}
		else
		{
			VK_sprintf(pszAutoBootCmd, "%s \"%s",pszAutoBootCmd,N_IMAGE_KERNEL_BOOT_OPT);
		}
	}
	else
	{
#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
		if(eDRV_BOOTOPTION_RSA_KEY & pContext->stBootOpt.eOption)
		{
			VK_sprintf(pszAutoBootCmd, "%s \"%s", pszAutoBootCmd, pContext->acRSA2String);
		}
#endif
	}

	if(eDRV_BOOTOPTION_EXT_APP & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s", pszAutoBootCmd, pContext->stBootOpt.extAppName);
	}

	VK_sprintf(pszAutoBootCmd, "%s\"",pszAutoBootCmd);

	PrintInfo("%s\n",pszAutoBootCmd);

	res = VK_SYSTEM_Command(pszAutoBootCmd);
	if (res < 0)
	{
		PrintInfo("Cannot boot to app\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	local_bootloader_Selectboot_BootFlash(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HINT32 		res = -1;
	const char *bootdev = N_IMAGE_KERNEL_BOOT_DEVICE;
	HUINT32 offset = N_IMAGE_KERNEL_OFFSET;
	const char *bootopt = N_IMAGE_KERNEL_BOOT_OPT;
	HCHAR		env[LONG_BUFFER_SIZE] = {0, };

	HAPPY(bootopt);
	HAPPY(offset);
	HAPPY(bootdev);
	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

	res = BSU_getenv("FLASH_BOOT_CMD", env, LONG_BUFFER_SIZE);
	if(res == DRV_OK && VK_strlen(env) != 0)
	{
		PrintInfo("1st Image Booting : %s\n", env);
		res = VK_SYSTEM_Command(env);
		if (res < 0)
		{
			PrintInfo("kernel broken boot otakernel\n");
		}
	}

#if defined(CONFIG_PROD_VTV200)
		PrintInfo("Consider 4 byte...\n");
		offset += 4;
#endif

#if defined(CONFIG_EMMC_FLASH)
		VK_sprintf(pszAutoBootCmd, "boot %s:0x%x \"%s", N_IMAGE_KERNEL_BOOT_DEVICE, N_IMAGE_KERNEL_OFFSET, N_IMAGE_KERNEL_BOOT_OPT);
#elif defined(CONFIG_NAND_FLASH)
	#if defined(CONFIG_PROD_VTV200)
		VK_sprintf(pszAutoBootCmd, "boot -nz -raw -addr=0x80100000 -max=94371840 %s:0x%x \"%s", bootdev, offset, bootopt);
	#else
		VK_sprintf(pszAutoBootCmd, "boot %s:0x%x \"%s", bootdev, offset, bootopt);
	#endif
#else
		VK_sprintf(pszAutoBootCmd, "boot ubi0.flash0.ubi:0x%x \"mtd2ubi ubi.mtd=%d root=%s", N_IMAGE_KERNEL_OFFSET,N_HMX_MTD2UBIBLOCK_NODE,N_HMX_MTD2UBIROOTFS_NODE);
#endif

	if(eDRV_BOOTOPTION_EXT_APP & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s", pszAutoBootCmd, pContext->stBootOpt.extAppName);
	}

	if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s",pszAutoBootCmd,pContext->acMac2String);
	}

	VK_sprintf(pszAutoBootCmd, "%s\"",pszAutoBootCmd);

	PrintInfo("1st Image Booting : %s\n", pszAutoBootCmd);

	res = VK_SYSTEM_Command(pszAutoBootCmd);
	if (res < 0)
	{
		PrintInfo("Cannot boot to app\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	local_bootloader_Selectboot_Boot2ndFlash(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HINT32 		res = -1;

	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

#if defined(CONFIG_EMMC_FLASH)
	#if defined(N_IMAGE_2ND_KERNEL_BOOT_OPT)
			VK_sprintf(pszAutoBootCmd, "boot %s:0x%x \"%s", N_IMAGE_2ND_KERNEL_BOOT_DEVICE, N_IMAGE_2ND_KERNEL_OFFSET, N_IMAGE_2ND_KERNEL_BOOT_OPT);
	#endif /* N_IMAGE_2ND_KERNEL_BOOT_OPT */
#else /* CONFIG_EMMC_FLASH */
	#if defined(N_UBI_2ND_KERNEL_OFFSET)
		#if defined(CONFIG_NAND_FLASH) && defined(CONFIG_BOOT_FROM_NAND)
				VK_sprintf(pszAutoBootCmd, "boot ubi0.flash0.ubi:0x%x \"mtd2ubi ubi.mtd=%d root=%s", N_UBI_2ND_KERNEL_OFFSET,N_HMX_MTD2UBIBLOCK_NODE,N_HMX_MTD2UBIPART2_NODE);
		#else
				VK_sprintf(pszAutoBootCmd, "boot ubi0.flash1.ubi:0x%x \"mtd2ubi ubi.mtd=%d root=%s", N_UBI_2ND_KERNEL_OFFSET,N_HMX_MTD2UBIBLOCK_NODE,N_HMX_MTD2UBIPART2_NODE);
		#endif
	#endif /*N_UBI_2ND_KERNEL_OFFSET*/

#endif /*CONFIG_EMMC_FLASH */

	if(eDRV_BOOTOPTION_EXT_APP & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s", pszAutoBootCmd, pContext->stBootOpt.extAppName);
	}

	if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s",pszAutoBootCmd,pContext->acMac2String);
	}

	VK_sprintf(pszAutoBootCmd, "%s\"",pszAutoBootCmd);

	PrintInfo("2ND Image Booting : %s\n", pszAutoBootCmd);
	res = VK_SYSTEM_Command(pszAutoBootCmd);
	if (res < 0)
	{
		PrintInfo("Cannot boot to app(backup)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


static HERROR	local_bootloader_Selectboot_BootOTA(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HINT32 		res = -1;

	const char *bootdev = N_IMAGE_OTA_KERNEL_BOOT_DEVICE;
	HUINT32 offset = N_IMAGE_OTA_KERNEL_OFFSET;
	const char *bootopt = N_IMAGE_OTA_KERNEL_BOOT_OPT;

	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

#if defined(CONFIG_PROD_VTV200)
		PrintInfo("Consider 4 byte...\n");
		offset += 4;
#endif

	/* Boot from flash */
#if defined(N_IMAGE_OTA_KERNEL_BOOT_OPT)
		VK_sprintf(pszAutoBootCmd, "boot %s:0x%x \"%s", bootdev, offset, bootopt);
#else
		VK_sprintf(pszAutoBootCmd, "boot %s:0x%x \"", bootdev, offset);
#endif

	if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s",pszAutoBootCmd,pContext->acMac2String);
	}

#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
	if(eDRV_BOOTOPTION_RSA_KEY & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s", pszAutoBootCmd, pContext->acRSA2String);
	}
#endif

	if(eDRV_BOOTOPTION_EXT_APP & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s", pszAutoBootCmd, pContext->stBootOpt.extAppName);
	}

	VK_sprintf(pszAutoBootCmd, "%s\"",pszAutoBootCmd);

	PrintInfo("%s\n", pszAutoBootCmd);

	res = VK_SYSTEM_Command(pszAutoBootCmd);
	if (res < 0)
	{
		PrintError("Cannot boot to ota app\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static int	local_bootloader_Selectboot_BootOTABackup(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HINT32 		res = -1;

#if defined(N_IMAGE_OTA_KERNEL_BACKUP_BOOT_DEVICE)
	const char *bootdev = N_IMAGE_OTA_KERNEL_BACKUP_BOOT_DEVICE;
	HUINT32 offset = N_IMAGE_OTA_KERNEL_BACKUP_OFFSET;
	const char *bootopt = N_IMAGE_OTA_KERNEL_BACKUP_BOOT_OPT;

#if defined(CONFIG_PROD_VTV200)
		PrintInfo("Consider 4 byte...\n");
		offset += 4;
#endif

	/* Boot from flash */
#if defined(N_IMAGE_OTA_KERNEL_BACKUP_BOOT_OPT)
		VK_sprintf(pszAutoBootCmd, "boot %s:0x%x \"%s", bootdev, offset, bootopt);
#else
		VK_sprintf(pszAutoBootCmd, "boot %s:0x%x \"", bootdev, offset);
#endif

	if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s",pszAutoBootCmd,pContext->acMac2String);
	}

#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
	if(eDRV_BOOTOPTION_RSA_KEY & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s", pszAutoBootCmd, pContext->acRSA2String);
	}
#endif

	if(eDRV_BOOTOPTION_EXT_APP & pContext->stBootOpt.eOption)
	{
		VK_sprintf(pszAutoBootCmd, "%s %s", pszAutoBootCmd, pContext->stBootOpt.extAppName);
	}

	VK_sprintf(pszAutoBootCmd, "%s\"",pszAutoBootCmd);

	PrintInfo("%s\n", pszAutoBootCmd);

	res = VK_SYSTEM_Command(pszAutoBootCmd);
	if (res < 0)
	{
		PrintError("Cannot boot to ota app(backup)\n");
	}
#endif /* N_IMAGE_OTA_KERNEL_BACKUP_BOOT_DEVICE */

	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

	return res;

}

static HERROR	local_bootloader_Selectboot_BootFactory_TryBoot_USB(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HINT32 		res = -1;
	HCHAR		env[LONG_BUFFER_SIZE] = {0, };
	HCHAR		szFacCmd[LONG_BUFFER_SIZE] = {0, };

	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

	PrintInfo("\n\n=====>> Try USB Factory Booting.. \n\n");

	/* Check USB Port */
	PrintInfo("Check USB Port..\n");
	res = BSU_open("usbdisk0");
	if(res != DRV_BLOADER_HANDLE_NULL)
	{
		BSU_close(res);
		res = BSU_getenv("FACTORY_MODEL", env, LONG_BUFFER_SIZE);
		if(res == DRV_OK && VK_strlen(env) != 0)
		{
			VK_sprintf(szFacCmd, "boot usbdisk0:%s_test.tet", env);
		}
		else
		{
			#if defined(N_IMAGE_FACTORYSW_BOOT_OPT)
				/* N_IMAGE_FACTORYSW_BOOT_OPT may be defined in flash_map.h */
				PrintInfo("N_IMAGE_FACTORYSW_BOOT_OPT(%s) is detected.\n", N_IMAGE_FACTORYSW_BOOT_OPT);
				VK_sprintf(szFacCmd, "boot usbdisk0:%s_test.tet \"%s\"", CONFIG_PRODUCT_NAME, N_IMAGE_FACTORYSW_BOOT_OPT);
			#else
				VK_sprintf(szFacCmd, "boot usbdisk0:%s_test.tet ", CONFIG_PRODUCT_NAME);
			#endif
		}
		res = VK_SYSTEM_Command(szFacCmd);
		if (res < 0)
		{
			PrintError("ERROR: cannot boot USB.(%d)\n", res);
		}
	}
	else
	{
		/* if usb cannot find, go prompt directly, no OTA Booting */
		PrintError("ERROR:NO USB Disk.\n\n");
		return ERR_FAIL;
	}

	if(res >= 0)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}


#if defined(CONFIG_FACTORY_TFTP)
static HERROR	local_bootloader_Selectboot_BootFactory_TryBoot_TFTP(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HINT32 		res = -1;
	HCHAR		env[LONG_BUFFER_SIZE] = {0, };

	HCHAR		szFacIfCmd[LONG_BUFFER_SIZE] = {0, };
	HCHAR		szFacMacAddr[MAX_MAC_STR_SIZE] = {0, };
	HCHAR		szFacKernel[LONG_BUFFER_SIZE] = {0, };
	HCHAR		szFacCmd[LONG_BUFFER_SIZE] = {0, };

	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

	PrintInfo("\n=====>> Try TFTP Factory Booting.. \n\n");
	PrintInfo("BOOT = NET\n");

	/* eth0 */
	res = BSU_getenv("FACTORY_IPADDR", env, LONG_BUFFER_SIZE);
	if(res == DRV_OK && VK_strlen(env) != 0)
		VK_sprintf(szFacIfCmd, "ifconfig eth0 -addr=%s", env);
	else
		VK_sprintf(szFacIfCmd, "ifconfig eth0 -addr=10.0.11.116", env);

	/* MacAddr */
	res = BSU_getenv("ETH0_HWADDR", env, LONG_BUFFER_SIZE);
	if(res == DRV_OK && VK_strlen(env) != 0)
		VK_sprintf(szFacMacAddr, "%s", env);
	else
		VK_sprintf(szFacMacAddr, "00:01:02:03:00:01");

#if defined(CONFIG_USB2ETH)
		VK_sprintf(szFacCmd, "%s", szFacIfCmd);
#else
		VK_sprintf(szFacCmd, "%s -hwaddr=%s", szFacIfCmd, szFacMacAddr);
#endif

	res = VK_SYSTEM_Command(szFacCmd);
	if (res < 0)
	{
		PrintError("ERROR:cannot set eth0.(%d)\n", res);
		return ERR_FAIL;
	}

	/* Kernel */
	res = BSU_getenv("FACTORY_SERVER", env, LONG_BUFFER_SIZE);
	if(res == DRV_OK && VK_strlen(env) != 0)
		VK_sprintf(szFacKernel, "%s:", env);
	else
		VK_sprintf(szFacKernel, "10.0.10.92:");

	res = BSU_getenv("FACTORY_MODEL", env, LONG_BUFFER_SIZE);
	if(res == DRV_OK && VK_strlen(env) != 0)
	{
		VK_sprintf(szFacKernel, "%s%s_test.tet", szFacKernel, env);
	}
	else
	{
		VK_sprintf(szFacKernel, "%s%s_test.tet", szFacKernel, CONFIG_PRODUCT_NAME);
	}

	PrintInfo("\n");
	VK_sprintf(szFacCmd, "boot %s", szFacKernel);
	PrintInfo("%s\n", szFacCmd);

	res = VK_SYSTEM_Command(szFacCmd);
	if (res < 0)
	{
		PrintError("ERROR: VK_SYSTEM_Command (%d)\n", res);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

/*
 * See also PAL_SYSTEM_BootUpOta() in pal_system.c
 * When enable to 'CONFIG_REBOOT_FOR_UPGRADER', Do rebooting before jump to upgrader
 * When enable to 'CONFIG_UOCTO_OTATYPE_IN_REGISTER', set ota type in scratch register, no NVRAM
 * For example, BSU-based Upgrader
*/
#if defined(CONFIG_FACTORY_LAUNCH_UPGRADER)
static HERROR	local_bootloader_Selectboot_BootFactory_TryBoot_OTA(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	int res = -1;

	DrvLOADER_DOWNLOAD_TYPE_e	eDnType = eDrvLOADER_DOWNLOAD_NONE;

	HAPPY(pCmdLine);
	HAPPY(pContext);
	HAPPY(pszAutoBootCmd);

	PrintInfo("\n\nERROR:cannot find booting method anymore, start OTA Booting..\n\n");
	PrintInfo("=====>> Try OTA Booting.. Not Support Yet\n\n");

	eDnType = eDrvLOADER_DOWNLOAD_BY_USB;
#if defined(CONFIG_UOCTO_OTATYPE_IN_REGISTER)
	if(DI_LOADER_SetScratch(DI_LOADER_SCRATCH_OTA, (HUINT32)eDnType) != DI_ERR_OK)
	{
		PrintError("[%s:%d] DI_LOADER_SetScratch(OTA_DN_TYPE) error\n", __FUNCTION__, __LINE__);
	}
#else
	if(DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, (HUINT8*)OTA_DN_TYPE, 0, &eDnType, sizeof(DrvLOADER_DOWNLOAD_TYPE_e)) != DI_ERR_OK)
	{
		PrintError("[%s:%d] DI_NVRAM_Write(OTA_DN_TYPE) error\n", __FUNCTION__, __LINE__);
	}
#endif

#if defined(CONFIG_UOCTO_REBOOT_FOR_UPGRADER)
	PrintInfo("Reboot for OTA\n");
	res = VK_SYSTEM_Command("reboot");
	if(res < 0)
	{
		PrintError("Reboot Fail. \n\n");
		return ERR_FAIL;
	}
#else
	VK_memset(pszAutoBootCmd, 0, LONG_BUFFER_SIZE);

	res = local_bootloader_Selectboot_BootOTA(pContext, pCmdLine, pszAutoBootCmd);
	if(res != DRV_OK)
	{
		PrintError("Booting Fail.\n\n");
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}
#endif

static int	local_bootloader_Selectboot_BootFactory(DRV_Bootloader_Context_t *pContext, cmdline_t *pCmdLine, HCHAR	*pszAutoBootCmd)
{
	HERROR		hErr = ERR_OK;

	DrvLOADER_DOWNLOAD_TYPE_e	eDnType = eDrvLOADER_DOWNLOAD_NONE;

	/* 0. Clear NVRAM OTA FLAG */
#if defined(CONFIG_UOCTO_OTATYPE_IN_REGISTER)
	if(DI_LOADER_SetScratch(DI_LOADER_SCRATCH_OTA, (HUINT32)eDnType) != DI_ERR_OK)
	{
		PrintError("[%s:%d] DI_LOADER_SetScratch(OTA_DN_TYPE) error\n", __FUNCTION__, __LINE__);
	}
#else
	if(DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, (HUINT8*)OTA_DN_TYPE, 0, &eDnType, sizeof(DrvLOADER_DOWNLOAD_TYPE_e)) != DI_ERR_OK)
	{
		PrintError("[%s:%d] DI_NVRAM_Write(OTA_DN_TYPE) error\n", __FUNCTION__, __LINE__);
	}
#endif

	PrintInfo("\n\n");
	PrintInfo("============================================================\n");
	PrintInfo("*														  *\n");
	PrintInfo("*			   [Loader 3.0] Factory Boot Mode			  *\n");
	PrintInfo("*														  *\n");
	PrintInfo("============================================================\n");


	/* 1. Try USB Booting */
	hErr = local_bootloader_Selectboot_BootFactory_TryBoot_USB(pContext, pCmdLine, pszAutoBootCmd);
	if(ERR_OK == hErr)
	{
		PrintInfo("USB Boot - Start -\r\n");
		return ERR_OK;
	}

#if defined(CONFIG_FACTORY_TFTP)
	/* 2. Try TFTP Booting */
	hErr = local_bootloader_Selectboot_BootFactory_TryBoot_TFTP(pContext, pCmdLine, pszAutoBootCmd);
	if(ERR_OK == hErr)
	{
		PrintInfo("TFTP Boot - Start -\r\n");
		return ERR_OK;
	}
#endif

#if defined(CONFIG_FACTORY_LAUNCH_UPGRADER)
	/* 3. Factory Fail, Try OTA Booting  */
	hErr = local_bootloader_Selectboot_BootFactory_TryBoot_OTA(pContext, pCmdLine, pszAutoBootCmd);
	if(ERR_OK == hErr)
	{
		PrintInfo("TFTP Boot - Start -\r\n");
		return ERR_OK;
	}
#endif

	return ERR_FAIL;
}


static void	local_bootloader_Selectboot_GetMac2Addr(DRV_Bootloader_Context_t *pContext)
{
	HINT32		res = -1;
	HCHAR		acMac2bin[MAX_MAC_BIN_SIZE] = {0, };

	pContext->bIsMac2Addr = TRUE;

	/* Get MAC2 address */
	res= DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, (HUINT8 *)MAC3_ADDR, 0, &acMac2bin[0], MAX_MAC_BIN_SIZE);
	if (res)
	{
		VK_snprintf(pContext->acMac2String, SHORT_BUFFER_SIZE, "mac3addr=00:01:02:03:FF:F0");
	}
	else
	{
		VK_snprintf(pContext->acMac2String, SHORT_BUFFER_SIZE, "mac3addr=%02X:%02X:%02X:%02X:%02X:%02X", acMac2bin[0],acMac2bin[1],acMac2bin[2],acMac2bin[3],acMac2bin[4],acMac2bin[5]);
	}
}

#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
static void	local_bootloader_Selectboot_GetSecureRegionRam(DRV_Bootloader_Context_t *pContext)
{
	HINT32		nIdx=0;

	pContext->bIsSecureRegionRam = TRUE;

	VK_memset(pContext->acRSA2String, 0x0, sizeof(pContext->acRSA2String));
	VK_memset(pContext->acRsakey, 0x0, sizeof(pContext->acRsakey));

	VK_memcpy(pContext->acRsakey, (void *)CONFIG_EXT_KEY_SECURE_REGION_OFFSET, RSAKEY_SIZE);
	VK_snprintf(pContext->acRSA2String, LONG_BUFFER_SIZE, "rsakey=");

	for(nIdx=0; nIdx<RSAKEY_SIZE; nIdx++)
	{
		VK_sprintf(pContext->acRSA2String, "%s%02X", pContext->acRSA2String, pContext->acRsakey[nIdx]);
	}
	PrintInfo("[RSA] cmdline(%d) : %s\n", VK_strlen(pContext->acRSA2String), pContext->acRSA2String);
}
#endif

static HERROR	local_bootloader_Selectboot(void *arg)
{
	HERROR		hErr = ERR_OK;
	HCHAR		szAutoBootCmd[LONG_BUFFER_SIZE];
	cmdline_t	cmd;

	DRV_Bootloader_Context_t *pContext = &s_stDrvBloaderContext;

	if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
	{
		local_bootloader_Selectboot_GetMac2Addr(pContext);
	}

#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
	if(eDRV_BOOTOPTION_RSA_KEY & pContext->stBootOpt.eOption)
	{
		local_bootloader_Selectboot_GetSecureRegionRam(pContext);
	}
#endif

	local_bloader_cmd_parse(&cmd, arg);

	if (local_cmd_sw_isset(&cmd, "-ram"))
	{
		hErr = local_bootloader_Selectboot_BootRam(pContext, &cmd, szAutoBootCmd);
		goto EXIT_FUNC;
	}
	if (local_cmd_sw_isset(&cmd, "-flash"))
	{
		hErr = local_bootloader_Selectboot_BootFlash(pContext, &cmd, szAutoBootCmd);
		goto EXIT_FUNC;
	}

	if (local_cmd_sw_isset(&cmd, "-2ndflash"))
	{
		hErr = local_bootloader_Selectboot_Boot2ndFlash(pContext, &cmd, szAutoBootCmd);
		goto EXIT_FUNC;
	}

	if (local_cmd_sw_isset(&cmd, "-ota"))
	{
		hErr = local_bootloader_Selectboot_BootOTA(pContext, &cmd, szAutoBootCmd);
		goto EXIT_FUNC;
	}

	if (local_cmd_sw_isset(&cmd, "-otabackup"))
	{
		hErr = local_bootloader_Selectboot_BootOTABackup(pContext, &cmd, szAutoBootCmd);
		goto EXIT_FUNC;
	}

	if (local_cmd_sw_isset(&cmd, "-factory"))
	{
		hErr = local_bootloader_Selectboot_BootFactory (pContext, &cmd, szAutoBootCmd);
		goto EXIT_FUNC;
	}

EXIT_FUNC:

	local_bloader_cmd_free(&cmd);

	return hErr;
}

#if defined(CONFIG_DEBUG)
static HERROR	local_bootloader_Selectboot_Default_BootNet(DRV_Bootloader_Context_t *pContext, void *arg)
{
	int res = -1;
	HCHAR		szAutoBootCmd[LONG_BUFFER_SIZE]= {0, };
	HCHAR		szIfCmd[LONG_BUFFER_SIZE]= {0, };
	HCHAR		env[LONG_BUFFER_SIZE] = {0, };
	HCHAR		envOpt[LONG_BUFFER_SIZE] = {0, };

	HAPPY(arg);

	res = BSU_getenv("IFCONFIG", env, LONG_BUFFER_SIZE);
	if (res !=DRV_OK || VK_strlen(env) == 0 )
		return ERR_FAIL;

#if defined(CONFIG_USB2ETH)
	VK_sprintf(szIfCmd, "%s", env);
#else
	{
		HCHAR	 	szMacAddr[MAX_MAC_STR_SIZE] = {0, };
		HERROR		hErr = ERR_OK;

		hErr = local_bootloader_GetNvramMacStr(szMacAddr, MAX_MAC_STR_SIZE);
		if(ERR_OK != hErr)
		{
			return hErr;
		}
		VK_sprintf(szIfCmd, "%s -hwaddr=%s", env, szMacAddr);
	}
#endif

	res = VK_SYSTEM_Command(szIfCmd);
	if (res < 0)
		return ERR_FAIL;

	res = BSU_getenv("KERNEL", env, LONG_BUFFER_SIZE);
	if (res !=DRV_OK || VK_strlen(env) == 0 )
		return ERR_FAIL;

	VK_memset(envOpt, 0x0, LONG_BUFFER_SIZE);
	res = BSU_getenv("KERNEL_OPT", envOpt, LONG_BUFFER_SIZE);
	if (res !=DRV_OK || VK_strlen(envOpt) == 0 )
		return ERR_FAIL;

	if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
	{
		VK_sprintf(szAutoBootCmd, "boot %s \"%s %s", env, envOpt, pContext->acMac2String);
	}
	else
	{
		VK_sprintf(szAutoBootCmd, "boot %s \"%s", env, envOpt);
	}

#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
		if(eDRV_BOOTOPTION_RSA_KEY & pContext->stBootOpt.eOption)
		{
			VK_sprintf(szAutoBootCmd, "%s %s", szAutoBootCmd, pContext->acRSA2String);
		}
#endif

	if(eDRV_BOOTOPTION_EXT_APP & pContext->stBootOpt.eOption)
	{
		VK_sprintf(szAutoBootCmd, "%s %s", szAutoBootCmd, pContext->stBootOpt.extAppName);
	}

	VK_sprintf(szAutoBootCmd, "%s\"",szAutoBootCmd);

	PrintInfo("%s\n", szAutoBootCmd);

	res = VK_SYSTEM_Command(szAutoBootCmd);
	if (res < 0)
	{
		PrintError("kernel loading error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

static HERROR	local_bootloader_Selectboot_Default_RecoveryMac(void *arg)
{
	int res = -1;

	DRV_Error	ret = ERR_FAIL;
	HERROR		hErr = ERR_OK;
	HCHAR	 	szMacAddr[MAX_MAC_STR_SIZE] = {0, };
	HCHAR		env[LONG_BUFFER_SIZE] = {0, };
	HCHAR		envOpt[LONG_BUFFER_SIZE] = {0, };

#if defined(CONFIG_DEBUG)
	HUINT8		macbin[32];
#endif

	HAPPY(arg);

	hErr = local_bootloader_GetNvramMacStr(szMacAddr, MAX_MAC_STR_SIZE);

#if defined(CONFIG_DEBUG)
	if(ERR_OK != hErr)
	{
		DI_ERR_CODE diErr = DI_ERR_OK;

		res = BSU_getenv("ETH0_HWADDR", env, LONG_BUFFER_SIZE);

		if(res==DRV_OK)
		{
			local_MAC_str2bin((HCHAR *)env, (HUINT8 *)macbin);
			DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)"MAC_ADDR", 0, macbin, MAX_MAC_BIN_SIZE);
		}
		else
		{
			ret = local_bootloader_Setenv("ETH0_HWADDR", DEFAULT_MAC, ENV_FLG_NORMAL);
			if(ERR_OK != ret)
			{
				PrintError("[%s:%d] error (%d)\r\n", __FUNCTION__, __LINE__, ret);
				return ret;
			}

			local_MAC_str2bin((HCHAR *)DEFAULT_MAC, (HUINT8 *)macbin);
			DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)"MAC_ADDR", 0, macbin, MAX_MAC_BIN_SIZE);
		}
	}
#else
	if(ERR_OK != hErr)
	{
		PrintError("DI_NVRAM_GetField Error");
		return ERR_FAIL;
	}
#endif
	else
	{
		/* check env : ETH0_HWADDR */
		res = BSU_getenv("ETH0_HWADDR", env, LONG_BUFFER_SIZE);
		if (res==DRV_OK && VK_strlen(env) > 0 )
		{
			PrintInfo("cfe or bolt [[%s]]\n",env);
			PrintInfo("nvram [[%s]]\n",szMacAddr);
			if (VK_strcmp(env, szMacAddr)!=0)
			{
				ret = local_bootloader_Setenv("ETH0_HWADDR", szMacAddr, ENV_FLG_NORMAL);
				if(ERR_OK != ret)
				{
					PrintError("[%s:%d] error (%d)\r\n", __FUNCTION__, __LINE__, ret);
					return ret;
				}

			}
		}
		else
		{
			ret = local_bootloader_Setenv("ETH0_HWADDR", szMacAddr, ENV_FLG_NORMAL);
			if(ERR_OK != ret)
			{
				PrintError("[%s:%d] error (%d)\r\n", __FUNCTION__, __LINE__, ret);
				return ret;
			}
		}
	}

	if(res < 0)
	{
		PrintError("Error : res(%d), ENV OPT(%s)", res, envOpt);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static int	local_bootloader_Selectboot_Default(void *arg)
{
	HINT32	 	res = 0;
	HERROR		hErr = ERR_OK;
	HCHAR		env[LONG_BUFFER_SIZE] = {0, };

	DRV_Bootloader_Context_t *pContext = &s_stDrvBloaderContext;

	hErr = local_bootloader_Selectboot_Default_RecoveryMac(arg);
	if(ERR_OK != hErr)
	{
		PrintError("RecoveryMac fail\r\n");
	}

	#if defined(CONFIG_DEBUG)
		if(eDRV_BOOTOPTION_CM_MACADDR & pContext->stBootOpt.eOption)
		{
			local_bootloader_Selectboot_GetMac2Addr(pContext);
		}

		#if defined(CONFIG_EXT_KEY_SECURE_REGION_RAM)
		if(eDRV_BOOTOPTION_RSA_KEY & pContext->stBootOpt.eOption)
		{
			local_bootloader_Selectboot_GetSecureRegionRam(pContext);
		}
		#endif

		VK_printf("arg %x (%s)\n",arg,arg);

		res = BSU_getenv("BOOT", env, LONG_BUFFER_SIZE);
		if (0 == res && VK_strlen(env))
		{
			PrintInfo("BOOT = %s\n", env);

			/* TFTP */
			if (VK_strcmp(env, "NET")==0)
			{
				hErr = local_bootloader_Selectboot_Default_BootNet(pContext, arg);
				goto FUNC_DONE;
			}
		}
	#endif

	if( arg == NULL )
	{
		hErr = local_bootloader_Selectboot("-flash");
		if (hErr != DRV_OK)
		{
			hErr = local_bootloader_Selectboot("-ota");
		}
	}
	else
	{
		hErr = local_bootloader_Selectboot(arg);
	}

FUNC_DONE:
	return hErr;
}

static int	local_cmd_Bootloader(void *arg)
{
	int		ret = 0;
	HCHAR *pszCmd = NULL;
	HCHAR *pszArg = NULL;

	pszArg = VK_MEM_Alloc(sizeof(HINT8) * 2048);
	if(NULL == pszArg)
	{
		PrintError("[%s:%d] out of memory\r\n", __FUNCTION__, __LINE__);
		return -1;
	}

	VK_memset(pszArg, 0, sizeof(HINT8) * 2048);
	pszCmd = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	while(pszCmd)
	{
		VK_strcat(pszArg, pszCmd);
		VK_strcat(pszArg, " ");

		pszCmd = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
		if(NULL == pszCmd)
		{
			break;
		}
	}

	PrintDebug("[%s:%d] Do Command : %s\r\n", __FUNCTION__, __LINE__, pszArg);
	ret = VK_SYSTEM_Command (pszArg);
	VK_MEM_Free(pszArg);

	return ret;
}

static int	local_cmd_Selectboot(void *arg)
{
	HERROR hErr = ERR_OK;

	hErr = local_bootloader_Selectboot(arg);
	if(ERR_OK != hErr)
	{
		PrintError("Selectboot Error");
	}

	return 0;
}



void local_cmd_normalCommand(void)
{
#if defined(CONFIG_ARM)
	CMD_RegisterGroup("BOLT", "BOLT", "BOLT commands");
	CMD_RegisterWord(local_cmd_Bootloader, "boltcmd", "bolt command","bolt");
#elif defined(CONFIG_MIPS)
	CMD_RegisterGroup("CFE", "CFE", "CFE commands");
	CMD_RegisterWord(local_cmd_Bootloader, "cfecmd", "cfe command","CFE");
#endif
	CMD_RegisterWord(local_cmd_Selectboot, "selectboot", "-flash,-2ndflash,-ota,-factory", "selectboot [-options]");
	CMD_ReleaseRegisterGroup();
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
DRV_Error DRV_BLOADER_Init(void)
{
	DRV_Error eRet = DRV_OK;

#if defined(CONFIG_DEBUG)
	HCHAR		szBoot[SHORT_BUFFER_SIZE] = {0, };
	HCHAR		szCmd[LONG_BUFFER_SIZE] = {0, };
#endif

	DRV_Bootloader_Context_t *pContext = &s_stDrvBloaderContext;

	VK_memset(pContext, 0, sizeof(DRV_Bootloader_Context_t));

	/* setting bolt/cfe environment */
	eRet = local_bloader_SetEnv();
	if(DRV_OK != eRet)
	{
		PrintError("[%s:%d] local_bloader_SetEnv error!\r\n", __FUNCTION__, __LINE__);
	}

#if defined(CONFIG_DEBUG) && defined(CONFIG_USB)
	eRet = BSU_getenv("BOOT", szBoot, SHORT_BUFFER_SIZE);
	if (eRet !=DRV_OK || szBoot[0] == 0 )
	{
		VK_sprintf(szCmd, "%s","waitusb -t=0 -d=\'USB Disk\' && batch usbdisk0:sysinit.txt");
		PrintInfo("[%s:%d] BOOT getevn : %s\n", __FUNCTION__, __LINE__, szCmd);
		VK_SYSTEM_Command(szCmd);
	}
#endif

	return DRV_OK;
}

DRV_Error DRV_BLOADER_SetBootOption(DRV_LOADER_BootOption_t	*pstOpt)
{
	HERROR eRet = DRV_OK;

	DRV_Bootloader_Context_t *pContext = &s_stDrvBloaderContext;

	VK_memcpy(&pContext->stBootOpt, pstOpt, sizeof(DRV_LOADER_BootOption_t));

	return eRet;
}

DRV_Error DRV_BLOADER_GetBootOption(DRV_LOADER_BootOption_t	*pstOpt)
{
	HERROR eRet = DRV_OK;

	*pstOpt = s_stDrvBloaderContext.stBootOpt;

	return eRet;
}

DRV_Error CMD_DRV_BLOADER_Init(void)
{
	DRV_Error eRet = DRV_OK;

	/* add command */
	local_cmd_normalCommand();

	/* add debug command */
#if defined(CONFIG_DEBUG)
	local_cmd_dbgCommand();
#endif

	return eRet;
}

void DRV_BLOADER_DeInit(void)
{

}

DRV_Error  DRV_BLOADER_set_envdevice(HCHAR *name)
{
	HINT32 nRet = -1;

	if(NULL == name)
	{
		PrintError("[%s:%d] null argument\r\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	nRet = BSU_set_envdevice(name);

	if(0 > nRet)
	{
		PrintError("[%s:%d] error (%d)\r\n", __FUNCTION__, __LINE__, nRet);
		return DRV_ERR_INVALID_PARAMETER;
	}

	return DRV_OK;
}

HINT32	DRV_BLOADER_Autobooting(DrvLOADER_IMAGE_Boot_e etImageBoot)
{
	HINT32 ret;

	switch(etImageBoot)
	{
		case eDrvLOADER_IMAGE_Boot_Factory:
			ret = local_bootloader_Selectboot((void*)"-factory");
			break;

		case eDrvLOADER_IMAGE_Boot_Ota:
			ret = local_bootloader_Selectboot((void*)"-ota");
			break;

		case eDrvLOADER_IMAGE_Boot_OtaBackup:
			ret = local_bootloader_Selectboot((void*)"-otabackup");
			break;

		case eDrvLOADER_IMAGE_Boot_NormalInFactory:
			ret = local_bootloader_Selectboot((void*)"-flash");
			break;

		default:
			ret = local_bootloader_Selectboot_Default((void*)NULL);
			break;
	}

	return ret;
}

HINT32	DRV_BLOADER_Ramboot(void *elfImageBuff, HUINT32 elfSize, DrvLOADER_IMAGE_Boot_e etImageBoot)
{
	HCHAR	szCmd[LONG_BUFFER_SIZE];
	HINT32	ret;


	switch(etImageBoot)
	{
		case eDrvLOADER_IMAGE_Boot_Factory:
			VK_snprintf(szCmd, 255, "-ram -bootaddr=0x%08x -bootsize=0x%08x", elfImageBuff, elfSize);
			ret = local_bootloader_Selectboot(szCmd);
			break;

		case eDrvLOADER_IMAGE_Boot_Ota:
		case eDrvLOADER_IMAGE_Boot_OtaBackup:
			VK_snprintf(szCmd, 255, "-ram -bootaddr=0x%08x -bootsize=0x%08x", elfImageBuff, elfSize);
			ret = local_bootloader_Selectboot(szCmd);
			break;

		case eDrvLOADER_IMAGE_Boot_NormalInFactory:
			VK_snprintf(szCmd, 255, "-ram -bootaddr=0x%08x -bootsize=0x%08x -normal", elfImageBuff, elfSize);
			ret = local_bootloader_Selectboot(szCmd);
			break;

		default:
			VK_snprintf(szCmd, 255, "-ram -bootaddr=0x%08x -bootsize=0x%08x -normal", elfImageBuff, elfSize);
			ret = local_bootloader_Selectboot_Default(szCmd);
			break;
	}

	return ret;
}

/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/
#if defined(CONFIG_DEBUG)
static HINT32 local_MAC_str2bin(HCHAR *srcptr, HUINT8 *dstptr)
{
	HCHAR c;
	HINT32 i;
	HINT32 m;
	HINT32 retval = 0;

	for (i = 0; i < 6; i++)
		dstptr[i] = 0;

	for (i = 0; i < 17; i++) {
		c = *srcptr++;

		m = (i + 1) % 3;

		/* Skip every 3rd char.  Just a separator. */
		if (!m)
			continue;

		if (c < '0' || c > 'f') {
			/* Invalid character */
			retval = 1;
			break;
		} else if (c < ':') {
			/* It's a digit */
			*dstptr |= c - '0';
		} else if (c >= 'a') {
			/* It's a lowercase hex digit */
			*dstptr |= c - 'a' + 10;
		} else if (c < 'A' || c > 'F') {
			/* Invalid character */
			retval = 1;
			break;
		} else {
			/* It's an uppercase hex digit */
			*dstptr |= c - 'A' + 10;
		}

		if (m == 1)
			*dstptr <<= 4;
		else
			dstptr++;
	}

	return retval;
}

static int	local_cmd_Setmac(void *arg)
{
	DRV_Error	ret = ERR_FAIL;
	HCHAR	*str;
	HCHAR	szMacAddr[128];
	HINT32	res;
	HUINT8	macbin[32];
	HUINT8	readmac[32];
	HCHAR	szDefault[LONG_BUFFER_SIZE];

	str = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	if (!str)
	{
		//ui_showusage(cmd);
		return DRV_ERR;
	}
	res = local_MAC_str2bin(str, macbin);
	if(res)
	{
		//ui_showusage(cmd);
		return DRV_ERR;
	}
	VK_strncpy(szMacAddr, str, 17);
	szMacAddr[17] = 0;

	ret = local_bootloader_Setenv("ETH0_HWADDR", szMacAddr, ENV_FLG_NORMAL);
	if(ERR_OK != ret)
	{
		PrintError("[%s:%d] error (%d)\r\n", __FUNCTION__, __LINE__, ret);
		return ret;
	}

	res = DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)"MAC_ADDR", 0, macbin, MAX_MAC_BIN_SIZE);
	if(res)
	{
		return res;
	}

 	res = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, (HUINT8*)"MAC_ADDR", 0, macbin, MAX_MAC_BIN_SIZE);
	if(res)
	{
		return res;
	}

	if( VK_memcmp(macbin, readmac, MAX_MAC_BIN_SIZE) != 0)
	{
		return DRV_ERR;
	}

	return res;
}

HINT32 local_xtoi(const HCHAR *dest)
{
	HINT32 x = 0;
	HINT32 digit;

	if ((*dest == '0') && (*(dest + 1) == 'x'))
		dest += 2;

	while (*dest) {
		if ((*dest >= '0') && (*dest <= '9'))
			digit = *dest - '0';
		else if ((*dest >= 'A') && (*dest <= 'F'))
			digit = 10 + *dest - 'A';
		else if ((*dest >= 'a') && (*dest <= 'f'))
			digit = 10 + *dest - 'a';
		else
			break;

		x *= 16;
		x += digit;
		dest++;
	}

	return x;
}

static int	local_cmd_Systemid(void *arg)
{
	HUINT8	aucSystemId[4];
	HCHAR	*str;
	HUINT32	err;
	HUINT32	systemId;

	str = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	if (!str)
	{
		err = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, (HUINT8*)"SYSTEM_ID", 0, aucSystemId, 4);
		if (err == 0)
			PrintInfo("system id %02X%02X%02X%02X\n", aucSystemId[0], aucSystemId[1], aucSystemId[2], aucSystemId[3]);
		else
			PrintError("no system id\n");
		goto DONE;
	}

	systemId = local_xtoi(str);
	put32bit(aucSystemId, systemId);
	PrintInfo("system id %02X%02X%02X%02X\n", aucSystemId[0], aucSystemId[1], aucSystemId[2], aucSystemId[3]);
	DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)"SYSTEM_ID", 0, aucSystemId, 4);

DONE :
	return 0;
}

const HUINT8 s_aes_key[] =
{
	0x48,0x4b,0x53,0x00,0x10,0x14,0x77,0x6e,
	0x5e,0x1c,0x78,0xde,0x83,0x0d,0x50,0x44,
	0xd3,0x55,0x94,0xd5,0x4d
};

const HUINT8 s_rsa_public_key_2048[] =
{
	0x48,0x4b,0x53,0x01,0x00,0xd0,0x41,0x6e,0xf4,0x26,0xb1,0xab,0xc1,0x14,0x9d,0x1c,
	0x96,0xbb,0xbc,0x45,0x9c,0x32,0x69,0x1d,0x2f,0x09,0xc7,0x0f,0x24,0x0d,0x7a,0xec,
	0xd9,0x2f,0xb7,0xf2,0xac,0xe7,0x22,0x27,0xfb,0xf8,0x31,0x3a,0xf3,0xcf,0x6c,0x27,
	0x1b,0x41,0xb0,0x5b,0xc7,0x1f,0xa5,0xf1,0x28,0x79,0x7f,0xc9,0x6c,0x9d,0x81,0xd0,
	0xbb,0x6c,0xa5,0xcc,0xba,0xeb,0x9b,0xfb,0x7f,0x14,0xf4,0xc2,0x29,0xd4,0x44,0xfa,
	0xad,0x79,0xe0,0x4d,0x07,0x92,0x18,0x7f,0x02,0xb0,0x8a,0xe5,0x2d,0xbc,0xb5,0x9a,
	0xfb,0x0a,0x37,0x97,0x5e,0x4c,0xbe,0x90,0xd6,0x8f,0xef,0x1c,0xab,0xb5,0x84,0xa9,
	0x0c,0x3b,0x31,0x83,0x37,0x72,0x01,0x59,0x61,0x7c,0x3f,0x68,0x15,0xbd,0x5e,0x5a,
	0x3f,0xf5,0x59,0xbd,0xed,0x05,0xa0,0x51,0x07,0xdf,0x8c,0x35,0x9a,0xae,0xd1,0x94,
	0xa7,0x8c,0x6d,0xf7,0x75,0x9b,0x94,0x20,0x70,0x3c,0x37,0x60,0x83,0xb5,0x46,0xa1,
	0x11,0x1b,0x68,0x65,0xa6,0x76,0xae,0x9b,0x7e,0x80,0x06,0xc8,0xb4,0xa9,0xa9,0x6f,
	0xe0,0xef,0x1b,0x63,0xd8,0xa8,0x2e,0xe5,0xda,0xad,0x33,0x82,0x2f,0xa7,0x22,0x34,
	0x30,0x88,0x7c,0x1d,0xde,0xef,0xa1,0x0a,0xb5,0xb7,0x72,0x68,0x27,0x75,0xc9,0x72,
	0x9a,0x80,0x4f,0xae,0xb6,0x85,0x11,0xf9,0x42,0x82,0xaf,0x02,0x90,0x2a,0x78,0xe5,
	0x4d,0x91,0xe7,0x07,0x2b,0xd6,0x63,0xf4,0x12,0x56,0x69,0x4d,0x3b,0x2d,0xcc,0x04,
	0x85,0x64,0x38,0xab,0x62,0x2d,0x5a,0xb9,0x06,0xd7,0x74,0x0c,0x0a,0xa4,0x99,0x48,
	0x51,0xf7,0x4f,0xfa,0xff
};

static int	local_cmd_Setkey(void *arg)
{
	UNUSED(arg);

	PrintInfo("setkey temporary OBSC_KEY\n");
	DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)"OBSC_KEY", 0, (HUINT8*)s_aes_key, sizeof(s_aes_key));
	PrintInfo("setkey temporary SIGN_KEY\n");
	DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)"SIGN_KEY", 0, (HUINT8*)s_rsa_public_key_2048, sizeof(s_rsa_public_key_2048));

	goto FUNC_DONE;

FUNC_DONE :
	return 0;
}

static int	local_cmd_Getmac(void *arg)
{
	HCHAR	 macval;
	HCHAR	 macbin[MAX_MAC_BIN_SIZE];
	HINT32	 i;
	UNUSED(arg);

	DI_NVRAM_GetField(DI_NVRAM_FIELD_MAC_ADDR, 0, macbin, MAX_MAC_BIN_SIZE);

	PrintInfo("mac_address: ");
	for(i=0 ;i<MAX_MAC_BIN_SIZE ;i++)
	{
		macval = macbin[i];
		PrintInfo("%02x",macval);
		if(i<MAX_MAC_BIN_SIZE-1) PrintInfo(":");
	}
	PrintInfo(" (in Hex)\n");

	return DRV_OK;
}

static HCHAR local_cmd_tolower(HCHAR c)
{
	return ((c >= 'A' && c <= 'Z') ? c + 32: c);
}

static void local_cmd_strlower(HCHAR *str)
{
    while (*str) {
	*str = local_cmd_tolower(*str);
	str++;
	}
}

static int	local_cmd_Autoenv(void *arg)
{
	HCHAR		szId[SHORT_BUFFER_SIZE];
	HCHAR		szIp[SHORT_BUFFER_SIZE];
	HCHAR		szModelName[SHORT_BUFFER_SIZE];
	HCHAR		szCmdAll[LONG_BUFFER_SIZE];
	HCHAR 		szMacTwoByteInput[SHORT_BUFFER_SIZE];
	HCHAR		szBoot[SHORT_BUFFER_SIZE];
	HCHAR		szDefault[SHORT_BUFFER_SIZE];

	HCHAR 		*pszIfConfig		= "setenv -p IFCONFIG \"ifconfig eth0 -auto\"";
	HCHAR 		*pszServer		= "setenv -p SERVER %s";
	HCHAR 		*pszId			= "setenv -p ID %s";
	HCHAR 		*pszKernel		= "setenv -p KERNEL \"%s:vmlinuz-%s-%s\"";
	HCHAR 		*pszKernelOpt		= "setenv -p KERNEL_OPT \"root=/dev/nfs nfsroot=%s:/nfsroot/%s/%s/root,tcp,nolock ip=:::::%s:dhcp rw\"";
	HCHAR 		*pszBoot			= "setenv -p BOOT %s";
	HCHAR 		*pszModelName	= "setenv -p MODEL_NAME %s";
//	HCHAR		*pszMacProg		= "setmac %s";
	HCHAR 		szMacAddr[MAX_MAC_STR_SIZE]	= {0, };

	HCHAR		*pszCmd = NULL;
	DRV_Error	err;

	DRV_Bootloader_Context_t *pContext = &s_stDrvBloaderContext;

	VK_memset(szDefault, 0, SHORT_BUFFER_SIZE);

	pszCmd = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	if( pszCmd == NULL)
	{
		szId[0] = 0;
		err = BSU_getenv("ID", szDefault, SHORT_BUFFER_SIZE);
		PrintInfo("ID - default(%s) : ", (szDefault[0] != '\0') ? szDefault : "break");
		BSU_gets(szId, SHORT_BUFFER_SIZE);
		if (VK_strlen(szId) == 0)
		{
			if (VK_strlen(szDefault) == 0)
			{
				PrintError("no id\n");
				return 1;
			}
			VK_strncpy(szId, szDefault, SHORT_BUFFER_SIZE);
		}
		PrintInfo("%s\n", szId);
	}
	else
	{
		VK_strcpy(szId, pszCmd);
	}

	pszCmd = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	if( pszCmd == NULL)
	{
		szIp[0] = 0;
		err = BSU_getenv("SERVER", szDefault, SHORT_BUFFER_SIZE);
		PrintInfo("SERVER IP - default(%s) : ", (szDefault[0] != '\0') ? szDefault : "break");
		BSU_gets( szIp, SHORT_BUFFER_SIZE);
		if (VK_strlen(szIp) == 0)
		{
			if (VK_strlen(szDefault) == 0)
			{
				PrintError("no ip\n");
				return 1;
			}
			VK_strncpy(szIp, szDefault, SHORT_BUFFER_SIZE);
		}
		PrintInfo("%s\n", szIp);
	}
	else
	{
		VK_strcpy(szIp, pszCmd);
	}

	pszCmd = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	if( pszCmd == NULL)
	{
		szModelName[0] = 0;
		err = BSU_getenv("MODEL_NAME", szDefault, SHORT_BUFFER_SIZE);
		PrintInfo("MODEL_NAME - default(%s) : ", (szDefault[0] != '\0') ? szDefault : "break");
		BSU_gets( szModelName, SHORT_BUFFER_SIZE);
		if (VK_strlen(szModelName) == 0)
		{
			if (VK_strlen(szDefault) == 0)
			{
				PrintError("no MODEL_NAME\n");
				return 1;
			}
			VK_strncpy(szModelName, szDefault, SHORT_BUFFER_SIZE);
		}
		PrintInfo("%s\n", szModelName);
	}
	else
	{
		VK_strcpy(szModelName, pszCmd);
	}

	pszCmd = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	if( pszCmd == NULL)
	{
		szMacTwoByteInput[0] = 0;
		err = BSU_getenv("ETH0_HWADDR", szDefault, SHORT_BUFFER_SIZE);
		if (szDefault[0] != '\0')
			PrintInfo("END of MAC Address [00:01:02:03:XX:XX] \n");
		else
			PrintInfo("END of MAC Address [00:01:02:03:XX:XX] - default(%s) : ", "break");
		BSU_gets(szMacTwoByteInput, SHORT_BUFFER_SIZE);

		if (VK_strlen(szMacTwoByteInput) == 0)
		{
			if (VK_strlen(szDefault) == 0)
			{
				PrintError("no mac\n");
				return 1;
			}
			VK_sprintf(szMacAddr,"%s",szDefault);
		}
		else if (VK_strlen(szMacTwoByteInput) == 17)
		{
			VK_sprintf(szMacAddr, "%s", szMacTwoByteInput);
		}
		else
		{
			VK_sprintf(szMacAddr, "00:01:02:03:%s", szMacTwoByteInput);
		}
		PrintInfo("%s\n", szMacAddr);
	}
	else
	{
		VK_strcpy(szMacAddr, pszCmd);
	}

	pszCmd = (HCHAR *)CMD_NextArg ((HCHAR **)&arg);
	if( pszCmd == NULL)
	{
		szBoot[0] = 0;
		err = BSU_getenv("BOOT", szDefault, SHORT_BUFFER_SIZE);
		PrintInfo("BOOT - default(%s) : ", (szDefault[0] != '\0') ? szDefault : "NET");
		BSU_gets( szBoot, SHORT_BUFFER_SIZE);
		if (VK_strlen(szBoot) == 0)
		{
			if (VK_strlen(szDefault) == 0)
				VK_strncpy(szBoot, "NET", SHORT_BUFFER_SIZE);
			else
				VK_strncpy(szBoot, szDefault, SHORT_BUFFER_SIZE);
		}
		PrintInfo("%s\n", szBoot);
	}
	else
	{
		VK_strcpy(szBoot, pszCmd);
	}

	local_cmd_strlower(szModelName);
	VK_sprintf(szCmdAll, pszIfConfig);
	PrintInfo("%s\n", szCmdAll);

	VK_SYSTEM_Command(szCmdAll);

	if (VK_strlen(szId) != 0 && VK_strlen(szIp) != 0 && VK_strlen(szModelName) != 0)
	{
		VK_sprintf(szCmdAll, pszKernel, szIp, szId, szModelName);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);

		VK_sprintf(szCmdAll, pszModelName, szModelName);

		VK_SYSTEM_Command(szCmdAll);

		VK_sprintf(szCmdAll, pszServer, szIp);

		VK_SYSTEM_Command(szCmdAll);

		VK_sprintf(szCmdAll, pszId, szId);

		VK_SYSTEM_Command(szCmdAll);
	}

	if (VK_strlen(szId) != 0 && VK_strlen(szIp) != 0 && VK_strlen(szModelName) != 0)
	{
		VK_sprintf(szCmdAll, pszKernelOpt, szIp, szId, szModelName, pContext->stBootOpt.ethernetName);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}

	VK_sprintf(szCmdAll, pszBoot, szBoot);
	PrintInfo("%s\n", szCmdAll);

	VK_SYSTEM_Command(szCmdAll);

	if (VK_strlen(szMacAddr) != 0 && VK_strlen(szMacTwoByteInput)!= 0)
	{
		PrintInfo("Written MAC to flash %s \n", szMacAddr);

		local_cmd_Setmac(szMacAddr);
	}

	return DRV_OK;
}

static int	local_cmd_Autoflash(void *arg)
{
	HINT32 		res = 0;
	HERROR		hErr = ERR_OK;
	HCHAR		szModelName[SHORT_BUFFER_SIZE];
	HCHAR		szAutoFlashOpt[SHORT_BUFFER_SIZE];
	HCHAR		szCmdAll[LONG_BUFFER_SIZE];
	HCHAR 		szIp[SHORT_BUFFER_SIZE];
	HCHAR 		szId[SHORT_BUFFER_SIZE];
	HINT32		allFlash = 0, loaderFlash = 0;
	HCHAR 		szPath[SHORT_BUFFER_SIZE];
	HCHAR 		szVersion[SHORT_BUFFER_SIZE];
	HCHAR		szIfCmd[LONG_BUFFER_SIZE];
	HCHAR 		szMacAddr[MAX_MAC_STR_SIZE];
	cmdline_t		cmd;

#if defined(CONFIG_SERIAL_NOR_FLASH)
	VK_SYSTEM_Command("gbulk");
#endif

	res = BSU_getenv("IFCONFIG", szCmdAll, LONG_BUFFER_SIZE);
	if (res != DRV_OK)
		return DRV_ERR;

	hErr = local_bootloader_GetNvramMacStr(szMacAddr, MAX_MAC_STR_SIZE);
	if(ERR_OK != hErr)
	{
		return DRV_ERR;
	}
	VK_sprintf(szIfCmd, "%s -hwaddr=%s", szCmdAll, szMacAddr);

	res = VK_SYSTEM_Command(szIfCmd);
	if (res < 0)
		return DRV_ERR;

	local_bloader_cmd_parse(&cmd, arg);

	res = BSU_getenv("SERVER", szIp, SHORT_BUFFER_SIZE);
	if (res != DRV_OK)
		goto EXIT_FUNC;

	res = BSU_getenv("ID", szId, SHORT_BUFFER_SIZE);
	if (res != DRV_OK)
		goto EXIT_FUNC;

	if (local_cmd_sw_isset(&cmd, "-model"))
	{
		HCHAR		*pszModel = NULL;

		local_cmd_sw_value(&cmd, "-model", &pszModel);
		if( pszModel != NULL )
		{
			VK_sprintf(szModelName, "%s", pszModel);
		}
		if( local_cmd_sw_name(&cmd, 1) == NULL)
			loaderFlash = 1;
	}
	else
	{
		res = BSU_getenv("MODEL_NAME", szModelName, SHORT_BUFFER_SIZE);
		if (res != DRV_OK)
			goto EXIT_FUNC;
	}

	res = BSU_getenv("AUTOFLASH_OPT", szAutoFlashOpt, SHORT_BUFFER_SIZE);
	if (res != DRV_OK && (local_cmd_sw_name(&cmd, 0) == NULL || loaderFlash))
		VK_sprintf(szAutoFlashOpt, "-bootlauncher");

	if (local_cmd_sw_isset(&cmd, "-all"))
	{
		allFlash = 1;
	}
	szPath[0] = 0;
	szVersion[0] = 0;
	if (local_cmd_sw_isset(&cmd, "-release"))
	{
		HCHAR 	*pszVersionString;
		HCHAR		*pszServer = NULL;

		allFlash = 1;
		local_cmd_sw_value(&cmd, "-server", &pszServer);
		VK_sprintf(szIp, "%s", pszServer ? pszServer : "10.0.11.50");
		VK_sprintf(szId, "%s", "release");
		VK_sprintf(szPath, "%s%s%s", "release/", szModelName, "/");
		if (local_cmd_sw_value(&cmd, "-release", (void *)&pszVersionString))
		{
			if (pszVersionString && VK_strlen(pszVersionString)!=0 && VK_strcmp(pszVersionString, "latest"))
			{
				VK_sprintf(szVersion, "%s%s", pszVersionString, "/");
			}
		}
	}
	if (allFlash || VK_strstr(szAutoFlashOpt, "-bootlauncher") || local_cmd_sw_isset(&cmd, "-bootlauncher"))
	{
		HCHAR *pszFlashBootName = NULL;
		pszFlashBootName = N_IMAGE_LAUNCHER_DEVICE;
		VK_sprintf(szCmdAll, "flash %s:%s%sbootpackage-%s-%s.bin %s", szIp, szPath, szVersion, szId, szModelName, pszFlashBootName);
		PrintInfo("%s\n", szCmdAll);
		VK_SYSTEM_Command(szCmdAll);
	}


	if (VK_strstr(szAutoFlashOpt, "-deletelauncher") || local_cmd_sw_isset(&cmd, "-deletelauncher"))
	{
		HCHAR *pszFlashBootName = NULL;
		pszFlashBootName = N_IMAGE_LAUNCHER_DEVICE;
		VK_sprintf(szCmdAll, "flash -blockerase -offset=0x%x %s", N_BSU_OFFSET, pszFlashBootName);
		PrintInfo("%s\n", szCmdAll);
		VK_SYSTEM_Command(szCmdAll);
	}

	if (VK_strstr(szAutoFlashOpt, "-launcher") || local_cmd_sw_isset(&cmd, "-launcher"))
	{
		HCHAR *pszFlashBootName = NULL;
		pszFlashBootName = N_IMAGE_LAUNCHER_DEVICE;
		VK_sprintf(szCmdAll, "flash %s:%s%sbsu-%s-%s.bin.gz -offset=0x%x %s", szIp, szPath, szVersion, szId, szModelName, N_BSU_OFFSET, pszFlashBootName);
		PrintInfo("%s\n", szCmdAll);
		VK_SYSTEM_Command(szCmdAll);
	}

	if (VK_strstr(szAutoFlashOpt, "-loader") || local_cmd_sw_isset(&cmd, "-loader"))
	{
		HCHAR *pszFlashBootName = NULL;
#if defined(BOLT_BSU)
		pszFlashBootName = N_IMAGE_LAUNCHER_DEVICE;
#else
		pszFlashBootName = "flash0.boot";
#endif
		VK_sprintf(szCmdAll, "flash %s:%s%sloader-%s-%s.bin %s", szIp, szPath, szVersion, szId, szModelName, pszFlashBootName);
		PrintInfo("%s\n", szCmdAll);
		VK_SYSTEM_Command(szCmdAll);
	}

	if (VK_strstr(szAutoFlashOpt, "-splash") || local_cmd_sw_isset(&cmd, "-splash"))
	{
		HUINT8			*pSplashBuf;
		HINT32			readLen;

		pSplashBuf = VK_MEM_Alloc(256*1024);
		if (pSplashBuf != NULL)
		{
			VK_memset(pSplashBuf, 0x0, 256*1024);
			VK_sprintf(szCmdAll, "load \"%s:%s%ssplash-%s-%s.jpg\" -tftp -raw -addr=0x%x -max=%d", szIp, szPath, szVersion, szId, szModelName, pSplashBuf, 256*1024);
			readLen = VK_SYSTEM_Command(szCmdAll);
			if( readLen > 0 )
			{
				res = DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)SFLASH, 0, pSplashBuf, readLen);
			}

			VK_MEM_Free(pSplashBuf);

			if ( res != DRV_OK )
			{
				goto EXIT_FUNC;
			}
		}
	}

#if defined(CONFIG_UBI) && !defined(CONFIG_EMMC_FLASH)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-rootfs") || local_cmd_sw_isset(&cmd, "-rootfs"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%sroot-%s-%s.squa ubi0.%s", N_UBI_ROOT_FS_OFFSET, szIp, szPath, szVersion, szId, szModelName, NAND_FLASH_UBI_DEVICE_NAME);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
	if (allFlash || VK_strstr(szAutoFlashOpt, "-kernel") || local_cmd_sw_isset(&cmd, "-kernel"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%svmlinuz-%s-%s ubi0.%s", N_UBI_KERNEL_OFFSET, szIp, szPath, szVersion, szId, szModelName, NAND_FLASH_UBI_DEVICE_NAME);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#if defined(N_UBI_2ND_ROOT_FS_OFFSET)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-2ndrootfs") || local_cmd_sw_isset(&cmd, "-2ndrootfs"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%sroot-%s-%s.squa ubi0.%s", N_UBI_2ND_ROOT_FS_OFFSET, szIp, szPath, szVersion, szId, szModelName, NAND_FLASH_UBI_DEVICE_NAME);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#endif
 #if defined(N_UBI_2ND_KERNEL_OFFSET)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-2ndkernel") || local_cmd_sw_isset(&cmd, "-2ndkernel"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%svmlinuz-%s-%s ubi0.%s", N_UBI_2ND_KERNEL_OFFSET, szIp, szPath, szVersion, szId, szModelName, NAND_FLASH_UBI_DEVICE_NAME);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#endif
#if defined(N_UBI_OTA_KERNEL_OFFSET)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-ota") || local_cmd_sw_isset(&cmd, "-ota"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%sotavmlinuz-%s-%s ubi0.%s", N_UBI_OTA_KERNEL_OFFSET, szIp, szPath, szVersion, szId, szModelName, NAND_FLASH_UBI_DEVICE_NAME);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#endif
#elif defined(CONFIG_EMMC_FLASH)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-rootfs") || local_cmd_sw_isset(&cmd, "-rootfs"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%sroot-%s-%s.squa %s", N_IMAGE_ROOTFS_OFFSET, szIp, szPath, szVersion, szId, szModelName, N_IMAGE_KERNEL_BOOT_DEVICE);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
	if (allFlash || VK_strstr(szAutoFlashOpt, "-kernel") || local_cmd_sw_isset(&cmd, "-kernel"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%svmlinuz-%s-%s %s", N_IMAGE_KERNEL_OFFSET, szIp, szPath, szVersion, szId, szModelName, N_IMAGE_KERNEL_BOOT_DEVICE);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#if defined(N_IMAGE_2ND_KERNEL_OFFSET)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-2ndrootfs") || local_cmd_sw_isset(&cmd, "-2ndrootfs"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%sroot-%s-%s.squa %s", N_IMAGE_ROOTFS_OFFSET, szIp, szPath, szVersion, szId, szModelName, N_IMAGE_2ND_KERNEL_BOOT_DEVICE);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#endif
 #if defined(N_IMAGE_2ND_KERNEL_OFFSET)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-2ndkernel") || local_cmd_sw_isset(&cmd, "-2ndkernel"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%svmlinuz-%s-%s %s", N_IMAGE_2ND_KERNEL_OFFSET, szIp, szPath, szVersion, szId, szModelName, N_IMAGE_2ND_KERNEL_BOOT_DEVICE);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#endif
#if defined(N_IMAGE_OTA_KERNEL_OFFSET)
	if (allFlash || VK_strstr(szAutoFlashOpt, "-ota") || local_cmd_sw_isset(&cmd, "-ota"))
	{
		VK_sprintf(szCmdAll, "flash -offset=0x%x -noerase %s:%s%sotavmlinuz-%s-%s %s", N_IMAGE_OTA_KERNEL_OFFSET, szIp, szPath, szVersion, szId, szModelName, N_IMAGE_OTA_KERNEL_BOOT_DEVICE);
		PrintInfo("%s\n", szCmdAll);

		VK_SYSTEM_Command(szCmdAll);
	}
#endif
#endif
	local_bloader_cmd_free(&cmd);

	VK_TASK_Sleep(50);
	res = VK_SYSTEM_Command("reboot");
	if (res < 0)
		return DRV_ERR;

	return DRV_OK;

EXIT_FUNC:
	local_bloader_cmd_free(&cmd);
	return DRV_ERR;
}

static void local_cmd_dbgCommand(void)
{
#if defined(CONFIG_ARM)
	CMD_RegisterGroup("BOLT", "BOLT", "BOLT commands");
#elif defined(CONFIG_MIPS)
	CMD_RegisterGroup("CFE", "CFE", "CFE commands");
#endif
	CMD_RegisterWord(local_cmd_Autoenv, "autoenv", "autoenv","autoenv");
	CMD_RegisterWord(local_cmd_Setmac, "setmac", "Program the MAC address.","setmac 00:60:6E:70:0D:A2");
	CMD_RegisterWord(local_cmd_Getmac, "getmac", "read MAC address from flash.","getmac");
	CMD_RegisterWord(local_cmd_Autoflash, "autoflash", "-bootlauncher, -loader, -launcher, -[2nd]kernel, -[2nd]rootfs, -ota, -splash, -all, -server=*, -release=*, -deletelauncher",
										"autoflash [-options]\n"
										"-bootlauncher : bolt/cfe + bsu write \n -loader : bolt/cfe write \n"
										"-launcher : bsu write\n-kernel : 1st area kernel write\n-roofs : 1st area rootfs write\n"
										"-2ndkernel : 2nd area kernel write\n-2ndroofs : 2nd area rootfs write\n"
										"-ota : upgrader write\n-splash : splash write\n-all : write all components\n"
										"-model : Instead of using MODEL_NAME\n"
										"-server=* : server ip for release burning\n-release=* : write release images with version numver, ex) autoflash -release=1.00.00");
	CMD_RegisterWord(local_cmd_Systemid, "systemid", "set systemid","systemid 12345678");
	CMD_RegisterWord(local_cmd_Setkey, "setkey", "setkey","setkey");
	CMD_ReleaseRegisterGroup();
}
#endif

