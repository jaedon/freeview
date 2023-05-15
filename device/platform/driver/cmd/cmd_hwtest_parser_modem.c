#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"

#include "cmdtool.h"
#include "drv_gpio.h"
#include "drv_uart.h"

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1
#define CBUF_SIZE				(1<<10)//1024

/* global variables */

/* static variables */
static int fd_rd, fd_wr;
static unsigned char *s_pucCbuf;
static unsigned int s_unCbuf_wp, s_unCbuf_rp;
static bool s_bDCDStatus;

/* static functions */
static void P_HwTest_Modem_Init(void);
static void P_HwTest_Modem_Task(void* param);
static ssize_t P_HwTest_Modem_Write(void* pvBuf, size_t unByteNum);
static ssize_t P_HwTest_Modem_Read(void* pvBuf, size_t unByteNum);
static void P_HwTest_Modem_Disconnect(void);
static void P_HwTest_Modem_GetStatusDCD_Callback_ISR(void *context, int param);
static void P_HwTest_Modem_GetWritableCBufSize(size_t *punSize);
static void P_HwTest_Modem_GetReadableCBufSize(size_t *punSize);

/* extern functions */

size_t wp_cnt, rp_cnt;//bh
int CMD_HwTest_Parser_Modem(void *szArgs)
{
	static int s_nIsFirstCalling=TRUE;
	int i;
	unsigned char cr = 0xd;

	char *szCmd=(char *)CMD_NextArg((char **)&szArgs);

	if(s_nIsFirstCalling)
	{
		P_HwTest_Modem_Init();
		s_nIsFirstCalling = FALSE;
	}

	if(szCmd==NULL)
	{
		return CMD_ERR;
	}

	if (VK_strcmp(szCmd,"cmd")==0)
	{
		szCmd=(char *)CMD_NextArg((char **)&szArgs);
		CMD_Printf("send %s\n", szCmd);
		P_HwTest_Modem_Write(szCmd, VK_strlen(szCmd));
		P_HwTest_Modem_Write(&cr, 1);
	}
	else if (VK_strcmp(szCmd,"stress")==0)
	{
		wp_cnt = rp_cnt = 0; //bh
		CMD_Printf("stress test : send [0~9]*1000\n");
		for(i=0; i<1000; i++)
		{
			P_HwTest_Modem_Write("0123456789", 10);
		}
	}
	else if (VK_strcmp(szCmd,"discon")==0)
	{
		CMD_Printf("disconnect\n");
		P_HwTest_Modem_Disconnect();
	}
	else if (VK_strcmp(szCmd,"dcd")==0)
	{
		CMD_Printf("get dcd status : %d\n", s_bDCDStatus);
	}
	else if (VK_strcmp(szCmd,"reset")==0)
	{
		CMD_Printf("modem chip reset\n");

		DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_LOW);
		VK_TASK_Sleep(20);
		DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_HI);
		VK_TASK_Sleep(500);
	}
	else if (VK_strcmp(szCmd,"reset_high")==0)
	{
		CMD_Printf("modem chip reset pin high\n");
		DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_HI);
	}
	else if (VK_strcmp(szCmd,"reset_low")==0)
	{
		CMD_Printf("modem chip reset pin low\n");
		DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_LOW);
	}
	else if (VK_strcmp(szCmd,"aaa")==0) //bh
	{
		P_HwTest_Modem_GetWritableCBufSize(&i);
		CMD_Printf("Writable:%d, ", i);
		P_HwTest_Modem_GetReadableCBufSize(&i);
		CMD_Printf("Readble:%d\n", i);
		CMD_Printf("wp:%d, rp:%d, s_unCbuf_wp:%d, s_unCbuf_rp:%d\n", wp_cnt, rp_cnt, s_unCbuf_wp, s_unCbuf_rp);
	}
	else
	{
		return CMD_ERR;
	}

	return CMD_OK;
}

static void P_HwTest_Modem_Init(void)
{
	struct termios tio;
	int nErr;
	HULONG ulTaskId;
	DRV_Error eErr;

	/* set DTR low */
	DRV_GPIO_Write(GPIO_ID_MODEM_DTR, GPIO_LOW);

	/* reset modem */
	DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_LOW);
	VK_TASK_Sleep(20);
	DRV_GPIO_Write(GPIO_ID_MODEM_RESET, GPIO_HI);
	VK_TASK_Sleep(500);

	/* register DCD line status interrupt */
	eErr = DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_MODEM_DCD, P_HwTest_Modem_GetStatusDCD_Callback_ISR, NULL, 0);
	if(eErr != DRV_OK)
	{
		CMD_Printf("[P_HwTest_Modem_Init] Error(0x%x) in DRV_GPIO_INTR_RegisterEventNotify()\n", eErr);
		return;
	}

	/* open uart A */
	fd_rd = open("/dev/ttyS0", O_RDONLY|O_NOCTTY);
	if(fd_rd < 0)
	{
		CMD_Printf("[P_HwTest_Modem_Init] Read Open Error-(%d)\n", fd_rd);
		return;
	}

	fd_wr = open("/dev/ttyS0", O_WRONLY|O_NOCTTY);
	if(fd_wr < 0)
	{
		CMD_Printf("[P_HwTest_Modem_Init] Write Open Error-(%d)\n", fd_wr);
		return;
	}

	tio.c_cflag = B9600|CS8|CREAD|CRTSCTS|CLOCAL;
	tio.c_cflag &= ~HUPCL;
	tio.c_lflag = 0;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;

	tio.c_cc[VTIME]    = 0;   /* 문자 사이의 timer를 disable */
    tio.c_cc[VMIN]     = 1;   /* 최소 1 문자 받을 때까진 blocking */

	tcflush(fd_rd,TCIOFLUSH);
	tcsetattr(fd_rd,TCSANOW, &tio);
	tcflush(fd_wr,TCIOFLUSH);
	tcsetattr(fd_wr,TCSANOW, &tio);

	/* alloc rx circular buf */
	s_pucCbuf = (unsigned char*)DD_MEM_Alloc(CBUF_SIZE);
	if(s_pucCbuf == NULL)
	{
		CMD_Printf("[P_HwTest_Modem_Init] Error in DD_MEM_Alloc()\n");
		return;
	}

	/* create rx task */
	nErr = VK_TASK_Create(P_HwTest_Modem_Task, MODEM_TASK_PRIORITY, BASIC_TASK_STACK_SIZE, "modemTask", NULL, &ulTaskId, FALSE);
	if( nErr != VK_OK )
	{
		CMD_Printf("[P_HwTest_Modem_Init] Error(0x%x) in VK_TASK_Create()\n", nErr);
		return;
	}

	VK_TASK_Start(ulTaskId);
}

static void P_HwTest_Modem_Task(void* param)
{
	unsigned char *pucBuf;
	unsigned char *pucModemReadBuf;
	size_t unReadSize;
	int retval, i;

	BSTD_UNUSED(param);

	while(1)
	{
		pucBuf = &s_pucCbuf[s_unCbuf_wp];
		P_HwTest_Modem_GetWritableCBufSize(&unReadSize);
		if(unReadSize)
		{
			retval = read(fd_rd, pucBuf, unReadSize);
			if(retval < 0)
			{
				CMD_Printf("Error :%0x\n", retval);
			}
			else if(retval==0)
			{
				CMD_Printf("."); /* to check non blocking */
			}
	/* for debugging
			else
			{
				for(i=0; i<retval; i++)
				{
					CMD_Printf("%c", pucBuf[i]);
				}
				CMD_Printf("[");
				for(i=0; i<retval; i++)
				{
					CMD_Printf("%02x ", pucBuf[i]);
				}
				CMD_Printf("]");
				CMD_Printf("\n");
			}
	*/
			s_unCbuf_wp = (s_unCbuf_wp + retval) & (CBUF_SIZE - 1);
			wp_cnt += retval;
		}

		/* --------- read cbuf -------- */
		P_HwTest_Modem_GetReadableCBufSize(&unReadSize);
		if(unReadSize)
		{
			pucModemReadBuf = (unsigned char*)DD_MEM_Alloc(unReadSize);

			retval = P_HwTest_Modem_Read(pucModemReadBuf, unReadSize);
			if((size_t)retval != unReadSize)
			{
				CMD_Printf("[P_HwTest_Modem_Task] Error in P_HwTest_Modem_Read()\n");
			}
			else
			{
				for(i=0; i<retval; i++)
				{
					CMD_Printf("%c", pucBuf[i]);
				}
			}
			rp_cnt += retval;
			DD_MEM_Free(pucModemReadBuf);
		}
	}
}

static ssize_t P_HwTest_Modem_Write(void* pvBuf, size_t unByteNum)
{
	size_t write_size, total_write_size;
	ssize_t retval;

	total_write_size = unByteNum;

	do
	{
		write_size = total_write_size;
		retval = write(fd_wr, (void*)pvBuf, write_size);
		if(retval < 0 || (size_t)retval > write_size)
		{
			CMD_Printf("[P_HwTest_Modem_Write] TX Error 1:%d \n", retval);
			return DRV_ERR;
		}

		total_write_size -= (size_t)retval;

	} while(total_write_size);

	return (ssize_t)unByteNum;
}

static ssize_t P_HwTest_Modem_Read(void* pvBuf, size_t unByteNum)
{
	size_t rp2wp_size, rp2end_size;
	size_t read_size_temp, read_size, total_read_size;

	rp2wp_size = (s_unCbuf_wp - s_unCbuf_rp) & (CBUF_SIZE - 1);
	rp2end_size = (CBUF_SIZE- 1) - s_unCbuf_rp;

	total_read_size = (unByteNum < rp2wp_size)? unByteNum:rp2wp_size;
	unByteNum = total_read_size;

	do
	{
		rp2wp_size = (s_unCbuf_wp - s_unCbuf_rp) & (CBUF_SIZE - 1);
		rp2end_size = (s_unCbuf_rp == (CBUF_SIZE- 1))? CBUF_SIZE:(CBUF_SIZE- 1) - s_unCbuf_rp;

		read_size_temp = (rp2wp_size < rp2end_size)? rp2wp_size:rp2end_size;
		read_size = (total_read_size < read_size_temp)? total_read_size:read_size_temp;

		VK_memcpy(pvBuf, &s_pucCbuf[s_unCbuf_rp], read_size);

		pvBuf = (void*)((int)pvBuf + read_size);
		s_unCbuf_rp = (s_unCbuf_rp + read_size) & (CBUF_SIZE - 1);
		total_read_size -= read_size;
	} while(total_read_size);

	return (ssize_t)unByteNum;
}

static void P_HwTest_Modem_Disconnect(void)
{
	DRV_GPIO_Write(GPIO_ID_MODEM_DTR, GPIO_HI);
	VK_TASK_Sleep(300);
	DRV_GPIO_Write(GPIO_ID_MODEM_DTR, GPIO_LOW);
}

static void P_HwTest_Modem_GetStatusDCD_Callback_ISR(void *context, int param)
{
	GPIO_STATE_t eState;

	BSTD_UNUSED(context);
	BSTD_UNUSED(param);

	DRV_GPIO_Read(GPIO_ID_MODEM_DCD, &eState);
	CMD_Printf("DCD:%d\n", eState);

	s_bDCDStatus = (bool)eState;
}

static void P_HwTest_Modem_GetWritableCBufSize(size_t *punSize)
{
	size_t wp2rp_size, wp2end_size;

	wp2rp_size = (s_unCbuf_rp - s_unCbuf_wp - 1) & (CBUF_SIZE - 1);
	wp2end_size = (s_unCbuf_wp == (CBUF_SIZE- 1))? CBUF_SIZE:(CBUF_SIZE- 1) - s_unCbuf_wp;

	*punSize = (wp2rp_size < wp2end_size)? wp2rp_size:wp2end_size;
}

static void P_HwTest_Modem_GetReadableCBufSize(size_t *punSize)
{
	*punSize = (s_unCbuf_wp - s_unCbuf_rp) & (CBUF_SIZE - 1);
}



