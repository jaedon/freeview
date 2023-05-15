/*------------------------------------------------------------------------------

 <dev:header>
    Copyright(c) 2009 Sony Corporation.

    $Revision: 887 $
    $Author: sking $

</dev:header>

------------------------------------------------------------------------------*/
/*
 I2C using "feusb" driver for windows env.
 Check "feusb" document in detail.
*/

//#include <windows.h>
//#include <winioctl.h>
//#include "feusb.h"
//#include <stdlib.h>
#include "i2c_feusb.h"
#include "sony_dvb.h"
#include "sony_dvb_cxd2820.h"
#include "htype.h"

#include "di_channel_priv.h"
#include "drv_i2c.h"


/* global */
//static HANDLE g_hI2c = NULL;

#if 0
/* static function prototype */
static int i2c_feusb_CheckResponse (void);

int i2c_feusb_Read (uint8_t deviceAddress, uint8_t * pData, uint32_t numberOfBytes)
{
    BOOL bResult = FALSE;
    DWORD dwByteReturned = 0;
    PARAM_VENDOR_REQUEST sVendorReq;

    if (g_hI2c == NULL)
        return I2C_FEUSB_ERROR_OTHER;
    if (!pData)
        return I2C_FEUSB_ERROR_ARG;
    if (numberOfBytes <= 0)
        return I2C_FEUSB_OK;    /* No Data */

    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;

    sVendorReq.Function = URB_FUNCTION_VENDOR_ENDPOINT;
    sVendorReq.TransferFlags = USBD_SHORT_TRANSFER_OK;
    sVendorReq.ReservedBits = 0;
    sVendorReq.Request = RQCTL_FEUSB_READWOSUBADR;
    sVendorReq.Value = (USHORT) (numberOfBytes);
    sVendorReq.Index = (unsigned short) ((deviceAddress << 8));

    bResult = DeviceIoControl (g_hI2c,
                               IOCTL_FEUSB_VENDORREQUEST_IN,
                               &sVendorReq, sizeof (PARAM_VENDOR_REQUEST), pData, numberOfBytes, &dwByteReturned, NULL);

    if (!bResult)
        return I2C_FEUSB_ERROR_ACCESS;

    return i2c_feusb_CheckResponse ();
}

int i2c_feusb_Write (uint8_t deviceAddress, const uint8_t * pData, uint32_t numberOfBytes)
{
    BOOL bResult = FALSE;
    DWORD dwByteReturned = 0;
    PARAM_VENDOR_REQUEST *pVendorReq = NULL;

    if (g_hI2c == NULL)
        return I2C_FEUSB_ERROR_OTHER;
    if (!pData)
        return I2C_FEUSB_ERROR_ARG;
    if (numberOfBytes <= 0)
        return I2C_FEUSB_OK;    /* No Data */

    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;

    pVendorReq = (PARAM_VENDOR_REQUEST *) VK_malloc (sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes - 1);
    pVendorReq->Function = URB_FUNCTION_VENDOR_ENDPOINT;
    pVendorReq->TransferFlags = USBD_SHORT_TRANSFER_OK;
    pVendorReq->ReservedBits = 0;
    pVendorReq->Request = RQCTL_FEUSB_WRITE;
    pVendorReq->Value = (USHORT) (numberOfBytes - 1);
    pVendorReq->Index = (unsigned short) ((deviceAddress << 8) | pData[0]);

    if (numberOfBytes >= 2) {
        VK_memcpy (((unsigned char *) pVendorReq) + sizeof (PARAM_VENDOR_REQUEST), &(pData[1]), numberOfBytes - 1);
    }

    bResult = DeviceIoControl (g_hI2c,
                               IOCTL_FEUSB_VENDORREQUEST_OUT,
                               pVendorReq, sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes - 1, NULL, 0, &dwByteReturned, NULL);

    VK_free (pVendorReq);
    if (!bResult)
        return I2C_FEUSB_ERROR_ACCESS;

    return i2c_feusb_CheckResponse ();
}

/* For gateway read access */
int i2c_feusb_ReadGw (uint8_t deviceAddress, uint8_t * pData, uint32_t numberOfBytes, uint8_t gwAddress, uint8_t gwSub)
{
    BOOL bResult = FALSE;
    DWORD dwByteReturned = 0;
    PARAM_VENDOR_REQUEST sVendorReq;

    uint32_t nLengthAccess = 0;
    uint32_t nLengthRemain = numberOfBytes;
    uint8_t *pBufferWrite = pData;

    if (g_hI2c == NULL)
        return I2C_FEUSB_ERROR_OTHER;
    if (!pData)
        return I2C_FEUSB_ERROR_ARG;
    if (numberOfBytes <= 0)
        return I2C_FEUSB_OK;    /* No Data */

    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;
    gwAddress >>= 1;

    VK_memset (&sVendorReq, 0, sizeof (sVendorReq));
    sVendorReq.Function = URB_FUNCTION_VENDOR_ENDPOINT;
    sVendorReq.TransferFlags = USBD_SHORT_TRANSFER_OK;
    sVendorReq.ReservedBits = 0;
    sVendorReq.Request = RQCTL_FEUSB_READ2; /* Use READ2 interface */
    sVendorReq.Index = (USHORT) (((gwSub & 0xff) << 8)
                                 | ((deviceAddress & 0x7f) << 1) | 0x01 /* Read bit */ );
    while (nLengthRemain > 0) {
        /* READ2 cannot handle over 255 bytes */
        nLengthAccess = nLengthRemain > 255 ? 255 : nLengthRemain;
        nLengthRemain -= nLengthAccess;

        /* Demod slave address is in pI2c->user */
        sVendorReq.Value = (USHORT) (((nLengthAccess & 0xff) << 8) | (gwAddress & 0xff));

        bResult = DeviceIoControl (g_hI2c,
                                   IOCTL_FEUSB_VENDORREQUEST_IN,
                                   &sVendorReq, sizeof (sVendorReq), pBufferWrite, nLengthAccess, &dwByteReturned, NULL);

        if (!bResult)
            return I2C_FEUSB_ERROR_ACCESS;
        pBufferWrite += nLengthAccess;
    }

    return i2c_feusb_CheckResponse ();
}

/* For gateway write access */
int i2c_feusb_WriteGw (uint8_t deviceAddress, const uint8_t * pData, uint32_t numberOfBytes, uint8_t gwAddress, uint8_t gwSub)
{
    BOOL bResult = FALSE;
    DWORD dwByteReturned = 0;
    PARAM_VENDOR_REQUEST *pVendorReq = NULL;

    if (g_hI2c == NULL)
        return I2C_FEUSB_ERROR_OTHER;
    if (!pData)
        return I2C_FEUSB_ERROR_ARG;
    if (numberOfBytes <= 0)
        return I2C_FEUSB_OK;    /* No Data */

    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;
    gwAddress >>= 1;

    pVendorReq = (PARAM_VENDOR_REQUEST *) VK_malloc (sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes + 1);
    pVendorReq->Function = URB_FUNCTION_VENDOR_ENDPOINT;
    pVendorReq->TransferFlags = USBD_SHORT_TRANSFER_OK;
    pVendorReq->ReservedBits = 0;
    pVendorReq->Request = RQCTL_FEUSB_WRITE;
    pVendorReq->Value = (USHORT) (numberOfBytes + 1);
    pVendorReq->Index = (unsigned short) ((gwAddress << 8) | gwSub);    /* Write to demod 0x09 */

    /* deviceAddress is first byte */
    ((unsigned char *) pVendorReq)[sizeof (PARAM_VENDOR_REQUEST)] = ((deviceAddress & 0x7f) << 1) | 0x00;
    VK_memcpy (((unsigned char *) pVendorReq) + sizeof (PARAM_VENDOR_REQUEST) + 1, pData, numberOfBytes);

    bResult = DeviceIoControl (g_hI2c,
                               IOCTL_FEUSB_VENDORREQUEST_OUT,
                               pVendorReq, sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes + 1, NULL, 0, &dwByteReturned, NULL);

    VK_free (pVendorReq);
    if (!bResult)
        return I2C_FEUSB_ERROR_ACCESS;

    return i2c_feusb_CheckResponse ();
}

static int i2c_feusb_CheckResponse (void)
{
    int ret = I2C_FEUSB_OK;

    BOOL bResult = FALSE;
    DWORD dwByteReturned = 0;
    PARAM_VENDOR_REQUEST sVendorReq;
    UCHAR nResponse = 0;

    sVendorReq.Function = URB_FUNCTION_VENDOR_ENDPOINT;
    sVendorReq.TransferFlags = USBD_SHORT_TRANSFER_OK;
    sVendorReq.ReservedBits = 0;
    sVendorReq.Request = RQCTL_FEUSB_GET_SERIAL_TRANSFER_RESPONSE;
    sVendorReq.Value = 0;
    sVendorReq.Index = 0;

    bResult = DeviceIoControl (g_hI2c,
                               IOCTL_FEUSB_VENDORREQUEST_IN,
                               &sVendorReq, sizeof (PARAM_VENDOR_REQUEST), &nResponse, sizeof (UCHAR), &dwByteReturned, NULL);

    if (!bResult)
        return I2C_FEUSB_ERROR_ACCESS;

    switch (nResponse) {
    case (ERR_IF_OK):
    case (ERR_ACK):
        ret = I2C_FEUSB_OK;
        break;
    case (ERR_NACK):
        ret = I2C_FEUSB_ERROR_ACK;
        break;
    case (ERR_IF_NG):
    case (ERR_POWER):
        ret = I2C_FEUSB_ERROR_ACCESS;
        break;
    default:
        ret = I2C_FEUSB_ERROR_OTHER;
        break;
    }

    return ret;
}

/* Driver initialization */
int i2c_feusb_Initialize (void)
{
    if (g_hI2c != NULL) {
        /* Already initialized ?? */
        return I2C_FEUSB_ERROR_OTHER;
    }

    /* Open FEUSB4 first */
    g_hI2c = CreateFile ("\\\\.\\feusb4-0",
                         GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (g_hI2c == INVALID_HANDLE_VALUE) {
        /* And then open FEUSB */
        g_hI2c = CreateFile ("\\\\.\\feusb3-0",
                             GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        if (g_hI2c == INVALID_HANDLE_VALUE) {
            g_hI2c = NULL;
            return I2C_FEUSB_ERROR_OTHER;
        }
        /* Set FX2I2C interface mode (FEUSB3 only) */
        {
            BOOL bResult = FALSE;
            DWORD dwByteReturned = 0;
            PARAM_VENDOR_REQUEST sVendorReq;

            sVendorReq.Function = URB_FUNCTION_VENDOR_ENDPOINT;
            sVendorReq.TransferFlags = USBD_SHORT_TRANSFER_OK;
            sVendorReq.ReservedBits = 0;
            sVendorReq.Request = RQCTL_FEUSB_SET_IF_MODE;
            sVendorReq.Value = IF_I2C;
            sVendorReq.Index = 0;

            bResult = DeviceIoControl (g_hI2c,
                                       IOCTL_FEUSB_VENDORREQUEST_OUT,
                                       &sVendorReq, sizeof (PARAM_VENDOR_REQUEST), NULL, 0, &dwByteReturned, NULL);

            if (bResult == 0)
                return I2C_FEUSB_ERROR_OTHER;
        }
    }

    return I2C_FEUSB_OK;
}

/* Driver finalization */
int i2c_feusb_Finalize (void)
{
    if (g_hI2c == NULL) {
        /* Not initialized ?? */
        return I2C_FEUSB_ERROR_OTHER;
    }

    if (CloseHandle (g_hI2c)) {
        g_hI2c = NULL;
        return I2C_FEUSB_OK;
    }
    else {
        return I2C_FEUSB_ERROR_OTHER;
    }
}

#else

extern HINT32 show_di_ch;

static HULONG s_ulI2cSemID;

/* static function prototype */
static int i2c_feusb_CheckResponse (void);

void LockI2c( void )
{
	int nRet;

	nRet = VK_SEM_Get( s_ulI2cSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[LockI2c] ERR! VK_SEM_Get :: s_ulDiseqcSemID  nRet(%d) \n ", nRet));
	}
}

void UnLockI2c( void )
{
	int nRet;

	nRet = VK_SEM_Release( s_ulI2cSemID );
	if(nRet)
	{
		CH_DI_Print(0, ("[UnLockI2c] ERR!VK_SEM_Release:: s_ulDiseqcSemID  nRet(%d) \n ", nRet));
	}
}

int i2c_feusb_Read (uint8_t deviceAddress, uint8_t * pData, uint32_t numberOfBytes)
{
    //BOOL bResult = FALSE;
    //DWORD dwByteReturned = 0;
    //PARAM_VENDOR_REQUEST sVendorReq;
    HUINT16 usChipAddr = 0;
	int nRet = 0;
	HUINT32	ulI2cChannel = I2C_CHANNEL_CHANNEL;

	SONY_DVB_TRACE_I2C_ENTER ("i2c_feusb_Read");

	LockI2c();

    if (!pData)
    {
		UnLockI2c();
        return I2C_FEUSB_ERROR_ARG;
    }
    if (numberOfBytes <= 0)
    {
		UnLockI2c();
        return I2C_FEUSB_OK;    /* No Data */
    }

	if( deviceAddress == SONY_DVB_CXD2820_DVBT_ADDRESS || deviceAddress == SONY_DVB_CXD2820_DVBC_ADDRESS )
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
#else
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
#endif
	}
    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;
#if 0
    sVendorReq.Function = URB_FUNCTION_VENDOR_ENDPOINT;
    sVendorReq.TransferFlags = USBD_SHORT_TRANSFER_OK;
    sVendorReq.ReservedBits = 0;
    sVendorReq.Request = RQCTL_FEUSB_READWOSUBADR;
    sVendorReq.Value = (USHORT) (numberOfBytes);
    sVendorReq.Index = (unsigned short) ((deviceAddress << 8));

    bResult = DeviceIoControl (g_hI2c,
                               IOCTL_FEUSB_VENDORREQUEST_IN,
                               &sVendorReq, sizeof (PARAM_VENDOR_REQUEST), pData, numberOfBytes, &dwByteReturned, NULL);
#endif
	usChipAddr = ((deviceAddress << 8));
	//bResult = DRV_I2c_ReadNoAddr(CONST_CHANNEL_I2C_BUS, usChipAddr, pData, numberOfBytes);
	nRet = DRV_I2c_ReadNoAddr(ulI2cChannel, deviceAddress, pData, numberOfBytes);

    if ( nRet != 0 )
    {
		UnLockI2c();
        return I2C_FEUSB_ERROR_ACCESS;
    }
	UnLockI2c();
    return i2c_feusb_CheckResponse ();
}

int i2c_feusb_Write (uint8_t deviceAddress, const uint8_t * pData, uint32_t numberOfBytes)
{
    //BOOL bResult = FALSE;
    //DWORD dwByteReturned = 0;
    //PARAM_VENDOR_REQUEST *pVendorReq = NULL;
    //HUINT8 *pucBuf = NULL;
    //HUINT16 usChipAddr = 0;
    int nRet = 0;
	HUINT32	ulI2cChannel = I2C_CHANNEL_CHANNEL;

	SONY_DVB_TRACE_I2C_ENTER ("i2c_feusb_Write");

	LockI2c();

	//CH_PrintFunc("deviceAddress(0x%x), numberOfBytes(%d)", deviceAddress, numberOfBytes);

    if (!pData)
    {
		UnLockI2c();
        return I2C_FEUSB_ERROR_ARG;
    }
    if (numberOfBytes <= 0)
    {
		UnLockI2c();
        return I2C_FEUSB_OK;    /* No Data */
    }

	if( deviceAddress == SONY_DVB_CXD2820_DVBT_ADDRESS || deviceAddress == SONY_DVB_CXD2820_DVBC_ADDRESS )
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
#else
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
#endif
	}
    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;
#if 0
    pVendorReq = (PARAM_VENDOR_REQUEST *) VK_malloc (sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes - 1);
    pVendorReq->Function = URB_FUNCTION_VENDOR_ENDPOINT;
    pVendorReq->TransferFlags = USBD_SHORT_TRANSFER_OK;
    pVendorReq->ReservedBits = 0;
    pVendorReq->Request = RQCTL_FEUSB_WRITE;
    pVendorReq->Value = (USHORT) (numberOfBytes - 1);
    pVendorReq->Index = (unsigned short) ((deviceAddress << 8) | pData[0]);

    if (numberOfBytes >= 2) {
        VK_memcpy (((unsigned char *) pVendorReq) + sizeof (PARAM_VENDOR_REQUEST), &(pData[1]), numberOfBytes - 1);
    }

    bResult = DeviceIoControl (g_hI2c,
                               IOCTL_FEUSB_VENDORREQUEST_OUT,
                               pVendorReq, sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes - 1, NULL, 0, &dwByteReturned, NULL);

    VK_free (pVendorReq);
#endif

#if 0
	pucBuf = VK_malloc (numberOfBytes - 1);
	usChipAddr = ((deviceAddress << 8) | pData[0]);

    if (numberOfBytes >= 2) {
        VK_memcpy (pucBuf , &(pData[1]), numberOfBytes - 1);
    }
#endif

    nRet = DRV_I2c_WriteNoAddr(ulI2cChannel, deviceAddress, pData, numberOfBytes);

    //free (pucBuf);
	//CH_PrintFunc(">>>>>>>> DRV_I2c_WriteNoAddr() : nRet(%d)", nRet);
    if ( nRet != 0 )
    {
		UnLockI2c();
        return I2C_FEUSB_ERROR_ACCESS;
    }
	UnLockI2c();
    return i2c_feusb_CheckResponse ();
}

/* For gateway read access */
int i2c_feusb_ReadGw (uint8_t deviceAddress, uint8_t * pData, uint32_t numberOfBytes, uint8_t gwAddress, uint8_t gwSub)
{
    //BOOL bResult = FALSE;
    //DWORD dwByteReturned = 0;
    //PARAM_VENDOR_REQUEST sVendorReq;

    //uint32_t nLengthAccess = 0;
    //uint32_t nLengthRemain = numberOfBytes;
    //uint8_t *pBufferWrite = pData;
	CH_UNUSED(gwSub);

	SONY_DVB_TRACE_I2C_ENTER ("i2c_feusb_ReadGw");

    if (!pData)
        return I2C_FEUSB_ERROR_ARG;
    if (numberOfBytes <= 0)
        return I2C_FEUSB_OK;    /* No Data */

    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;
    gwAddress >>= 1;
#if 0
    VK_memset (&sVendorReq, 0, sizeof (sVendorReq));
    sVendorReq.Function = URB_FUNCTION_VENDOR_ENDPOINT;
    sVendorReq.TransferFlags = USBD_SHORT_TRANSFER_OK;
    sVendorReq.ReservedBits = 0;
    sVendorReq.Request = RQCTL_FEUSB_READ2; /* Use READ2 interface */
    sVendorReq.Index = (USHORT) (((gwSub & 0xff) << 8)
                                 | ((deviceAddress & 0x7f) << 1) | 0x01 /* Read bit */ );
    while (nLengthRemain > 0) {
        /* READ2 cannot handle over 255 bytes */
        nLengthAccess = nLengthRemain > 255 ? 255 : nLengthRemain;
        nLengthRemain -= nLengthAccess;

        /* Demod slave address is in pI2c->user */
        sVendorReq.Value = (USHORT) (((nLengthAccess & 0xff) << 8) | (gwAddress & 0xff));

        bResult = DeviceIoControl (g_hI2c,
                                   IOCTL_FEUSB_VENDORREQUEST_IN,
                                   &sVendorReq, sizeof (sVendorReq), pBufferWrite, nLengthAccess, &dwByteReturned, NULL);

        if (!bResult)
            return I2C_FEUSB_ERROR_ACCESS;
        pBufferWrite += nLengthAccess;
    }
#endif
    return i2c_feusb_CheckResponse ();
}

/* For gateway write access */
int i2c_feusb_WriteGw (uint8_t deviceAddress, const uint8_t * pData, uint32_t numberOfBytes, uint8_t gwAddress, uint8_t gwSub)
{
    BOOL bResult = FALSE;
    //DWORD dwByteReturned = 0;
    //PARAM_VENDOR_REQUEST *pVendorReq = NULL;
    HUINT16 usChipAddr = 0;
    HUINT8 *pucBuf = NULL;
	HUINT32	ulI2cChannel = I2C_CHANNEL_CHANNEL;

	SONY_DVB_TRACE_I2C_ENTER ("i2c_feusb_WriteGw");

	LockI2c();

    if (!pData)
    {
		UnLockI2c();
        return I2C_FEUSB_ERROR_ARG;
    }
    if (numberOfBytes <= 0)
    {
		UnLockI2c();
        return I2C_FEUSB_OK;    /* No Data */
    }

	CH_PrintFunc("deviceAddress(0x%x), gwAddress(0x%x), gwSub(0x%x)", deviceAddress, gwAddress, gwSub);

	if( deviceAddress == SONY_DVB_CXD2820_DVBT_ADDRESS || deviceAddress == SONY_DVB_CXD2820_DVBC_ADDRESS )
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
#else
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
#endif
	}

    /* Convert I2C address to 7bit form */
    deviceAddress >>= 1;
    gwAddress >>= 1;

#if 0
    pVendorReq = (PARAM_VENDOR_REQUEST *) VK_malloc (sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes + 1);
    pVendorReq->Function = URB_FUNCTION_VENDOR_ENDPOINT;
    pVendorReq->TransferFlags = USBD_SHORT_TRANSFER_OK;
    pVendorReq->ReservedBits = 0;
    pVendorReq->Request = RQCTL_FEUSB_WRITE;
    pVendorReq->Value = (USHORT) (numberOfBytes + 1);
    pVendorReq->Index = (unsigned short) ((gwAddress << 8) | gwSub);    /* Write to demod 0x09 */

    /* deviceAddress is first byte */
    ((unsigned char *) pVendorReq)[sizeof (PARAM_VENDOR_REQUEST)] = ((deviceAddress & 0x7f) << 1) | 0x00;
    VK_memcpy (((unsigned char *) pVendorReq) + sizeof (PARAM_VENDOR_REQUEST) + 1, pData, numberOfBytes);

    bResult = DeviceIoControl (g_hI2c,
                               IOCTL_FEUSB_VENDORREQUEST_OUT,
                               pVendorReq, sizeof (PARAM_VENDOR_REQUEST) + numberOfBytes + 1, NULL, 0, &dwByteReturned, NULL);

    VK_free (pVendorReq);
#endif

	pucBuf = DD_MEM_Alloc (numberOfBytes + 1);

	if( pucBuf == NULL )
	{
		UnLockI2c();
        return I2C_FEUSB_ERROR_ACCESS;
	}

	usChipAddr = ((gwAddress << 8) | gwSub);
	pucBuf[0] = ((deviceAddress & 0x7f) << 1) | 0x00;
	VK_MEM_Memcpy (&(pucBuf[1]), pData, numberOfBytes);

    bResult = DRV_I2c_WriteNoAddr(ulI2cChannel, usChipAddr, pucBuf, numberOfBytes + 1);

	if( pucBuf != NULL )
	{
    	DD_MEM_Free (pucBuf);
	}

    if (!bResult)
    {
		UnLockI2c();
        return I2C_FEUSB_ERROR_ACCESS;
    }
	UnLockI2c();
    return i2c_feusb_CheckResponse ();
}

static int i2c_feusb_CheckResponse (void)
{
    return I2C_FEUSB_OK;
}

/* Driver initialization */
int i2c_feusb_Initialize (void)
{
	int nRet = DI_CH_OK;

	nRet = VK_SEM_Create( &s_ulI2cSemID, "hdr_foxt2", VK_SUSPENDTYPE_MAX);
	if(nRet != DI_CH_OK )
	{
		CH_DI_Print(0, ("[VK_SEM_Create] ERR! i2c_feusb_Initialize()\n"));
		return DI_CH_ERR;
	}
    return I2C_FEUSB_OK;
}

/* Driver finalization */
int i2c_feusb_Finalize (void)
{
    return I2C_FEUSB_OK;
}

#endif

