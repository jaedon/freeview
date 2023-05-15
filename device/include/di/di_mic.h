
/*******************************************************************/
//	* @author	   Junho Park
//	* @file 		   di_mic.h
/*******************************************************************/



/*******************************************************************/
// Header Files													     */
/*******************************************************************/

#ifndef __DI_MIC_H__
#define __DI_MIC_H__


#include "htype.h"
#include "di_err.h"


DI_ERR_CODE DI_MIC_Start(void);
DI_ERR_CODE DI_MIC_Stop (void);
DI_ERR_CODE DI_MIC_SetVolume(HUINT32 ulMicVolume);
DI_ERR_CODE DI_MIC_SetMixerVolume(HUINT32 ulMicMixerVolume);




#endif /* __DI_MIC_H__ */

