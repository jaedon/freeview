/****************************************************************************
*
* File Name   : mgr_bml.c
*
* Description : AP module initialization application
*
*
* Revision History	:
*
* Date			Modification							Name
* -----------	-------------------------------------	------------------
* 2009/01/14	created									wjmoh
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */

#include <htype.h>
#include <hlib.h>
#include <octo_common.h>

#include <mgr_common.h>


/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Types.															*/
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

HERROR MGR_BML_StartBrowserApp (void)
{
	HERROR		hErr = ERR_FAIL;
#if defined(CONFIG_3RD_BML)
	hErr = pmgr_bml_StartBrowserApp();
#endif
	return hErr;
}

HERROR MGR_BML_Start (void)
{
#if defined(CONFIG_3RD_BML)
	return pmgr_bml_Start();
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_BML_Stop (void)
{
#if defined(CONFIG_3RD_BML)
	return pmgr_bml_Stop();
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_BML_Show (void)
{
#if defined(CONFIG_3RD_BML)
	return pmgr_bml_Show();
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_BML_Hide (void)
{
#if defined(CONFIG_3RD_BML)
	return pmgr_bml_Hide();
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_BML_SetKeyControlMode (HBOOL bKeyControlMode)
{
#if defined(CONFIG_3RD_BML)
	return pmgr_bml_SetKeyControlMode(bKeyControlMode);
#else
	return ERR_FAIL;
#endif
}


/* End of File. ---------------------------------------------------------- */


