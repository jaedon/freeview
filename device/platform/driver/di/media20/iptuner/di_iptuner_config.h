/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner_config.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  May 20 03:08:21 KST 2009 $
 * File Description:	   IPTUNER Configurations  
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/
#ifndef __DI_IPTUNER_CONFIG_H__
#define __DI_IPTUNER_CONFIG_H__

/******************************
 * DEBUG Option               *
 ******************************/
//#define IPTUNER_ALL_DEBUG

#define __USE_VFIO__

#define SUPPORT_HUMAX_COOKIE 1

//#define SUPPORT_IPV6_TRANSPORT	
#if 0 /* for CANVAS */
#define INET_RADIO
#define SUPPORT_TITLE
#endif
#endif /* !  __DI_IPTUNER_CONFIG_H__ */
