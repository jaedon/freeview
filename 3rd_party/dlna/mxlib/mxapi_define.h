
/*******************************************************************************
 * VALUPS R&D Lab., VALUPS INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by VALUPS Inc.
 * 
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of VALUPS Inc.
 ******************************************************************************/

 



#ifndef MXAPI_DEFINE_H
#define MXAPI_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif
	


#ifdef WIN32
	#if defined(MXAPI_EXPORT_DLL)
		#define __MXAPI __declspec(dllexport)
	#elif defined(MXAPI_IMPORT_DLL)
		#define __MXAPI __declspec(dllimport)
	#else
		#define __MXAPI
	#endif
#else
	#define __MXAPI
#endif


	



#ifdef __cplusplus
}
#endif



#endif // MXAPI_DEFINE_H

