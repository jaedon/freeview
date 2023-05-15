
/********************************************************************
 * Workfile   : drv_micom.c
 * Project    : Kraken
 * Author     : khwoo
 * Description: Commands Template
 *
 *                                 Copyright (c) 2009 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#define _______________________________________________________________________
#define ____HEADER_FILES____
#include "htype.h"
#include "vkernel.h"

#include "bstd.h"
#include "breg_mem.h"
#include "bkni.h"

#include "linden_trace.h"
#include "di_uart.h"

#include "bchp_sun_top_ctrl.h"
#include "bchp_uartc.h"

#include "vkernel.h"

#include "di_err.h"
#include "di_key.h"
#include "di_system.h"
#include "drv_micom.h"
//#include "di_panel.h"

//#define HDEBUG
//#define HDEBUG_DEBUG
#define DEBUG_MODULE    TRACE_MODULE_DRV_MICOM

#define _______________________________________________________________________
#define ____MACRO_DEFINITION____
#define MAX_HEX_LINE_LEN	80
#define MAX_MICOM_IMAGE_SIZE	64*1024

#define ERR_MICOM_CHECKSUM		0xE1
#define ERR_MICOM_TOO_BIG		0xE2
#define ERR_MICOM_OK			0x0

//#define MICOM_LOADER_START_ADDR 0x1E00
#define MICOM_PAGE_SIZE			64


#define DRV_ERR_MICOM_DOWNLOAD	100

#define _______________________________________________________________________
#define ____EXTERNAL_VARIABLE_DEFINITION____
extern unsigned long s_ulMicomTxSema;
extern unsigned long s_ulMicomRxSema;


#define _______________________________________________________________________
#define ____EXTERNAL_FUNCTION_DEFINITION____
#undef PrintDebug
#define PrintDebug DI_UART_Print
extern DI_ERR_CODE DRV_SYSTEM_EventCallback(DI_SYS_EVENT eSysEvent, HUINT32 ulProgress);

#define _______________________________________________________________________
#define ____TYPE_DEFINITION____
typedef struct  // Intel HEX file record
{
	HUINT8		length; // Record length in number of data bytes.
	HUINT32		offset; // Offset address.
	HUINT8		type; // Record type.
	HUINT8		data[MAX_HEX_LINE_LEN]; // Optional data bytes.
} HEXRecord;

typedef struct
{
	HUINT8		data[MAX_MICOM_IMAGE_SIZE]; // Holds the data bytes.
	HUINT32		start, end; // Data address range.
	HUINT32 autoincrement; // Bootloader supports address autoincrement?
	HUINT32		address;
	HUINT32		size; // Size of databuffer.
} HEX_FILE_INFO;

#define _______________________________________________________________________
#define ____GLOBAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_VARIABLE_DEFINITION____
static HEX_FILE_INFO g_HexFileInfo;


#define _______________________________________________________________________
#define ____STATIC_FUNCTION_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_FUNCTION_BODY____
static void setAddress(HINT32 address)
{
	HUINT8 ucRxData=0xff;

	/* Set current address */
	if( address < 0x10000 )
	{
		DRV_MICOM_TX_BYTE( 'A' );
		DRV_MICOM_TX_BYTE( (address >> 8) & 0xff );
		DRV_MICOM_TX_BYTE( address & 0xff );
	}
	else
	{
		DRV_MICOM_TX_BYTE( 'H' );
		DRV_MICOM_TX_BYTE( (address >> 16) & 0xff );
		DRV_MICOM_TX_BYTE( (address >> 8) & 0xff );
		DRV_MICOM_TX_BYTE( address & 0xff );
	}

	/* Should return CR */
	DRV_MICOM_RX_BYTE(&ucRxData);
	if( ucRxData != '\r' ) {
		PrintDebug( "Setting address for programming operations failed! ");
	}
}

static void writeFlashLowByte( HINT32 value )
{
	HUINT8 ucRxData=0xff;

	DRV_MICOM_TX_BYTE( 'c' );
	DRV_MICOM_TX_BYTE( (HUINT8)value );

	DRV_MICOM_RX_BYTE(&ucRxData);
	if( ucRxData != '\r' )
		PrintDebug( "Writing Flash low byte failed! ");
}

static void writeFlashHighByte( HINT32 value )
{
	HUINT8 ucRxData=0xff;

	DRV_MICOM_TX_BYTE( 'C' );
	DRV_MICOM_TX_BYTE( (HUINT8)value );

	DRV_MICOM_RX_BYTE(&ucRxData);
	if( ucRxData != '\r' )
		PrintDebug( "Writing Flash high byte failed! ");
}

static void writeFlashPage()
{
	HUINT8 ucRxData=0xff;

	DRV_MICOM_TX_BYTE( 'm' );
	DRV_MICOM_RX_BYTE(&ucRxData);
	if( ucRxData != '\r' )
		PrintDebug( "Writing Flash page failed! ");
}

static HINT32 ReadString(HINT8* pFile, HINT32 lFileSize, HINT32 *piFileIndex, HINT8* pHexLine)
{
	HINT32 iIndex=*piFileIndex;
	HINT32 iLineIndex=0;

	while (1)
	{
		if (iIndex<lFileSize && pFile[iIndex]==0x0d)
		{
			pHexLine[iLineIndex]=0;
			iIndex++; // because the next one is expected to be 0x0a
			iIndex++; // To set the file pointer to the next hex line.
			*piFileIndex=iIndex;
			break;
		}
		else
			pHexLine[iLineIndex++]=pFile[iIndex++];
	}

	if (iLineIndex>0)
		return TRUE;
	else
		return FALSE;
}

static HINT32 GetTxtLength(HINT8 *txt)
{
	HINT32 iIndex=0;

	while (1)
	{
		if (txt[iIndex]==0x00)
			break;
		else
			iIndex++;
	}

	return iIndex;
}

static HINT32 convertHex(HINT8 *txt)
{
	HINT32 result = 0;
	HINT32 digit = 0;
	HINT32 i;
	HINT32 iTxtLen;

	iTxtLen=GetTxtLength(txt);
	if(iTxtLen == 0 )
		PrintDebug( "Cannot convert zero-length hex-string to number!" );

	if( iTxtLen > 8 )
		PrintDebug( "Hex conversion overflow! Too many hex digits in string." );


	for( i = 0; i < iTxtLen ; i++ )
	{
		/* Convert hex digit */
		if( txt[i] >= '0' && txt[i] <= '9' )
			digit = txt[i] - '0';
		else if( txt[i] >= 'a' && txt[i] <= 'f' )
			digit = txt[i] - 'a' + 10;
		else if( txt[i] >= 'A' && txt[i] <= 'F' )
			digit = txt[i] - 'A' + 10;
		else
			PrintDebug( "Invalid hex digit found!" );

		/* Add digit as least significant 4 bits of result */
		result = (result << 4) | digit;
	}

	return result;
}


static HINT8* string_Mid(HINT8* hexline, HINT32 iStartPos, HINT32 iLen)
{
	static HINT8 s_caTmpString[MAX_HEX_LINE_LEN];
	HINT32 iIndex;

	for (iIndex=0 ; iIndex<iLen ; iIndex++)
		s_caTmpString[iIndex]=hexline[iStartPos+iIndex];
	s_caTmpString[iIndex]=0x00;

	return &s_caTmpString[0];
}


static void parseRecord(HINT8 *hexLine, HEXRecord *recp)
{
	HUINT8 checksum;
	HINT32 recordPos; // Position inside record data fields.
	HINT32 iLineLen;


	iLineLen=GetTxtLength(hexLine);
	if( iLineLen < 11 ) // At least 11 characters.
		PrintDebug("Wrong HEX file format, missing fields! ");

	/* Check format for line */
	if( hexLine[0] != ':' ) // Always start with colon.
		PrintDebug("Wrong HEX file format, does not start with colon! ");

	/* Parse length, offset and type */
	recp->length = (HUINT8)convertHex( string_Mid(hexLine, 1, 2 ) );
	recp->offset = convertHex( string_Mid(hexLine, 3, 4 ) );
	recp->type = (HUINT8)convertHex( string_Mid(hexLine, 7, 2 ) );

	/* We now know how HINT32 the record should be */
	if( iLineLen < (11+recp->length*2) )
		PrintDebug("Wrong HEX file format, missing fields!");

	/* Process checksum */
	checksum = recp->length;
	checksum += (HUINT8) ((recp->offset >> 8) & 0xff);
	checksum += (HUINT8) (recp->offset & 0xff);
	checksum += recp->type;

	/* Parse data fields */
	if( recp->length )
	{
		/* Read data from record */
		for( recordPos = 0; recordPos < recp->length; recordPos++ )
		{
			recp->data[ recordPos ] = (HUINT8)convertHex( string_Mid(hexLine, 9 + recordPos*2, 2 ) );
			checksum += recp->data[ recordPos ];
		}
	}

	/* Correct checksum? */
	checksum += (HUINT8)convertHex( string_Mid(hexLine, 9 + recp->length*2, 2 ) );
	if( checksum != 0 )
	{
		PrintDebug( "Wrong checksum for HEX record! ");
	}
}

static HUINT32 readHexFile(HUINT8 *pucData, HUINT32 ulSize)
{
	HINT8 hexLine[MAX_HEX_LINE_LEN]; // Contains one line of the HEX file.
	HEXRecord rec; // Temp record.

	HUINT32 baseAddress; // Base address for extended addressing modes.
	HINT32 dataPos; // Data position in record.

	HINT32 iFileIndex=0;
#if defined(_DEBUG_MICOM_DOWNLOAD_)
	HINT32 iTemp=0;
#endif

	/* Prepare */
	baseAddress = 0;
	g_HexFileInfo.size=MAX_MICOM_IMAGE_SIZE;
	g_HexFileInfo.start = g_HexFileInfo.size;
	g_HexFileInfo.end = 0;

#if defined(_DEBUG_MICOM_DOWNLOAD_)
	PrintDebug("\n");
	for (iFileIndex=0 ; iFileIndex<ulSize ; iFileIndex++)
		PrintDebug("%c", pucData[iFileIndex]);
	PrintDebug("\n");
	iFileIndex=0;
#endif
	/* Parse records */
	while(ReadString(pucData, ulSize, &iFileIndex, hexLine)==TRUE)
	{
#if defined(_DEBUG_MICOM_DOWNLOAD_)
		PrintDebug("\n");
		iTemp=0;
		while (1)
		{
			PrintDebug("%c", hexLine[iTemp++]);
			if (hexLine[iTemp]==0x00)
				break;
		}
		PrintDebug("\n");
#endif
		/* Process record according to type */
		parseRecord( hexLine, &rec );

		switch( rec.type )
		{
			case 0x00 : // Data record ?
				/* Copy data */
				if( baseAddress + rec.offset + rec.length > g_HexFileInfo.size )
				{
					PrintDebug( "HEX file defines data outside buffer limits! ");
					return FALSE;
				}

				for( dataPos = 0; dataPos < rec.length; dataPos++ )
					g_HexFileInfo.data[ baseAddress + rec.offset + dataPos ] = rec.data[ dataPos ];

				/* Update byte usage */
				if( baseAddress + rec.offset < g_HexFileInfo.start )
					g_HexFileInfo.start = baseAddress + rec.offset;

				if( baseAddress + rec.offset + rec.length - 1 > g_HexFileInfo.end )
					g_HexFileInfo.end = baseAddress + rec.offset + rec.length - 1;

				break;

			case 0x02 : // Extended segment address record ?
				baseAddress = (rec.data[0] << 8) | rec.data[1];
				baseAddress <<= 4;
				break;

			case 0x03 : // Start segment address record ?
				break; // Ignore it, since we have no influence on execution start address.

			case 0x04 : // Extended linear address record ?
				baseAddress = (rec.data[0] << 8) | rec.data[1];
				baseAddress <<= 16;
				break;

			case 0x05 : // Start linear address record ?
				break; // Ignore it, since we have no influence on exectuion start address.

			case 0x01 : // End of file record ?
				return TRUE;

			default:
				PrintDebug("Unsupported HEX record format! ");
				return FALSE;
		}
	}


	/* We should not end up here */
	PrintDebug("Premature end of file encountered! Make sure file ");
	return FALSE;
}

static HUINT32 writeFlashBlock(HINT8 *pucData, HUINT32 ulSize)
{
/*
	pucData is actualy a starting point of micom HEX image.
*/
	HINT32 start, end; // Data address range.
	HINT32 blocksize; // Bootloader block size.
	HINT32 bytecount;
	HINT32 address;
	HINT32 pagesize;
	HUINT8 ucTx;
	HUINT8 ucCheckSum;
	HUINT8 ucRxData=0xff;

	HAPPY(pucData);
	HAPPY(ulSize);

	pagesize=MICOM_PAGE_SIZE;

	/* Get block size, assuming command 'b' just issued and 'Y' has been read */
	blocksize=pagesize; // to reduce the bootloader size

	/* Get range from HEX file */
	start = g_HexFileInfo.start;
	end = g_HexFileInfo.end;


	/* Need to write one odd byte first? */
	address = start;
	if( address & 1 )
	{
		setAddress( address >> 1 ); // Flash operations use word addresses.

		/* Use only high byte */
		writeFlashLowByte( 0xff ); // No-write in low byte.
		writeFlashHighByte( g_HexFileInfo.data[address] );
		address++;

		/* Need to write page? */
		if( (address % pagesize) == 0 ||
				address > end ) // Just passed page limit or no more bytes to write?
		{
			setAddress( (address-2) >> 1 ); // Set to an address inside the page.
			writeFlashPage();
			setAddress( address >> 1 );
		}
	}

	/* Need to write from middle to end of block first? */
	if( (address % blocksize) > 0 ) // In the middle of a block?
	{
		bytecount = blocksize - (address % blocksize); // Bytes left in block.

		if( (address+bytecount-1) > end ) // Is that past the write range?
		{
			bytecount = end-address+1; // Bytes left in write range.
			bytecount &= ~0x01; // Adjust to word count.
		}

		if( bytecount > 0 )
		{
			setAddress( address >> 1 ); // Flash operations use word addresses.

			/* Start Flash block write */
			DRV_MICOM_TX_BYTE( 'B' );
			DRV_MICOM_TX_BYTE( (bytecount>>8) & 0xff ); // Size, MSB first.
			DRV_MICOM_TX_BYTE( bytecount & 0xff );

			ucCheckSum=0;
			while( bytecount > 0 )
			{
				if (address>=MAX_MICOM_IMAGE_SIZE)
				{
					PrintDebug("\nNot enough flash memory!");
					return ERR_MICOM_TOO_BIG;
				}
				ucTx=g_HexFileInfo.data[ address ];
				DRV_MICOM_TX_BYTE( ucTx );
                ucCheckSum+=ucTx;
				address++;
				bytecount--;
			}

			DRV_MICOM_TX_BYTE( ucCheckSum );
			VK_TASK_Sleep(2);
			DRV_MICOM_RX_BYTE(&ucRxData);
			if( ucRxData != '\r' )
			{
				PrintDebug( "Writing Flash block failed! 1");
				return ucRxData;
			}

#if defined(LOADER_APP)
			if (g_numKindBlock <= 1)
				UpdateDisplay(GetTextData(0, STR_ID_PROGRAMMING_MICOM, NULL), "?", (address*100)/(end-start));
#else
			DRV_SYSTEM_EventCallback(DI_SYS_EVENT_MICOM_UPDATE_PROGRESS, (address*100)/(end-start));
#endif
			PrintDebug( "#" ); // Advance progress indicator.
		}
	}

	/* More complete blocks to write? */
	while( (end-address+1) >= blocksize )
	{
		bytecount = blocksize;
		setAddress( address >> 1 ); // Flash operations use word addresses.

		/* Start Flash block write */
		DRV_MICOM_TX_BYTE( 'B' );
		DRV_MICOM_TX_BYTE( (bytecount>>8) & 0xff ); // Size, MSB first.
		DRV_MICOM_TX_BYTE( bytecount & 0xff );

		ucCheckSum=0;
		while( bytecount > 0 )
		{
			if (address>=MAX_MICOM_IMAGE_SIZE)
			{
				PrintDebug("\nNot enough flash memory!");
				return ERR_MICOM_TOO_BIG;
			}
			ucTx=g_HexFileInfo.data[ address];
			DRV_MICOM_TX_BYTE( ucTx );
			VK_TASK_Sleep(2);
			ucCheckSum+=ucTx;
			address++;
			bytecount--;
		}

		DRV_MICOM_TX_BYTE( ucCheckSum );
		VK_TASK_Sleep(2);
		DRV_MICOM_RX_BYTE(&ucRxData);
		if( ucRxData != '\r' )
		{
			PrintDebug( "Writing Flash block failed! 2");
			return ucRxData;
		}

#if defined(LOADER_APP)
		if (g_numKindBlock <= 1)
			UpdateDisplay(GetTextData(0, STR_ID_PROGRAMMING_MICOM, NULL), "?", (address*100)/(end-start));
#else
		DRV_SYSTEM_EventCallback(DI_SYS_EVENT_MICOM_UPDATE_PROGRESS, (address*100)/(end-start));
#endif
		PrintDebug( "#" ); // Advance progress indicator.
	}

	/* Any bytes left in last block */
	if( (end-address+1) >= 1 )
	{
		bytecount = (end-address+1); // Get bytes left to write.
		if( bytecount & 1 )
			bytecount++; // Align to next word boundary.

		setAddress( address >> 1 ); // Flash operations use word addresses.

		/* Start Flash block write */
		DRV_MICOM_TX_BYTE( 'B' );
		DRV_MICOM_TX_BYTE( (bytecount>>8) & 0xff ); // Size, MSB first.
		DRV_MICOM_TX_BYTE( bytecount & 0xff );
#if 0
	 	DRV_MICOM_TX_BYTE( 'F' ); // Flash memory.
#else
		// to reduce the bootloader size
#endif
		ucCheckSum=0;
		while( bytecount > 0 )
		{
			if (address>=MAX_MICOM_IMAGE_SIZE)
			{
				PrintDebug("\nNot enough flash memory!");
				return ERR_MICOM_TOO_BIG;
			}

			if( address > end )
			{
				ucTx=0xff;
				DRV_MICOM_TX_BYTE( ucTx ); // Don't write outside write range.
				ucCheckSum+=ucTx;
			}
			else
			{
				ucTx=g_HexFileInfo.data[ address ];
				DRV_MICOM_TX_BYTE( ucTx );
				ucCheckSum+=ucTx;
			}

			address++;
			bytecount--;
		}

		DRV_MICOM_TX_BYTE( ucCheckSum );
		DRV_MICOM_RX_BYTE(&ucRxData);
		if( ucRxData != '\r' )
		{
			PrintDebug( "Writing Flash block failed! 3");
			return ucRxData;
		}

#if defined(LOADER_APP)
		if (g_numKindBlock <= 1)
	   		UpdateDisplay(GetTextData(0, STR_ID_PROGRAMMING_MICOM, NULL), "?", (address*100)/(end-start));
#else
		DRV_SYSTEM_EventCallback(DI_SYS_EVENT_MICOM_UPDATE_PROGRESS, (address*100)/(end-start));
#endif
		PrintDebug( "#" ); // Advance progress indicator.
	}
	PrintDebug("\n" ); // Finish progress indicator.
	return ERR_MICOM_OK; // Indicate supported command.
}

DI_ERR_CODE UpgradeToMicom(HUINT32 ulOffset,HUINT8 *pucData, HUINT32 ulSize)
{
#if defined(_DEBUG_MICOM_DOWNLOAD_)
	HUINT32 	i;
#endif
	HUINT8 ucRxData=0x30;
	HUINT8 ucStep=1;
	HUINT8 ucRetry=0;
	HINT32 iResult=DRV_ERR_MICOM_DOWNLOAD;

	HAPPY(ulOffset);

#if defined(_DEBUG_MICOM_DOWNLOAD_)
	PrintDebug("Programming Micom : size %d\n\r", ulSize);
#endif

	while (1)
	{
		switch (ucStep) {
		case 1 :
#if defined(LOADER_APP)
			if (g_numKindBlock <= 1)
				UpdateDisplay(GetTextData(0, STR_ID_PROGRAMMING_MICOM, NULL), "P", 0);
#endif
			if (readHexFile(pucData, ulSize)==TRUE)
			{
				ucStep=3; // No reset
#if defined(_DEBUG_MICOM_DOWNLOAD_)
				PrintDebug("\n g_HexFileInfo.data\n\r");
				for (i=0 ; i<4000 ; i++)
				{
					if (i%16==0)
						PrintDebug("\n");
					PrintDebug("%02X", g_HexFileInfo.data[i]);
				}
				PrintDebug("\n");
#endif
			}
			else
			{
				ucStep=100;
				PrintDebug("wrong HEX format\r\n");
			}
			continue;
		case 2 :
			DRV_MICOM_TX_BYTE('R');
			VK_TASK_Sleep(2);
			continue;
		case 3 :
			DRV_MICOM_TX_BYTE('!');
			VK_TASK_Sleep(2);
			DRV_MICOM_RX_BYTE(&ucRxData);
			if (ucRxData=='?')
			{
				ucStep=4;
			}
			else
			{
				ucRetry++;
				if (ucRetry>5)
				{
					ucRetry=0;
					ucStep=100;
#if defined(LOADER_APP)
					UpdateDisplay(GetTextData(0, STR_ID_MICOM_NO_LOADER, NULL), "?", 0);
#endif
					PrintDebug("Finding Micom Bootloader...Not found\r\n");
				}
			}
			continue;
		case 4 :
			PrintDebug("Finding Micom Bootloader...Found\r\n");
			ucStep=5;
			continue;
		case 5 :
			PrintDebug("Eraseing Old Micom App\r\n");
			DRV_MICOM_TX_BYTE('e');
			VK_TASK_Sleep(1000);
			DRV_MICOM_RX_BYTE(&ucRxData);
			if (ucRxData!=0x0d)
			{
				ucRetry++;
				if (ucRetry>3)
				{
					PrintDebug("Something wrong with Flash Memory\r\n");
					PrintDebug("LD_UPGRADE_Update failed\r\n");
#if defined(LOADER_APP)
					UpdateDisplay(GetTextData(0, STR_ID_MICOM_ERASE_FAIL, NULL), "?", 0);
#endif
					ucRetry=0;
					ucStep=100;
				}
			}
			else
				ucStep=6;
			continue;

		case 6 :
			{
				HINT32 iResult;

				PrintDebug("Writing new APP...\n");
				iResult=writeFlashBlock(pucData, ulSize);
				if (iResult==ERR_MICOM_OK)
				{
					ucStep=8;
				}
				else
				{
					if (iResult==ERR_MICOM_TOO_BIG)
						PrintDebug("Err : Image is too big\r\n");
					else if (iResult==ERR_MICOM_CHECKSUM)
						PrintDebug("Err : CheckSum\r\n");
					else
						PrintDebug("Err : Unknown reason\r\n");
					ucStep=7;
#if defined(LOADER_APP)
					UpdateDisplay(GetTextData(0, STR_ID_MICOM_WRITE_FAIL, NULL), "?", 0);
#endif
				}
				continue;
			}

		case 7 :
			PrintDebug("Erasing the written APP...\n");
			DRV_MICOM_TX_BYTE('e');
			VK_TASK_Sleep(2);
			DRV_MICOM_RX_BYTE(&ucRxData);
			if (ucRxData!=0x0d)
			{
				ucRetry++;
				if (ucRetry>3)
				{
					PrintDebug("Something wrong with Flash Memory\n");
					PrintDebug("LD_UPGRADE_Update failed\n");
					ucRetry=0;
					ucStep=100;
				}
			}
			else
				ucStep=100;
			continue;

		case 8 :
			iResult=DI_ERR_OK;
			PrintDebug("Starting APP...\n");
			DRV_MICOM_TX_BYTE('E');
			VK_TASK_Sleep(2);
			ucStep=100;
			continue;
		case 100 :
		default :
			break;
		}
		break;
	}
	return iResult;
}

#define _______________________________________________________________________
#define ____GLOBAL_FUNCTION_BODY____
extern unsigned long s_ulMicomTxRxSema;

DRV_Error DRV_MICOM_Update(HUINT8 *pMicomImage, HUINT32 binarySize)
{
	HUINT32			nRetryCount;
	HUINT8			ucRxData=0xff;
	HUINT8			ucBuff;
	DI_ERR_CODE		error;
	int			i = 0;

	VK_SEM_Get(s_ulMicomTxSema);
	VK_SEM_Get(s_ulMicomRxSema);
	for (nRetryCount=0 ; nRetryCount<10 ; nRetryCount++)
	{
		/* Clean recieve buffer */
		for(i = 0; i < 10; i++)
		{
			DRV_MICOM_RX_BYTE(&ucBuff);
		}

		DRV_MICOM_TX_BYTE('!');
		VK_TASK_Sleep(2);
		DRV_MICOM_RX_BYTE(&ucRxData);
		if (ucRxData=='?')
		{
			// micom is already in the download mode.
			break;
		}
		else
		{
			// perhaps, micom's app is running. Let it reset itself and get into the download mode.
			DRV_MICOM_GetReadyForDownload();
			VK_TASK_Sleep(50); // Wait until the micom resets itself; about 50ms
		}
	}

	if (nRetryCount>=10)
	{
		PrintError("Micom doesn't get into the download mode\r\n");
		VK_SEM_Release(s_ulMicomRxSema);
		VK_SEM_Release(s_ulMicomTxSema);
		return 1;
	}

	error = UpgradeToMicom(0, pMicomImage, binarySize);
	if (error)
	{
		PrintError("DRV_ERR_MICOM_DOWNLOAD\r\n");
		VK_SEM_Release(s_ulMicomRxSema);
		VK_SEM_Release(s_ulMicomTxSema);
		return error;
	}

	VK_SEM_Release(s_ulMicomRxSema);
	VK_SEM_Release(s_ulMicomTxSema);
	return DI_ERR_OK;
}

