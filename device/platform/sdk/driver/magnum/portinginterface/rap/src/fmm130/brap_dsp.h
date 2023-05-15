/***************************************************************************
*     Copyright (c) 2004, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dsp.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 1/10/05 8:22p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_dsp.h $
* 
* Hydra_Software_Devel/1   1/10/05 8:22p nitinb
* PR 13082: Moved files to new directory structure
* 
* Hydra_Software_Devel/3   12/9/04 7:35p nitinb
* PR 13082: Implemented DSP PI design review comments
* 
* Hydra_Software_Devel/2   10/27/04 3:31p nitinb
* PR 13082: Adding code
* 
* Hydra_Software_Devel/1   10/26/04 4:40a nitinb
* PR13082: Initial version
* 
***************************************************************************/


/*=************************ Module Overview ********************************
The audio DSP is one of the object types of BRAP PI architecture. Each DSP
object is referred to as DSP channel. Physically each DSP object is
the actual DSP hardware processor.

The audio dsp channel API(BRAP_DSP_P/BRAP_DSP) is part of the BRAP PI
implementaton. This module manages a set of  audio DSP channels. These APIs 
deal with the operations carried out on DSP channel (like DSP booting etc).

Each DSP channel is capable of running multiple DSP contexts. Prototypes
of DSP context APIs (BRAP_DSPCHN_P/BRAP_DSPCHN) are in baud_dspchn.h. 
The various DSP contexts are decode, sample rate conversion (SRC), 
pass through etc.

DSP channel APIs (BRAP_DSP_P/BRAP_DSP) and DSP context APIs 
(BRAP_DSPCHN_P/BRAP_DSPCHN) are used by audio manager's decode channel 
APIs (BRAP_DEC).

Design
The design of this module has been broken down into the following sections:

o Part 1

  APIs that are not exposed to the application. These APIs have prefix
  BRAP_DSP_P. These APIs are used by audio manager's decode channel APIs 
  only.

o Part 2

  APIs exposed to the application. These APIs have prefix BRAP_DSP. They
  are implemented to expose DSP object's hardware features to application.
  They are only for unforeseen cases and are provided on need basis.
  These APIs are used by audio manager's decode channel APIs as well as
  by application.
***************************************************************************/

#ifndef BRAP_DSP_H__
#define BRAP_DSP_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	This structure contains DSP settings.

***************************************************************************/
typedef struct BRAP_DSP_Settings
{
	uint32_t	TBD;
} BRAP_DSP_Settings;



#ifdef __cplusplus
}
#endif

#endif /* BRAP_DSP_H__ */
/* End of File */

