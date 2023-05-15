#ifndef __CMD_HWTEST_PARSER_PVR_H__
#define __CMD_HWTEST_PARSER_PVR_H__

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
typedef void (*CMD_HwTest_Pvr_Event_Callback)(void *pvParam);

typedef struct
{
	HUINT32	   	ulEvent;			/* CMD_HWTEST_PVR_EVENT */
	HUINT32		ulParam;		
} CMD_HWTEST_PVR_MSG;

typedef	enum
{
	CMD_HWTEST_PVR_EVENT_COPY_DONE=0,
	CMD_HWTEST_PVR_EVENT_MAX_NUMBER	
}	CMD_HWTEST_PVR_EVENT;

void CMD_HwTest_PvrRegisterEventCallback(CMD_HWTEST_PVR_EVENT eEvent, CMD_HwTest_Pvr_Event_Callback pCallbackFunc);
#endif

int CMD_HwTest_Parser_Pvr(void *szArgs);
int CMD_HwTest_Parser_PvrEx(void *szArgs);


#endif //__CMD_HWTEST_PARSER_PVR_H__

