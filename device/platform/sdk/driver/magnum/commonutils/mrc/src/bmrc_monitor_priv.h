/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmrc_monitor_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 2/29/12 7:19p $
 *
 * Module Description:
 *
 * Implementation of the Realtime Memory Monitor for 7038
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mrc/7420/bmrc_monitor_priv.h $
 * 
 * Hydra_Software_Devel/11   2/29/12 7:19p albertl
 * SW7344-37: Fixed some masks for SCB 4.2.
 * 
 * Hydra_Software_Devel/10   3/22/11 3:50p albertl
 * SW7344-37: Added changes for SCB Protocal 4.2.  Fixed transfer size
 * message.
 * 
 * Hydra_Software_Devel/9   3/28/10 4:04p albertl
 * SW7405-4059:  Fixed missing MAX defines.
 * 
 * Hydra_Software_Devel/8   3/26/10 1:32p albertl
 * SW7405-4059: Updated debug messages to be more helpful.  Included link
 * to SCB specs documentation, also available in this JIRA.
 * 
 * Hydra_Software_Devel/6   2/25/08 7:04p albertl
 * PR36876:  Rewrote MRC to abstract client names and streamline adding of
 * future chips.  3548 support added.
 * 
 * Hydra_Software_Devel/5   5/1/07 5:08p albertl
 * PR29633:  Updated and changed MRC for 7405.
 * 
 * Hydra_Software_Devel/4   2/7/07 7:39p albertl
 * PR25898:  Changed BMRC_MONITOR range checking to JWord (256 byte)
 * granularity to eliminate false violations from clients with JWord
 * minimum access sizes.
 * 
 * Hydra_Software_Devel/3   3/7/06 6:36p albertl
 * PR19498:  Added SCB Command decoding in error messages, and NMB is
 * printed only when relevant.  Checker ranges no longer cover extra
 * bytes that exceed 8 byte size alignment.
 * 
 * Hydra_Software_Devel/2   3/1/06 5:24p albertl
 * PR18701:  Renamed structs to use private names.  Changed
 * BMRC_P_Monitor_FileClientInfoto use char pointer instead of array.
 * 
 * Hydra_Software_Devel/1   2/16/06 7:43p albertl
 * PR19101:  Created typedefs for structs in bmrc_monitor_clients.c.
 * 
 ***************************************************************************/
#ifndef BMRC_MONITOR_PRIV_H_
#define BMRC_MONITOR_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Make this detect version at runtime */
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7435) || (BCHP_CHIP==7325) || (BCHP_CHIP==7344)
#define BMRC_MONITOR_P_SCB_PROTOCOL_VER 0x42
#else
#define BMRC_MONITOR_P_SCB_PROTOCOL_VER 0x01
#endif

/* SCB Protocol Specifications the following are derived from are available at
   http://www.blr.broadcom.com/projects/DVT_BLR/Memc_Arch/.  */
typedef enum 
{
	BMRC_P_Monitor_ScbCommand_eLR = 1,
	BMRC_P_Monitor_ScbCommand_eLW = 2,
	BMRC_P_Monitor_ScbCommand_eREF = 3,
	BMRC_P_Monitor_ScbCommand_eMRS = 4,
	BMRC_P_Monitor_ScbCommand_eEMRS = 5,
	BMRC_P_Monitor_ScbCommand_ePALL = 6,
	BMRC_P_Monitor_ScbCommand_eDR = 7,
	BMRC_P_Monitor_ScbCommand_eDW = 8,
	BMRC_P_Monitor_ScbCommand_eMR = 9,
	BMRC_P_Monitor_ScbCommand_eMW = 10,
	BMRC_P_Monitor_ScbCommand_eCR4 = 11,
	BMRC_P_Monitor_ScbCommand_eCR8 = 12,
	BMRC_P_Monitor_ScbCommand_eUnknown = 0
}BMRC_P_Monitor_ScbCommand;


typedef struct BMRC_P_Monitor_FileClientInfo
{
	const char *prefix;
	const BMRC_Client *clients;
} BMRC_P_Monitor_FileClientInfo;

typedef struct BMRC_P_Monitor_ScbCommandInfo {
	BMRC_P_Monitor_ScbCommand eScbCommand;
	uint32_t ulCommand;
	uint32_t ulMask;
	char *pName;
}BMRC_P_Monitor_ScbCommandInfo;

#define BMRC_P_MONITOR_SCB_MEM_ACCESS_ALIGN      ~0x000000FF /* some clients have J-word min access length */
#define BMRC_P_MONITOR_SCB_CHECKER_SIZE_MASK     ~0x00000007
#define BMRC_P_MONITOR_CHECKER_ADDR_ALIGN         BMRC_P_MONITOR_SCB_MEM_ACCESS_ALIGN


/* SCB defines */

#if (BMRC_MONITOR_P_SCB_PROTOCOL_VER >= 0x42)
#define BMRC_P_MONITOR_SCB_TRANSFER_ACCESS_MASK   0x1E0
#define BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MASK     0x01F
#else
#define BMRC_P_MONITOR_SCB_TRANSFER_ACCESS_MASK   0x1F0
#define BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MASK     0x00F
#endif

#define BMRC_P_MONITOR_SCB_INTERNAL_MASK          0x1FF
#define BMRC_P_MONITOR_SCB_MPEG_BLOCK_ACCESS_MASK 0x180
#define BMRC_P_MONITOR_SCB_CACHE_ACCESS_MASK      0x1FF

#define BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MAX      (BMRC_P_MONITOR_SCB_TRANSFER_SIZE_MASK + 1)

/* SCB MPEG block command fields */
#define BMRC_P_MONITOR_SCB_MPEG_X_BIT             0x080
#define BMRC_P_MONITOR_SCB_MPEG_Y_MASK            0x03E
#define BMRC_P_MONITOR_SCB_MPEG_T_BIT             0x001

#define BMRC_P_MONITOR_SCB_YLINES_MAX             (64)

#if (BMRC_MONITOR_P_SCB_PROTOCOL_VER >= 0x42)
#define BMRC_P_MONITOR_SCB_COMMAND_LR   0x000
#define BMRC_P_MONITOR_SCB_COMMAND_LW   0x020
#define BMRC_P_MONITOR_SCB_COMMAND_DR   0x180
#define BMRC_P_MONITOR_SCB_COMMAND_DW   0x1A0
#define BMRC_P_MONITOR_SCB_COMMAND_CR4  0x044
#define BMRC_P_MONITOR_SCB_COMMAND_CR8  0x048

#else
#define BMRC_P_MONITOR_SCB_COMMAND_LR   0x000
#define BMRC_P_MONITOR_SCB_COMMAND_LW   0x010
#define BMRC_P_MONITOR_SCB_COMMAND_DR   0x060
#define BMRC_P_MONITOR_SCB_COMMAND_DW   0x070
#define BMRC_P_MONITOR_SCB_COMMAND_CR4  0x024
#define BMRC_P_MONITOR_SCB_COMMAND_CR8  0x028
#endif

#define BMRC_P_MONITOR_SCB_COMMAND_REF  0x05C
#define BMRC_P_MONITOR_SCB_COMMAND_MRS  0x05D
#define BMRC_P_MONITOR_SCB_COMMAND_EMRS 0x05E
#define BMRC_P_MONITOR_SCB_COMMAND_PALL 0x05F

#define BMRC_P_MONITOR_SCB_COMMAND_MR   0x080
#define BMRC_P_MONITOR_SCB_COMMAND_MW   0x100


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BMRC_MONITOR_PRIV_H_ */

/* End of File */
