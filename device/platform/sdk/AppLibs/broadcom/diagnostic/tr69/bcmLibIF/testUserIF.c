

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
 * File Name  : testUserIF.c
 *
 * Description: UserInterface object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.3 $
 * $Id: testUserIF.c,v 1.3 2005/11/18 20:50:41 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
 
#include <syslog.h>
#include "../main/utils.h"
#include "../inc/appdefs.h"
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h" /* defines for ACS state */
#include "bcmTestWrapper.h"


#ifdef DMALLOC
#include "dmalloc.h"
#endif

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
				return TRX_OK;
}
TRX_STATUS getUserInterface_UpgradeAvailable(char **value)
{
			*value = strdup("0");
			return TRX_OK;
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

