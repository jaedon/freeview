/*****************************************************************************
//
//  Copyright (c) 2005  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       userinterfaceparams.c
//
******************************************************************************
//  Description:
//             UserInterface profile supported objects
//
*****************************************************************************/

/*
* Declare instances of CPE strings
*/
#include "sharedparams.h"
#include "userinterfaceparams.h" /* profiles for parameter callbacks */

/* InternetGatewayDevice.UserInterface. */
#if 0
TRXGFUNC(getUserInterface_PasswordRequired);
TRXSFUNC(setUserInterface_PasswordRequired);
TRXGFUNC(getUserInterface_PasswordUserSelectable);
TRXSFUNC(setUserInterface_PasswordUserSelectable);
#endif
TRXGFUNC(getUserInterface_UpgradeAvailable);
TRXSFUNC(setUserInterface_UpgradeAvailable);
#if 0
TRXGFUNC(getUserInterface_WarrantyDate);
TRXSFUNC(setUserInterface_WarrantyDate);
TRXGFUNC(getUserInterface_ISPName );
TRXSFUNC(setUserInterface_ISPName );
TRXGFUNC(getUserInterface_ISPHelpDesk);
TRXSFUNC(setUserInterface_ISPHelpDesk);
TRXGFUNC(getUserInterface_ISPHomePage );
TRXSFUNC(setUserInterface_ISPHomePage );
TRXGFUNC(getUserInterface_ISPHelpPage);
TRXSFUNC(setUserInterface_ISPHelpPage);
TRXGFUNC(getUserInterface_ISPLogo );
TRXSFUNC(setUserInterface_ISPLogo );
TRXGFUNC(getUserInterface_ISPLogoSize);
TRXSFUNC(setUserInterface_ISPLogoSize);
TRXGFUNC(getUserInterface_ISPMailServer );
TRXSFUNC(setUserInterface_ISPMailServer );
TRXGFUNC(getUserInterface_ISPNewsServer);
TRXSFUNC(setUserInterface_ISPNewsServer);
TRXGFUNC(getUserInterface_TextColor);
TRXSFUNC(setUserInterface_TextColor);
TRXGFUNC(getUserInterface_BackgroundColor);
TRXSFUNC(setUserInterface_BackgroundColor);
TRXGFUNC(getUserInterface_ButtonColor);
TRXSFUNC(setUserInterface_ButtonColor);
TRXGFUNC(getUserInterface_ButtonTextColor);
TRXSFUNC(setUserInterface_ButtonTextColor);
TRXGFUNC(getUserInterface_AutoUpdateServer);
TRXSFUNC(setUserInterface_AutoUpdateServer);
TRXGFUNC(getUserInterface_UserUpdateServer);
TRXSFUNC(setUserInterface_UserUpdateServer);
TRXGFUNC(getUserInterface_ExampleLogin );
TRXSFUNC(setUserInterface_ExampleLogin );
TRXGFUNC(getUserInterface_ExamplePassword );
TRXSFUNC(setUserInterface_ExamplePassword );
#endif 

TRxObjNode UserInterfaceDesc[] = {
	#if 0
	{PasswordRequired,{{tBool,0}},
	  setUserInterface_PasswordRequired,
	  getUserInterface_PasswordRequired,
	  NULL,NULL},
	{PasswordUserSelectable,{{tBool,0}},
	  setUserInterface_PasswordUserSelectable,
	  getUserInterface_PasswordUserSelectable,
	  NULL,NULL},
	#endif
	{UpgradeAvailable,{{tBool,0,0}},
	  setUserInterface_UpgradeAvailable,
	  getUserInterface_UpgradeAvailable,
	  NULL,NULL},
	#if 0
	{WarrantyDate,{{tDateTime,0}},
	  setUserInterface_WarrantyDate,
	  getUserInterface_WarrantyDate,
	  NULL,NULL},
	{ISPName ,{{tString,64}},
	  setUserInterface_ISPName ,
	  getUserInterface_ISPName ,
	  NULL,NULL},
	{ISPHelpDesk,{{tString,32}},
	  setUserInterface_ISPHelpDesk,
	  getUserInterface_ISPHelpDesk,
	  NULL,NULL},
	{ISPHomePage ,{{tString,256}},
	  setUserInterface_ISPHomePage ,
	  getUserInterface_ISPHomePage ,
	  NULL,NULL},
	{ISPHelpPage,{{tString,256}},
	  setUserInterface_ISPHelpPage,
	  getUserInterface_ISPHelpPage,
	  NULL,NULL},
	{ISPLogo ,{{tBase64,0}},
	  setUserInterface_ISPLogo ,
	  getUserInterface_ISPLogo ,
	  NULL,NULL},
	{ISPLogoSize,{{tUnsigned,0}},
	  setUserInterface_ISPLogoSize,
	  getUserInterface_ISPLogoSize,
	  NULL,NULL},
	{ISPMailServer ,{{tString,256}},
	  setUserInterface_ISPMailServer ,
	  getUserInterface_ISPMailServer ,
	  NULL,NULL},
	{ISPNewsServer,{{tString,256}},
	  setUserInterface_ISPNewsServer,
	  getUserInterface_ISPNewsServer,
	  NULL,NULL},
	{TextColor,{{tString,6}},
	  setUserInterface_TextColor,
	  getUserInterface_TextColor,
	  NULL,NULL},
	{BackgroundColor,{{tString,6}},
	  setUserInterface_BackgroundColor,
	  getUserInterface_BackgroundColor,
	  NULL,NULL},
	{ButtonColor,{{tString,6}},
	  setUserInterface_ButtonColor,
	  getUserInterface_ButtonColor,
	  NULL,NULL},
	{ButtonTextColor,{{tString,6}},
	  setUserInterface_ButtonTextColor,
	  getUserInterface_ButtonTextColor,
	  NULL,NULL},
	{AutoUpdateServer,{{tString,256}},
	  setUserInterface_AutoUpdateServer,
	  getUserInterface_AutoUpdateServer,
	  NULL,NULL},
	{UserUpdateServer,{{tString,256}},
	  setUserInterface_UserUpdateServer,
	  getUserInterface_UserUpdateServer,
	  NULL,NULL},
	{ExampleLogin ,{{tString,40}},
	  setUserInterface_ExampleLogin ,
	  getUserInterface_ExampleLogin ,
	  NULL,NULL},
	{ExamplePassword ,{{tString,30}},
	  setUserInterface_ExamplePassword ,
	  getUserInterface_ExamplePassword ,
	  NULL,NULL},
	#endif
    {NULL, {{0,0,0}}, NULL, NULL, NULL, NULL}
};
