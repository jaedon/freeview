
/**************************************************************************************/
/**
 * @file OplAlexahelper.h
 *
 * Alexa information
 *
 **************************************************************************************
 **/

#ifndef __OPLALEXAHELPER_H__
#define __OPLALEXAHELPER_H__

#include "bpltype.h"
#include "opltype.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/

/******************************************************************
	Typedef
******************************************************************/

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

BPLStatus OPL_AlexaHelper_processData(const char *data);

#ifdef __cplusplus
};

/******************************************************************
	class OplAlexaHelper
******************************************************************/
class OplAlexaHelper {

public:
	OplAlexaHelper()
	{
	}
	virtual ~OplAlexaHelper()
	{
	}

	static bool processData(const char *data)
	{ return OPL_AlexaHelper_processData(data) == BPL_STATUS_OK; }
};

#endif

#endif // __OPLALEXAHELPER_H__

