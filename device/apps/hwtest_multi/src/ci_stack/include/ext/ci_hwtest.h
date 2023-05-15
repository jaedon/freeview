#ifndef _CI_HWTEST_H_
#define _CI_HWTEST_H_

#include <htype.h> 
#include <vkernel.h> 

#define	SIZE_1K		0x00000400
#define	SIZE_2K		0x00000800
#define	SIZE_4K		0x00001000
#define	SIZE_8K		0x00002000
#define	SIZE_16K 	0x00004000
#define	SIZE_32K 	0x00008000
#define	SIZE_64K	0x00010000
#define	SIZE_128K	0x00020000
#define	SIZE_256K	0x00040000
#define	SIZE_300K	0x0004B000
#define	SIZE_400K	0x00064000
#define	SIZE_512K	0x00080000

#define PAL_CI_ITC_TIMER_TASK_STACK_SIZE	SIZE_64K
#define PAL_CI_ITC_LAYER_TASK_STACK_SIZE	SIZE_1K

#define PAL_CI_ITC_MSG_Q_SIZE			64

#define PAL_CI_ITC_TIMER_TASK_PRIORITY		(VK_TASK_PRIORITY_MW_REF+1)
#define PAL_CI_ITC_LAYER_TASK_PRIORITY		(VK_TASK_PRIORITY_MW_REF+1)

#define	ERR_OK					0
#define	ERR_FAIL				1

//#define CI_VK_TASK_GETCURRENTID_WORKAROUND
#define VK_Print
void CI_HWTEST_DeleteAppInfo();
void CI_HWTEST_Init();
 void CI_HWTEST_AddAppInfo();
 void CI_HWTEST_AddCaSysID();
 void CI_HWTEST_DeleteCaSysID();
#endif //_CI_HWTEST_H_
