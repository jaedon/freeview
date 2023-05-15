
 /*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : bcmUserIF.c
 *
 * Description: UserInterface object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.1 $
 * $Id: bcmUserIF.c,v 1.1 2005/07/21 15:03:46 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/tr69cdefs.h"
#include "../inc/appdefs.h"

/*#include "bcmtypes.h"	*/
#include "bcm_types.h"
/*#include "bcmcfm.h" */

/** */
/** InternetGatewayDevice.UserInterface. */
#if 0
TRX_STATUS setUserInterface_PasswordRequired(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_PasswordRequired(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_PasswordUserSelectable(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_PasswordUserSelectable(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif

TRX_STATUS setUserInterface_UpgradeAvailable(char *value)
{
#if 0
	void *obj;
	PBcmCfm_TR069Cfg_t pObj;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_TR069Cfg_t)obj;
		if ( pObj != NULL ){
			pObj->upgradeAvailable = strcmp(value,"1")==0;
			if ( BcmCfm_objSet(BCMCFM_OBJ_SYS_TR069, obj, index) == BcmCfm_Ok )
				return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}
TRX_STATUS getUserInterface_UpgradeAvailable(char **value)
{
#if 0
	void *obj;
	PBcmCfm_TR069Cfg_t pObj;
	uint32 index = 0;
	if ( BcmCfm_objGet(BCMCFM_OBJ_SYS_TR069, &obj, &index) == BcmCfm_Ok ) {
		pObj = (PBcmCfm_TR069Cfg_t)obj;
		if ( pObj != NULL ){
			*value = strdup(pObj->upgradeAvailable? "1": "0");
			BcmCfm_objFree(BCMCFM_OBJ_SYS_TR069, obj);
			return TRX_OK;
		}
	}
#endif
	BSTD_UNUSED(value);
	return TRX_ERR;
}

#if 0
TRX_STATUS setUserInterface_WarrantyDate(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_WarrantyDate(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPName (char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPName (char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPHelpDesk(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPHelpDesk(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPHomePage (char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPHomePage (char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPHelpPage(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPHelpPage(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPLogo (char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPLogo (char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPLogoSize(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPLogoSize(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPMailServer (char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPMailServer (char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ISPNewsServer(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ISPNewsServer(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_TextColor(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_TextColor(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_BackgroundColor(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_BackgroundColor(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ButtonColor(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ButtonColor(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ButtonTextColor(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ButtonTextColor(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_AutoUpdateServer(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_AutoUpdateServer(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_UserUpdateServer(char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_UserUpdateServer(char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ExampleLogin (char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ExampleLogin (char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS setUserInterface_ExamplePassword (char *value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to set param -- delete idp if not needed *****/
		return TRX_OK;
	//}
	//return TRX_ERR;
}
TRX_STATUS getUserInterface_ExamplePassword (char **value)
{
	//InstanceDesc *idp;
	//if( idp=getCurrentInstanceDesc()){
		/* Add code here to get param -- delete idp if not needed *****/
		 *value = strdup("");
		return TRX_OK;
	//}
	//return TRX_ERR;
}
#endif

