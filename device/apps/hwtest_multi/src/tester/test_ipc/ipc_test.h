
#ifndef _IPC_TEST_H_
#define _IPC_TEST_H_
/******************************************************************************
 * Include Files
 ******************************************************************************/

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/


typedef enum 
{
	MSG_PRINT_MESSAGE,
	MSG_CALLBACK_FUNCTION,
	MSG_PROCESS_END,
	MSG_COUNT_MAX
} MSG_TYPE_E;

typedef struct
{
	MSG_TYPE_E		msg;
	unsigned int	lParam;
	void			*pvParam;
	unsigned char	buf[64];
} IPC_MSG_T;

typedef struct IPC_CALL_EXAMPLE_t
{
	unsigned char	buf[64];
} IPC_CALL_EXAMPLE_T;

typedef enum 
{
	IPC_CALLBACK_ID_1 = 1,
	IPC_CALLBACK_ID_2,
	IPC_CALLBACK_ID_3,
	IPC_CALLBACK_ID_4,
	IPC_CALLBACK_ID_5,
	IPC_CALLBACK_ID_6,
	IPC_CALLBACK_ID_7,
	IPC_CALLBACK_ID_8,
	IPC_CALLBACK_ID_9,
	IPC_CALLBACK_ID_10,
	IPC_CALLBACK_ID_MAX
} IPC_CALLBACK_ID;


/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/


#endif /* _IPC_TEST_H_ */

/** @} */ // end of DI_CHANNEL
