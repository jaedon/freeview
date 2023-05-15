/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: server.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/28/09 2:41p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/appframework/src/linuxclient/server/server.h $
 * 
 * Hydra_Software_Devel/1   4/28/09 2:41p jessem
 * PR 54380: Initial version.
 *
 ***************************************************************************/
#ifndef SERVER_H__
#define SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "berr.h"
#include "bstd_defs.h"

/* Helper macros */
#if BDBG_DEBUG_BUILD
#define BEMU_SERVER_P_GENERIC_GET_CONTEXT(handle, context, structname) \
do { \
	(context) = (handle); \
	BDBG_ASSERT(context); \
} while(0)
#else
#define BEMU_SERVER_P_GENERIC_GET_CONTEXT(handle, context, structname) \
	(context) = (handle)
#endif

#define BEMU_SERVER_P_GET_CONTEXT(handle, context) \
	BEMU_SERVER_P_GENERIC_GET_CONTEXT(handle, context, BEMU_P_Server_Context)
	
/* data types */
/***************************************************************************
Summary:
	The server settings

Description:
	This defines settings used by the server software.

	ulPort         - the port used by the server when opening a socket. 
	                 The client has to use this port.

See Also:
	BVDC_Open, BVDC_Close.
***************************************************************************/
typedef struct BEMU_Server_Settings
{
	uint32_t ulPort;
} BEMU_Server_Settings;

/* Forward declarations */
/***************************************************************************
Summary:
	The server context handle.

Description:
	This is an opaque handle that application created with BEMU_Open.
	BEMU_Server_Handle holds the context of the Server.  There should only one
	BEMU_Server_Handle per server.  

See Also:
	BEMU_Open, BEMU_Close.
***************************************************************************/
typedef struct BEMU_P_Server_Context       *BEMU_Server_Handle;

/***************************************************************************
Summary:
	The emulator context handle.

Description:
	This is an opaque handle that was created with BEMU_Server_Open.
	BEMU_Handle holds the context of the emulator.  There should only one
	BEMU_Handle per emulator.  

See Also:
	BEMU_Open, BEMU_Close.
***************************************************************************/
typedef struct BEMU_P_Context              *BEMU_Handle;

/***************************************************************************
Summary:
	The emulator settings.

Description:
	This must be defined by the actual emulator commnand API implementation.
	Its contents are at the defined by the API implementer and will be used
	only by the emulator it was designed for.
	
See Also:
	BEMU_Open, BEMU_Close, BEMU_GetSettings, BEMU_Server_GetSettings
***************************************************************************/
struct BEMU_Settings;


/***************************************************************************
Summary:
	This function opens and initializes the server and emulator.

Description:
	A handle to the emulator is created via a call to BEMU_Open. A socket 
	connection is established and will be in listening mode until a command 
	is received from the client which will then be relayed to the emulator. 
	Once the emulator completes the task, an acknowledgement will be sent 
	back to the client.

Input:
	pstSettings - The default setting that user want the emulator to be
	in. This parameter can be NULL. In this case emulator's inherent default
	structure will be used. This inherent structure could be queried prior
	to BEMU_Open with BEMU_GetSettings, modified and passed to 
	BEMU_Server_Open.

	pstServerSettings- The default setting that user want the server to be
	in. This parameter can be NULL. In this case server's inherent default
	structure will be used. This inherent structure could be queried prior
	to BEMU_Server_Open with BEMU_Server_GetSettings, modified and passed to 
	BEMU_Server_Open.

Output:
	phEmuServer - a reference to a Server handle.  Upon successful open this will
	reference to a fresh new Server handle (context).  If an error occurs
	during BEMU_Server_Open *phEmuServer be NULL.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:
	BEMU_Close, BEMU_GetSettings, BEMU_Server_GetSettings.
**************************************************************************/
BERR_Code BEMU_Server_Open(BEMU_Server_Handle *phEmuServer, const struct BEMU_Settings *pstSettings, const BEMU_Server_Settings *pstServerSettings);

/***************************************************************************
Summary:
	This function closes the server and emulator.

Description:
	The handle to the emulator created by BEMU_Server_Open is destroyed via
	call to BEMU_Close. The socket connection is destroyed as well.

Input:
	hEmuServer - a reference to a Server handle. 

Output:


Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:
	BEMU_Server_Open
**************************************************************************/
BERR_Code BEMU_Server_Close(BEMU_Server_Handle hEmuServer);

/***************************************************************************
Summary:
	This function gets the settings of the server and emulator

Description:
	This is used in conjunction with BEMU_Server_Open

Input:
	pstSettings - The current setting of the emulator.

	pstServerSettings- The current setting of the server.

Output:


Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:
	BEMU_Server_Open, BEMU_Settings, BEMU_Server_Settings
**************************************************************************/
void BEMU_Server_GetSettings(struct BEMU_Settings *pstSettings, BEMU_Server_Settings *pstServerSettings);
	
/***************************************************************************
Summary:
	This function opens and initializes the emulator.

Description:
 	A handle to the emulator is created. The settings passed in will be used
 	to configur ethe emulator accordinigly.

Input:
	pstSettings - The default setting that user want the emulator to be
	in. This parameter can be NULL. In this case emulator's inherent default
	structure will be used. This inherent structure could be queried prior
	to BEMU_Open with BEMU_GetSettings, modified and passed to 
	BEMU_Server_Open.

Output:
	phEmu - a reference to an emulator handle.  Upon successful open this will
	reference to a fresh new emulator handle (context).  If error occur during
	BEMU_Open *phEmu be NULL.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:
	BEMU_Close, BEMU_GetSettings, BEMU_Settings
**************************************************************************/
BERR_Code BEMU_Open(BEMU_Handle *phEmu, const struct BEMU_Settings *pstSettings);

/***************************************************************************
Summary:
	This function closes the emulator.

Description:
	The handle to the emulator created by BEMU_Open is destroyed. 

Input:
	hEmu - a reference to an emulator handle. 

Output:


Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:
	BEMU_Open
**************************************************************************/
BERR_Code BEMU_Close(BEMU_Handle hEmu);

/***************************************************************************
Summary:
	This function gets the settings of the emulator

Description:
	This is used in conjunction with BEMU_Open

Input:

Output:
	pstSettings - The current setting of the emulator.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:
	BEMU_Open, BEMU_Settings
**************************************************************************/
void BEMU_GetSettings(struct BEMU_Settings *pSettings);

/***************************************************************************
Summary:
	This function reads a register

Description:

Input:
	ulAddr      - The register address (offset)
	
Output:
	pulData     - Contains the value stored in the register

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:

**************************************************************************/
BERR_Code BEMU_RegisterRead(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t *pulData);

/***************************************************************************
Summary:
	This function writes to a register

Description:

Input:
	ulAddr      - The register address (offset)
	ulData      - The value to store in the register
	
Output:

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:

**************************************************************************/
BERR_Code BEMU_RegisterWrite(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t ulData);

/***************************************************************************
Summary:
	This function reads a range of memory addreses

Description:

Input:
	ulAddr      - The start address of the memory range to read from
	ulSize      - The number of values to read
	
Output:
	pucData     - Contains the values read from the memory range

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:

**************************************************************************/
BERR_Code BEMU_MemoryRead(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t ulSize, uint8_t *pucData);

/***************************************************************************
Summary:
	This function writes to a range of memory addreses

Description:

Input:
	ulAddr      - The start address of the memory range to write to
	ulSize      - The number of values to write
	pucData     - Contains the values written to memory 
	
Output:

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:

**************************************************************************/
BERR_Code BEMU_MemoryWrite(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t ulSize, uint8_t *pucData); 

/***************************************************************************
Summary:
	This function interprets a command and executes the command accordingly

Description:
	The command passed in is determined from a preset list of commands that 
	are available on a per emulator basis. Examples of this command can be 
	to start a capture from a compositor or to use a set audio parameters. 
	

Input:
	ulCommand   - The command to execute
	iSize       - The size of data associated with the command
	pilData     - Contains the data associated with the command
	
Output:

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened the server.

See Also:

**************************************************************************/
BERR_Code BEMU_Command(BEMU_Handle hEmu, uint32_t ulCommand, int iSize, int32_t *pilData);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SERVER_H__ */

