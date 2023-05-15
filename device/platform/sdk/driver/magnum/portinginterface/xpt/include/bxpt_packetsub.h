/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_packetsub.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 6/18/12 10:53a $
 *
 * Porting interface code for the packet substitution section of the
 * data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_packetsub.h $
 * 
 * Hydra_Software_Devel/6   6/18/12 10:53a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * Hydra_Software_Devel/5   1/12/12 2:40p gmullen
 * SW7425-2138: Fixed incorrect parser band index
 * 
 * Hydra_Software_Devel/4   1/11/12 10:32a gmullen
 * SW7425-2138: Added BXPT_HAS_PB_PACKETSUB
 * 
 * Hydra_Software_Devel/3   12/15/11 9:50a gmullen
 * SW7425-1589: Merged to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7425-1589/1   12/12/11 4:35p gmullen
 * SW7425-1589: Added BXPT_PacketSub_GetEobIntId()
 * 
 * Hydra_Software_Devel/2   1/25/11 3:11p rjlewis
 * SW7420-879: supply a means to provide an output rate default on open.
 * Get defaults provides the default value.
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/2   10/25/10 9:11a gmullen
 * SW7425-15: Updated headers and added stub versions of C files
 * 
 * Hydra_Software_Devel/1   10/8/10 2:57p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/9   8/18/10 7:55a gmullen
 * SW7403-924: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7403-924/2   8/11/10 10:53a gmullen
 * SW7403-924: New method: change band_num in PSUB
 * 
 * Hydra_Software_Devel/SW7403-924/1   8/9/10 9:07a gmullen
 * SW7403-924: See the JIRA for the workaround notes
 * 
 * Hydra_Software_Devel/8   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/7   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/6   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/5   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/4   3/26/08 11:24a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/3   11/28/07 11:24a gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/2   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/1   5/13/05 1:27p gmullen
 * PR15309: First draft of XPT PI.
 * 
 * Hydra_Software_Devel/14   10/4/04 3:26p gmullen
 * PR12902: Added requested function
 * 
 * Hydra_Software_Devel/13   3/25/04 11:15a gmullen
 * PR8852: Checked for duplicate PID usage in record, remux, and hsx.
 * 
 * Hydra_Software_Devel/12   2/8/04 5:21p gmullen
 * PR9210: Changed includes to comply.
 * 
 * Hydra_Software_Devel/11   1/19/04 2:54p gmullen
 * PR8852: Changed definition of _SetOutputRate(). RDB description is
 * confusing.
 * 
 * Hydra_Software_Devel/10   1/5/04 2:49p gmullen
 * PR9142: Fixed compiler warnings.
 * 
 * Hydra_Software_Devel/9   1/5/04 12:21p gmullen
 * PR8852: Moved BandNum from ChannelSettings to SetPidChanNum()
 * 
 * Hydra_Software_Devel/8   12/4/03 5:32p gmullen
 * PR8729: Added prototype for
 * BXPT_PacketSub_GetChannelDefaultSettings().
 * 
 * Hydra_Software_Devel/7   10/3/03 5:06p gmullen
 * Updated function prototypes.
 * 
 * Hydra_Software_Devel/6   9/26/03 4:50p gmullen
 * Replaced stubs with real code.
 * 
 * Hydra_Software_Devel/5   9/8/03 9:21a gmullen
 * Updated for channel-based API.
 * 
 * Hydra_Software_Devel/4   8/14/03 4:06p gmullen
 * Replaced Get/Set calls.
 * 
 * Hydra_Software_Devel/3   7/31/03 10:55a gmullen
 * Changed 'Description' fields in enum and structure comments to
 * 'Summary' so they'll show up in auto-generated docs.
 * 
 * Hydra_Software_Devel/2   7/30/03 6:35p gmullen
 * Added comments to enums
 * 
 * Hydra_Software_Devel/1   7/25/03 3:39p gmullen
 * Initial release for 7038
 *
 ***************************************************************************/

/*= Module Overview *********************************************************
The packet substitution module provides a method to adjust the contents of 
PIDs carrying PSI or SI information, after packets have been removed by the 
PID parsers. Every time packet is removed, a 'gap' is created in the stream
where the packet used to be. The packet substitution logic can insert new
data into those gaps. The modified stream that is created is available to 
all other blocks in the transport core, and can be treated like any data
packet stream.
***************************************************************************/

#ifndef BXPT_PACKETSUB_H__
#define BXPT_PACKETSUB_H__

#include "bxpt.h"

/***************************************************************************
Summary:
Defines the DMA priority levels a packet sub channel can use. A channel with
HIGH priority will output all its packets before a channel with LOW priority 
will be allowed to output any.
****************************************************************************/
typedef enum BXPT_PacketSubDmaPriority
{
    BXPT_PacketSubDmaPriority_eHigh = 1,			
    BXPT_PacketSubDmaPriority_eLow = 0
}
BXPT_PacketSubDmaPriority;

/***************************************************************************
Summary:
Handle for accessing the packet substitution API via a channel. Users should 
not directly access the contents of the structure.
****************************************************************************/
typedef struct BXPT_P_PacketSubHandle *BXPT_PacketSub_Handle;

/***************************************************************************
Summary:
Settings for the Playback channels. These values can be passed in when the
channel is opened, and will be used as the new default settings.
****************************************************************************/
typedef struct BXPT_PacketSub_ChannelSettings
{
	unsigned int	PacketLen;							/* Packet length. */

#if BXPT_HAS_FIXED_PSUB_DMA_PRIORITY
	/*
	** Insert packet in gap between upstream data. When enabled, forced packet 
	** insertion mode for this pasrser band is enabled and normal substitution 
	** of null packet for this band is disabled. Output rate control is the 
	** same for forced packet insertion mode and normal substitution mode. 
	*/
	bool ForcedInsertionEn;	
#else
	BXPT_PacketSubDmaPriority DmaPriority;	/* Priority for this channel. */
#endif

	/* Provides a default rate for packet insertion. */
	uint32_t OutputRate;
}
BXPT_PacketSub_ChannelSettings;

/***************************************************************************
Summary:
Layout of a packet sub descriptor. The order of the members matches the order 
the hardware will expect them in when the descriptor is read from memory. 
Thus, the descriptor's contents can be accessed by creating a pointer to this 
type of struct and setting that pointer to the address of the real descriptor
in memory (after being memory mapped, of course).
****************************************************************************/
typedef struct BXPT_PacketSub_Descriptor
{
	uint32_t	BufferStartAddr;	/* (Pointer to) the buffer associated with this descriptor. */
	uint32_t	BufferLength;		/* Length (in bytes) of the buffer. */
	uint32_t	Flags;				/* Channel-specific flags. */
	uint32_t	NextDescAddr;		/* (Pointer to) the next descriptor in the chain. */
} 
BXPT_PacketSub_Descriptor;

/***************************************************************************
Summary: 
Status bits for the packet substitution channel. 
****************************************************************************/
typedef struct BXPT_PacketSub_ChannelStatus
{
	bool Finished;		/* true if channel has processed last descriptor in the chain. */
	bool Busy;			/* true if channel is active. */
	bool Run;			/* true if a playback session has been started. */
}
BXPT_PacketSub_ChannelStatus;

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Return the number of packet substitution channels.

Description:
For the given transport core, return the number of packet substitution channels 
that is supported. 

Returns:
    BERR_SUCCESS                - Retrieved channel count.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PacketSub_GetTotalChannels(
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int *TotalChannels		/* [out] The number of PacketSub channels. */
	);	

/***************************************************************************
Summary:
Open a given packet substitution channel.

Description:
Reset the given playback channel, configure it using the given defaults, then
return a handle to access that channel.

Returns:
    BERR_SUCCESS                - Packet substitution channel opened.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PacketSub_OpenChannel(
	BXPT_Handle hXpt, 	   						/* [in] Handle for this transport */
	BXPT_PacketSub_Handle *hPSub,				/* [out] Handle for opened packet sub channel */
	unsigned int ChannelNo,							/* [in] Which channel to open. */
	BXPT_PacketSub_ChannelSettings *ChannelSettings /* [in] The defaults to use */
	);	

/***************************************************************************
Summary:
Close a given packet sub channel.

Description:
Shutdown a given playback channel, disable any interrupts from it. 

Returns:
	void
****************************************************************************/
void BXPT_PacketSub_CloseChannel(
	BXPT_PacketSub_Handle hPSub	/* [in] Handle for the channel to close*/
	);	

/***************************************************************************
Summary:
Get the default settings for a given packet sub channel.

Description:
Retrieves the default values used when opening a packet sub channel. The user
can change the defaults, then pass the new settings in when 
BXPT_PacketSub_OpenChannel() is called.

Returns:
    BERR_SUCCESS                - Call completed successfully.
    BERR_INVALID_PARAMETER      - Bad channel number
****************************************************************************/
BERR_Code BXPT_PacketSub_GetChannelDefaultSettings(
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int ChannelNo,			/* [in] Which channel to get defaults from. */
	BXPT_PacketSub_ChannelSettings *ChannelSettings /* [out] The defaults */
	);

/***************************************************************************
Summary:
Set the ouput PID channel number. 

Description:
Set the PID channel number that the substituted packets will be	given. 

All chips support substitution into PID channels from live feeds (via the 
front end parsers). On some chips, substitution from playback feeds is also 
supported. Such chips have BXPT_HAS_PB_PACKETSUB defined for them in bxpt.h . 
To use playback feeds, add the BXPT_PB_PARSER macro like so: 
 
    BXPT_PacketSub_SetPidChanNum( hPSub, PidChanNum, BXPT_PB_PARSER( BandNum ) );
 
BandNum should be set to the index of the playback channel that the PID channel 
is mapped to, i.e. BandNum should be 1 if playback 1 is used.  

Returns:
    BERR_SUCCESS                - Call completed successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PacketSub_SetPidChanNum( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	unsigned int PidChanNum,		/* [in] Which PID channel to assign the output to. */
	unsigned int BandNum			/* [in] Which band number to assign the output to */
	);

/***************************************************************************
Summary:
Get the ouput PID channel number. 

Description:
On certain chips (the 7403 right now), only certain PID channels can 
be used with the PSUB hardware. This API will return the correct PID channel
number for the given PSUB. 

Returns:
    unsigned int - PID channel number to be used with this PacketSub block.
****************************************************************************/
unsigned int BXPT_PacketSub_GetPidChanNum( 
	BXPT_PacketSub_Handle hPSub 	/* [in] Handle for the channel. */
	);

/***************************************************************************
Summary:
Force immediate output. 

Description:
Substitution data is normally inserted on a space-available basis, meaning 
the PID parsers must drop a packet from the selected output band to make room 
for a substitution packet. This function forces immediate insertion for the 
given channel. For a live stream, forced insertion means that input packets 
are dropped immediatly to make room. For a playback stream with pacing, the 
playback channel will be paused to allow the insertion. Playback without 
pacing is handled like a live stream.

Returns:
    BERR_SUCCESS                - Immediate output enabled.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_SetFullRateOutput, BXPT_PacketSub_SetOutputRate
****************************************************************************/
BERR_Code BXPT_PacketSub_SetForcedOutput( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Enable			/* [in] Force output immediately if TRUE */
	);

/***************************************************************************
Summary:
Set output rate equal to input rate. 

Description:
Ignore the programmable output rate that was set through 
BXPT_PacketSub_SetOutputRate(), and make the output rate equal to the input 
rate. Disabling this feature will return to the programmable rate.

Returns:
    BERR_SUCCESS                - Output rate now matches input.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_SetForcedOutput, BXPT_PacketSub_SetOutputRate
****************************************************************************/
BERR_Code BXPT_PacketSub_SetFullRateOutput( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Enable			/* [in] Use full rate if TRUE */
	);

/***************************************************************************
Summary:
Set output rate to the user-supplied value. 

Description:
Set packet output rate, expressed in bits/second. Unless overridden by 
BXPT_PacketSub_SetFullRateOutput(), the output rate for substituted packets
will the given bitrate, rounded to the nearest 1600 bits/second.

Returns:
    BERR_SUCCESS                - Output rate now set.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_SetFullRateOutput, BXPT_PacketSub_SetForcedOutput
****************************************************************************/
BERR_Code BXPT_PacketSub_SetOutputRate( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	uint32_t OutputRate	/* [in] The output rate, in bits/second */
	);

/***************************************************************************
Summary:
Pause packet substitution.

Description:
Pause the packet substitution channel. Packet subbing stops until this 
function is called again to disable the pause.

Returns:
    BERR_SUCCESS                - Paused enabled or disabled.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PacketSub_PauseChannel( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Pause			/* [in] Pause channel if TRUE, continue if FALSE */
	);

/***************************************************************************
Summary:
Initialize a descriptor for the packet substitution linked list.

Description:
Initialize the contents of a packet sub descriptor. The caller passes in the 
starting address of the buffer that the descriptor will point to. 
The buffer can start at any byte address, and may be of any length. 

The caller specifies if an interrupt should be generated when the
hardware has finished processing the contents of the descriptor's
buffer. 

The memory for the descriptor must be allocated by the caller, and must 
start on a 16-byte boundary. 

Since descriptors are usually used in chains, the function allows the 
descriptor's NextDescAddr to be initialized along with the rest of the 
fields. This next descriptor does not have to be initialized itself. The
address must meet the 16-byte boundary requirement.

Returns:
    BERR_SUCCESS                - Descriptor initialized.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_AddDescriptors, BXPT_PacketSub_CheckHeadDescriptor
****************************************************************************/
BERR_Code BXPT_PacketSub_CreateDesc( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	BXPT_PacketSub_Descriptor * const Desc,		/* [in] Descriptor to initialize */ 
	uint8_t *Buffer, 							/* [in] Data buffer. */
	uint32_t BufferLength, 						/* [in] Size of buffer (in bytes). */
	bool IntEnable, 							/* [in] Interrupt when done? */
	BXPT_PacketSub_Descriptor * const NextDesc 	/* [in] Next descriptor, or NULL */
	);

/***************************************************************************
Summary:
Add a descriptor to a packet substitution linked list.

Description:
Add a descriptor chain to an existing (but possibly empty) chain used by a
packet sub channel. If the existing chain is empty, the chain being added 
simply becomes the current chain. 

The FirstDesc argument is a pointer to the descriptor at the start of the 
chain being added. The LastDesc argument passed in is a pointer to the end 
of that chain.

Returns:
    BERR_SUCCESS                - Descriptor added to the list.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_CreateDesc, BXPT_PacketSub_CheckHeadDescriptor
****************************************************************************/
BERR_Code BXPT_PacketSub_AddDescriptors( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	BXPT_PacketSub_Descriptor *LastDesc, 	/* [in] Last descriptor in new chain */
	BXPT_PacketSub_Descriptor *FirstDesc 	/* [in] First descriptor in new chain */
	);

/***************************************************************************
Summary:
Return the address of the current packet sub descriptor.

Description:
For a given packet substitution channel, get the address of the descriptor 
that is currently being used by the hardware. 

Returns:
    BERR_SUCCESS                - Retrieved address from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PacketSub_GetCurrentDescriptorAddress(
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	BXPT_PacketSub_Descriptor **LastDesc		/* [in] Address of the current descriptor. */
	);

/***************************************************************************
Summary:
Determine if a descriptor is the head of a linked list.

Description:
Determine if the current descriptor being processed by the packet sub channel 
is the first on the channel's chain (which means this descriptor is still 
being used). For packet sub channels that are still 'busy', the size of the 
descriptor's buffer is also returned. This value should be ignore for all 
other types of channels.

Returns:
    BERR_SUCCESS                - No errors occurred during the check.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_CreateDesc, BXPT_PacketSub_CheckHeadDescriptor
****************************************************************************/
BERR_Code BXPT_PacketSub_CheckHeadDescriptor( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	BXPT_PacketSub_Descriptor *Desc, 	/* [in] Descriptor to check. */
	bool *InUse, 						/* [out] Is descriptor in use? */
	uint32_t *BufferSize 				/* [out] Size of the buffer (in bytes). */
	);

/***************************************************************************
Summary:
Begin packet substitution on the specified channel.

Description:
Start a packet sub channel. If no buffers have been added to the packet sub
channel, the hardware is not actually started until the first buffer is added.

Note: the caller should install an interrupt handler prior to calling this
function. That handler will be called when a descriptor's data has been 
processed if the IntEnable bit in the descriptor was set.

Returns:
    BERR_SUCCESS                - Channel started.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_StopChannel, BXPT_PacketSub_PauseChannel
****************************************************************************/
BERR_Code BXPT_PacketSub_StartChannel( 
	BXPT_PacketSub_Handle hPSub	/* [in] Handle for the channel. */
	);

/***************************************************************************
Summary:
Stop a packet sub channel. 

Description:
Stop packet substitution on the specified channel.The interrupts for the 
given channel are disabled. 

Returns:
    BERR_SUCCESS                - Channel stopped.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_PacketSub_StartChannel, BXPT_PacketSub_PauseChannel
****************************************************************************/
BERR_Code BXPT_PacketSub_StopChannel( 
	BXPT_PacketSub_Handle hPSub	/* [in] Handle for the channel. */
	);

/***************************************************************************
Summary: 
Get the packet sub channels status bits.

Description:
Return the packet sub channels FINISHED, RUN, and BUSY status bits. The bits
are read directly from the hardware.

Returns:
    BERR_SUCCESS                - Settings read from hardware successfully.
****************************************************************************/
BERR_Code BXPT_PacketSub_GetChannelStatus(
	BXPT_PacketSub_Handle hPSub,			/* [in] Handle for the channel. */
	BXPT_PacketSub_ChannelStatus *Status 	/* [out] Channel status. */
	);

/***************************************************************************
Summary: 
Return BINT_Id for the packetsub end-of-buffer interrupt. 

Description:
The result is a BINT_Id value suitable for use in BINT_CreateCallback(). 

Returns:
    BERR_SUCCESS                - Id generated successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter  
****************************************************************************/
BERR_Code BXPT_PacketSub_GetEobIntId(
	BXPT_PacketSub_Handle hPSub,			/* [in] Handle for the channel. */
    BINT_Id *IntId
    );

#if BXPT_HAS_PACKETSUB_FORCED_INSERTION
/***************************************************************************
Summary: 
Insert a packet in any gap between upstream packet. 
 
Description:
This API provides the same functionality as ForcedInsertionEn in the 
BXPT_PacketSub_ChannelSettings struct. It is provided for cases where setting 
ForcedInsertionEn at open is inconvenient.  
 
Returns:
    BERR_SUCCESS                - Id generated successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter  
****************************************************************************/
BERR_Code BXPT_PacketSub_SetForcedInsertion( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Enable			/* [in] */
	);
#endif
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
typedef struct
{
    unsigned BandNum[ BXPT_P_MAX_PACKETSUBS ];
}
BXPT_P_PacketSubCfg;
void BXPT_PacketSub_P_SaveCfg( 
    BXPT_Handle hXpt, 
    unsigned int PidChannelNum, 
    BXPT_P_PacketSubCfg *PsubCfg 
    );
void BXPT_PacketSub_P_RestoreCfg( 
    BXPT_Handle hXpt, 
    unsigned int PidChannelNum, 
    BXPT_P_PacketSubCfg *PsubCfg 
    );
#endif
#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_PACKETSUB_H__ */

/* end of file */
