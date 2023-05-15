/*
 * $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_pack/simrm.h-arc   1.0   17 Dec 2004 10:32:20   jhkim1  $
 */
#ifndef 	_SIMRM_H_
#define		_SIMRM_H_

typedef enum CI_HOST_MSG_TYPE_ENUM
{
	CI_HOST_INFO = 0,
	CI_HOST_ENTER,
	CI_HOST_MENU_ANS,
	CI_HOST_ENQ_ANS,
	CI_HOST_ANS,
	CI_HOST_EXIT_REQ
} CI_HOST_MSG_TYPE;

typedef enum CI_MODULE_MSG_TYPE_ENUM
{
	CI_MODULE_SLOT = 0,
	CI_MODULE_TEXT,
	CI_MODULE_MENU,
	CI_MODULE_LIST,
	CI_MODULE_ENQ,
	CI_MODULE_EXIT
} CI_MODULE_MSG_TYPE;

typedef enum CI_MODULE_STATUS_ENUM
{
	CI_MODULE_NOT = 0,
	CI_MODULE_INSTALL,
	CI_MODULE_INVALID,
	CI_MODULE_OK
} CI_MODULE_MSG_STATUS;

extern void simulRM(void *lparameters);

#endif

