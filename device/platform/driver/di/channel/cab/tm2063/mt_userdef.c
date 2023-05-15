/*****************************************************************************
**
**  Name: mt_userdef.c
**
**  Description:    User-defined MicroTuner software interface 
**
**  Functions
**  Requiring
**  Implementation: MT_WriteSub
**                  MT_ReadSub
**                  MT_Sleep
**
**  References:     None
**
**  Exports:        None
**
**  CVS ID:         $Id: mt_userdef.c,v 1.2 2008/11/05 13:46:20 software Exp $
**  CVS Source:     $Source: /export/vol0/cvsroot/software/tuners/MT2063/mt_userdef.c,v $
**	               
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**
*****************************************************************************/
#include "htype.h"
#include "mt_userdef.h"
#include "di_channel.h"
#include "di_channel_priv.h"
//#include "../inc/di_channel_i2c.h"
//#include "tda1002x.h"
#include "drv_i2c.h"

#ifdef PLATFORM_TM2063
#define MASTER_TUNER	0
#define SLAVE_TUNER		1
#endif



/*****************************************************************************
**
**  Name: MT_WriteSub
**
**  Description:    Write values to device using a two-wire serial bus.
**
**  Parameters:     hUserData  - User-specific I/O parameter that was
**                               passed to tuner's Open function.
**                  addr       - device serial bus address  (value passed
**                               as parameter to MTxxxx_Open)
**                  subAddress - serial bus sub-address (Register Address)
**                  pData      - pointer to the Data to be written to the 
**                               device 
**                  cnt        - number of bytes/registers to be written
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      user-defined
**
**  Notes:          This is a callback function that is called from the
**                  the tuning algorithm.  You MUST provide code for this
**                  function to write data using the tuner's 2-wire serial 
**                  bus.
**
**                  The hUserData parameter is a user-specific argument.
**                  If additional arguments are needed for the user's
**                  serial bus read/write functions, this argument can be
**                  used to supply the necessary information.
**                  The hUserData parameter is initialized in the tuner's Open
**                  function.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**
*****************************************************************************/
UData_t MT_WriteSub(Handle_t hUserData, 
                    UData_t addr, 
                    U8Data subAddress, 
                    U8Data *pData, 
                    UData_t cnt)
{
	UData_t status = MT_OK;                  /* Status to be returned        */
	HUINT8	ucI2CBus = 2;	
	int nRet = 0;//, i;

	if((int)hUserData == MASTER_TUNER)	
	{
		ucI2CBus = 2;	
	}
	else if((int)hUserData == SLAVE_TUNER)
	{
		ucI2CBus = 3;				
	}
	else
	{
		ucI2CBus = 2;		
	}

	//DRV_CH_C_I2CEnable(hUserData);
	nRet = DRV_I2c_WriteA8( ucI2CBus, addr, subAddress, pData, cnt );
	if ( nRet != DI_CH_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		status = MT_ERROR;
	}
	else
	{
		//CH_PrintDebug(" Success \n");
	}
	//DRV_CH_C_I2CDisable(hUserData);

	return status;

}

/*****************************************************************************
**
**  Name: MT_ReadSub
**
**  Description:    Read values from device using a two-wire serial bus.
**
**  Parameters:     hUserData  - User-specific I/O parameter that was
**                               passed to tuner's Open function.
**                  addr       - device serial bus address  (value passed
**                               as parameter to MTxxxx_Open)
**                  subAddress - serial bus sub-address (Register Address)
**                  pData      - pointer to the Data to be written to the 
**                               device 
**                  cnt        - number of bytes/registers to be written
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      user-defined
**
**  Notes:          This is a callback function that is called from the
**                  the tuning algorithm.  You MUST provide code for this
**                  function to read data using the tuner's 2-wire serial 
**                  bus.
**
**                  The hUserData parameter is a user-specific argument.
**                  If additional arguments are needed for the user's
**                  serial bus read/write functions, this argument can be
**                  used to supply the necessary information.
**                  The hUserData parameter is initialized in the tuner's Open
**                  function.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**
*****************************************************************************/
UData_t MT_ReadSub(Handle_t hUserData, 
                   UData_t addr, 
                   U8Data subAddress, 
                   U8Data *pData, 
                   UData_t cnt)
{
	UData_t status = MT_OK;                        /* Status to be returned        */
	HUINT8	ucI2CBus = 2;	
	int nRet = 0;//, i;

	if((int)hUserData == MASTER_TUNER)	
	{
		ucI2CBus = 2;	
	}
	else if((int)hUserData == SLAVE_TUNER)
	{
		ucI2CBus = 3;				
	}
	else
	{
		ucI2CBus = 2;		
	}
	//CH_PrintDebug(" MT_ReadSub[%d][0x%x][0x%x]\n", hUserData, addr, subAddress);
	//DRV_CH_C_I2CEnable(hUserData);

	nRet = DRV_I2c_ReadA8( ucI2CBus, addr, subAddress, pData, cnt );

	if ( nRet != DI_CH_OK )
	{
		CH_PrintErr(" Error[0x%x]", nRet);
		status = MT_ERROR;
	}
	else
	{
		//CH_PrintDebug(" ucRegValue[0x%x]\n", ucRegValue);
	}
	//DRV_CH_C_I2CDisable(hUserData);

	//while(1);
	return status;
	
/*  return status;  */
}


/*****************************************************************************
**
**  Name: MT_Sleep
**
**  Description:    Delay execution for "nMinDelayTime" milliseconds
**
**  Parameters:     hUserData     - User-specific I/O parameter that was
**                                  passed to tuner's Open function.
**                  nMinDelayTime - Delay time in milliseconds
**
**  Returns:        None.
**
**  Notes:          This is a callback function that is called from the
**                  the tuning algorithm.  You MUST provide code that
**                  blocks execution for the specified period of time. 
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**
*****************************************************************************/
void MT_Sleep(Handle_t hUserData, 
              UData_t nMinDelayTime)
{
    /*
    **  ToDo:  Add code here to implement a OS blocking
    **         for a period of "nMinDelayTime" milliseconds.
    */
	CH_UNUSED(hUserData);
	VK_TASK_Sleep(nMinDelayTime);
    
}


#if defined(MT2060_CNT)
#if MT2060_CNT > 0
/*****************************************************************************
**
**  Name: MT_TunerGain  (MT2060 only)
**
**  Description:    Measure the relative tuner gain using the demodulator
**
**  Parameters:     hUserData  - User-specific I/O parameter that was
**                               passed to tuner's Open function.
**                  pMeas      - Tuner gain (1/100 of dB scale).
**                               ie. 1234 = 12.34 (dB)
**
**  Returns:        status:
**                      MT_OK  - No errors
**                      user-defined errors could be set
**
**  Notes:          This is a callback function that is called from the
**                  the 1st IF location routine.  You MUST provide
**                  code that measures the relative tuner gain in a dB
**                  (not linear) scale.  The return value is an integer
**                  value scaled to 1/100 of a dB.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   06-16-2004    DAD    Original
**   N/A   11-30-2004    DAD    Renamed from MT_DemodInputPower.  This name
**                              better describes what this function does.
**
*****************************************************************************/
UData_t MT_TunerGain(Handle_t hUserData,
                     SData_t* pMeas)
{
    UData_t status = MT_OK;                        /* Status to be returned        */

    /*
    **  ToDo:  Add code here to return the gain / power level measured
    **         at the input to the demodulator.
    */



    return (status);
}
#endif
#endif
