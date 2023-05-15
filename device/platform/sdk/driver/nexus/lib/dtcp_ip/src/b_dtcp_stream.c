/********************************************************************************************
*     (c)2004-2012 Broadcom Corporation                                                     *
*                                                                                           *
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,   *
*  and may only be used, duplicated, modified or distributed pursuant to the terms and      *
*  conditions of a separate, written license agreement executed between you and Broadcom    *
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants*
*  no license (express or implied), right to use, or waiver of any kind with respect to the *
*  Software, and Broadcom expressly reserves all rights in and to the Software and all      *
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU       *
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY                    *
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.                                 *
*
*  Except as expressly set forth in the Authorized License,                                 *
*
*  1.     This program, including its structure, sequence and organization, constitutes     *
*  the valuable trade secrets of Broadcom, and you shall use all reasonable efforts to      *
*  protect the confidentiality thereof,and to use this information only in connection       *
*  with your use of Broadcom integrated circuit products.                                   *
*                                                                                           *
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"          *
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR                   *
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO            *
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES            *
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,            *
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION             *
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF              *
*  USE OR PERFORMANCE OF THE SOFTWARE.                                                      *
*                                                                                           *
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS         *
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR             *
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR               *
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF             *
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT              *
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE            *
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF              *
*  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: b_dtcp_stream.c $
 * $brcm_Revision: 23 $
 * $brcm_Date: 9/11/12 2:52p $
 * 
 * Module Description:
 *     DTCP Streaming functions.
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/dtcp_ip/src/b_dtcp_stream.c $
 * 
 * 23   9/11/12 2:52p leisun
 * SWSECURITY-48: Auto-generating test keys, consolidate test/production
 *  build, make common DRM default ON
 * 
 * 22   5/5/11 3:04p leisun
 * SWSECURITY-49: Update Nonce value change period, other device modes
 *  support
 * 
 * 21   2/11/11 5:39p leisun
 * SWSECURITY-32: Updated lib to V1SE1.3 conformance.
 * 
 * 20   3/1/10 2:29p leisun
 * SW7420-561: Update from plugfest bugs
 * 
 * 19   12/11/09 3:55p ssood
 * SW7420-502: Add option to allow apps to indicate to DTCP/IP library to
 *  generate PCP for every encrypted data blob
 * 
 * 18   10/12/09 4:22p leisun
 * SW7405-3138: Fix for HTTP live streaming, crash when killing client
 * 
 * 17   10/6/09 1:48p leisun
 * SW7405-3138: Fixes for live HTTP streaming
 * 
 * 16   10/6/09 1:44p leisun
 * SW7405-3138: Fixes for live HTTP streaming
 * 
 * 15   9/11/09 11:42a leisun
 * SW7405-3013: coverity defect report fix
 * 
 * 14   8/10/09 1:40p lwhite
 * PR55693: Fix wrap padding
 * 
 * 13   8/7/09 5:33p lwhite
 * PR55693: PCP 14 byte fix
 * 
 * 12   7/28/09 3:33p lwhite
 * PR55693: Fixed DTCP/IP close
 * 
 * 11   7/16/09 7:02p lwhite
 * PR55693: Fixed signedness
 * 
 * 10   7/14/09 2:11p leisun
 * PR 55693: Fix segfault bug when switching channel using brutus
 * 
 * 9   7/13/09 11:19a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 * 
 * 7   7/1/09 4:32p leisun
 * PR 55693: added GetNewExchKey function, fixed linked list deletion
 *  problem, etc
 * 
 * 6   6/30/09 10:43p leisun
 * PR 55693: Added exchange key expiration timer, and other fixes
 * 
 * 5   6/25/09 4:32p leisun
 * PR 55693: Fix memory leak for DTCP
 * 
 * 4   6/25/09 9:55a ssood
 * PR53773: turning off DTCP debug logs
 * 
 * 3   6/24/09 3:31p leisun
 * PR 55693: checking in DTCP-IP lib, fixed PCP handling error when stream
 *  larger then max PCP size
 * 
 * 2   6/11/09 10:54a leisun
 * PR 55907: Add HW M2M feature for DTCP-IP
 * 
 * 1   6/5/09 4:07p leisun
 * PR 55693: checking in DTCP-IP lib
 *********************************************************************************************/
#include <stdio.h>
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_ake.h"
#include "b_dtcp_ip_ake.h"
#include "b_dtcp_status_codes.h"
#include "b_dtcp_stream.h"
#include "b_dtcp_content.h"

BDBG_MODULE(b_dtcp_ip);

/*#define DUMP_STREAM		1 */

static int pcpNum;
#ifdef DUMP_STREAM
static FILE * fout;
#endif

#define MIN(a, b) ((a > b)? b : a)

/*! \brief return proper EMI value for a stream given the content stream's CCI value and device mode.
 * \param[in] Session AKE session data.
 * \param[in] cci content stream's CCI value.
 * \param[in] content_type stream's content type.
 * \param[out] returned emi value.
 */
static void B_DTCP_IP_GetEmiFromCCI(B_AkeCoreSessionData_T *Session, int cci, int content_type, int *emi)
{
	switch (Session->DeviceMode) {
		case B_DeviceMode_eSource:
			/* Format-non-cognizant source function*/
			if (cci == B_CCI_eCopyNever)
				*emi = B_ExtEmi_eCopyNever;
			else if (cci == B_CCI_eCopyOneGeneration) {
				/* NOTE1: we set the emi to COG [Format-cognizant recording only].
				 * If it's desired that this stream is recordable by 
				 * Format-non-cognizant device, reset it later by API.
				 */
				*emi = B_ExtEmi_eCopyOneFC;
			} else if (cci == B_CCI_eNoMoreCopy)
				*emi = B_ExtEmi_eNoMoreCopy;
			else if (cci == B_CCI_eCopyFree)
				*emi = B_ExtEmi_eCopyFree;
			else
				*emi = B_ExtEmi_eInvalid;
			break;
		case B_DeviceMode_eFCSource:
			/*Format-cognizant source function */
			if (cci == B_CCI_eCopyNever)
				*emi = B_ExtEmi_eCopyNever;
			else if (cci == B_CCI_eCopyOneGeneration &&
					content_type == B_Content_eAudioVisual) {
				/* See NOTE1 above */
				*emi = B_ExtEmi_eCopyOneFC;
			} else if (cci == B_CCI_eNoMoreCopy)
				*emi = B_ExtEmi_eNoMoreCopy;
			else if (cci == B_CCI_eCopyFree)
				*emi = B_ExtEmi_eCopyFree;
			else
				*emi = B_ExtEmi_eInvalid;
			break;
		case B_DeviceMode_eAudioFCSource:
			/*NOTE2: for autio, copy-never is Type Specific!.*/
			if (cci == B_Audio_CCI_eCPT)
				*emi = B_ExtEmi_eCopyOneFC;
			else if (cci == B_Audio_CCI_eNoMoreCopy)
				*emi = B_ExtEmi_eNoMoreCopy;
			else if (cci == B_Audio_CCI_eCopyFree)
				*emi = B_ExtEmi_eCopyFree;
			else
				*emi = B_ExtEmi_eInvalid;
			break;
		default:
			*emi = B_ExtEmi_eInvalid;
	}

}
/*! \brief exported function to set the stream attribute, when PCP_UR is used.
 *  only set these attribute for source stream.
 *  \param[in] stream source stream handle.
 *  \param[in] content_has_cci, a flag indicate the content has embedded CCI.
 *  \param[in] content_type type of the content,e.g. AudioVisual, type1Audio, etc.
 *  \param[in] aps Analog copy protection information obtained from embedded CCI.
 *  \param[in] ict Image_Constraint_Token obtained from CCI.
 *  \param[in] ast Analog_sunset_token obtained from CCI.
 */
void B_DTCP_IP_SetSourceStreamAttribute(B_StreamHandle_T stream, bool content_has_cci, 
		int content_type, int aps, int ict, int ast)
{
	BDBG_ASSERT(stream);
	stream->content_type = content_type;
	/* DTCP-IP V1SE1.31, section 4.23.3*/
	if (content_type == B_Content_eMPEG_TS ||
			content_type == B_Content_eType2Audio ||
			content_type == B_Content_eMultiplex) {
		BDBG_WRN(("Setting PCP_UR to ZERO\n"));
		return ;
	}
	/* 
	 * V1SE 1.31, section 4.23.3, use UR_MODE 01 if aps, ict, ast available 
	 * from embedded CCI.
	 */
	if (content_has_cci == true)
		stream->ur_mode = B_UR_Mode_eCCI;
	else
		stream->ur_mode = B_UR_Mode_eNoCCI;
	if (content_type == B_Content_eAudioVisual) {
		stream->aps = aps;
		stream->ict = ict;
		stream->ast = ast;
	}else if (content_type == B_Content_eType1Audio) {
		stream->aps = 0;
		stream->ict = 0;
		stream->ast = 0;
	}
}
/*! \brief exported function to check the sink stream's attribute, to determine 
 * if processing (decryption) is allowed.
 * \param[in] akeHandle AKE session handle.
 * \param[in] stream stream handle.
 * \param[in/out] content_type stream content type.
 * \param[in/out] aps analog copy right information.
 * \param[in/out] ict image constraint token.
 * \param[in/out] ast analog sunset token.
 */
BERR_Code B_DTCP_IP_GetSinkStreamAttribute(B_AkeHandle_T akeHandle, B_StreamHandle_T stream, 
		int *content_type, int *aps, int *ict, int *ast)
{
	B_AkeCoreSessionData_T *Session = (B_AkeCoreSessionData_T *)akeHandle;
	BDBG_ASSERT(Session);

	if (Session->pcp_ur_cap == 0)
		return BERR_NOT_SUPPORTED;
	*content_type = stream->content_type;
	*aps = stream->aps;
	*ict = stream->ict;
	*ast = stream->ict;

	return BERR_SUCCESS;
}

/*! \brief set the stream's emi value, if caller wish to override the emi value
 *  obtained from calling GetEmiFromCCI() function.
 *  \param[in] stream stream handle.
 *  \param[in] emi emi value to set.
 */
void B_DTCP_IP_SetSourceStreamEmi(B_StreamHandle_T stream, int emi)
{
	BDBG_ASSERT(stream);
	stream->Emi = emi;
}
/*! \brief Get the sink stream's emi value
 *  \param[in] stream stream handle.
 *  \param[in] emi emi value to set.
 */
int B_DTCP_IP_GetSinkStreamEmi(B_StreamHandle_T stream)
{
	BDBG_ASSERT(stream);
	return	stream->Emi;
}
/*! \brief Get the DTCP descriptor based on stream info, to insert into PMT of MPEG_TS stream.
 * \param[in] stream stream handle.
 * \param[in] buf buffer to store the dtcp descriptor.
 * \param[in] length length of the buffer.
 */
BERR_Code B_DTCP_IP_GetDescriptor(B_StreamHandle_T stream, unsigned char *buf, int length)
{
	struct __b_dtcp_descriptor *descriptor;
	struct __b_dtcp_descriptor_private_data *private_data;
	BDBG_ASSERT(buf);
	BDBG_ASSERT(stream);

	if (length < DTCP_DESCRIPTOR_LENGTH)
		return BERR_INVALID_BUFFER_LENGTH;

	if (stream->content_type != B_Content_eMPEG_TS)
		return BERR_NOT_SUPPORTED;

	descriptor = (struct __b_dtcp_descriptor *)buf;
	descriptor->descriptor_tag = 0x88;
	descriptor->descriptor_length = DTCP_DESCRIPTOR_LENGTH;
	descriptor->CA_System_ID = 0x0fff;

	/*NOTE: We only considered the MPEG_TS video stream here.*/
	private_data = (struct __b_dtcp_descriptor_private_data *)descriptor->private_data;
	private_data->Retention_Move_mode = 1;
	private_data->DTCP_CCI = stream->cci;
	private_data->Image_Constraint_Token = stream->ict;
	private_data->APS = stream->aps;

	return BERR_SUCCESS;

}

/*! \brief function pointer to a function to open a source stream, after AKE.
 *  \param[in] akeHandle handle to AKE session.
 *  \param[in] Transport the stream's transport type.
 *  \param[in] content_length length of the content to be transmited.
 *  \param[in] content_type type of the content, e.g. AudioVisual/Type1_Audio.
 *  \param[in] cci embedded cci(copy control information) of this stream.
 *  \param[in] max_packet_size maximum packet size (pcp length).
 */
B_StreamHandle_T B_DTCP_IP_OpenSourceStream(B_AkeHandle_T akeHandle, B_StreamTransport_T Transport,
		int content_length, int cci, int content_type, int max_packet_size)
{
	struct __b_dtcp_stream_data * StreamData = NULL;
	B_AkeCoreSessionData_T * Session = (B_AkeCoreSessionData_T *)akeHandle;
	BDBG_ASSERT(content_length);

	BDBG_ENTER(B_DTCP_IP_OpenSourceStream);
	
	if(Session == NULL) {
		BDBG_ERR(("Invalid AKE handle!\n"));
		return NULL;
	}
	
	if((StreamData = (struct __b_dtcp_stream_data *)BKNI_Malloc(sizeof(struct __b_dtcp_stream_data)))
			== NULL)
	{
		BDBG_ERR(("Failed to allocate memory for stream : size=%d\n", sizeof(struct __b_dtcp_stream_data)));
		return NULL;
	}else {
		BKNI_Memset(StreamData, 0, sizeof(struct __b_dtcp_stream_data));
	}
	if (!max_packet_size)
	    StreamData->use_per_packet_pcp = true;
    else if (max_packet_size & 0xFFFFF) {
		BDBG_ERR(("max_packet_size must be multiple of 1MB\n"));
		BKNI_Free(StreamData);
		return NULL;
	} else
	    StreamData->use_per_packet_pcp = false;
    BDBG_MSG(("Per Packet PCP: %s\n", StreamData->use_per_packet_pcp? "Yes":"No"));
	/* Initalize stream data */
	StreamData->StreamType = B_Stream_eSource;
	StreamData->Transport = Transport;
	StreamData->cci = cci;
	B_DTCP_IP_GetEmiFromCCI(Session, cci, content_type, &StreamData->Emi);
	if (StreamData->Emi == B_ExtEmi_eInvalid) {
		BDBG_ERR(("Unable to get proper emi value based on CCI %d and content_type %d\n", 
			cci, content_type));
		BDBG_ERR(("Setting emi to copy-never\n"));
		StreamData->Emi = B_ExtEmi_eCopyNever;
	}
	if (Session->pcp_ur_cap == 1)
		BDBG_WRN(("Using PCP_UR mode!\n"));
	StreamData->content_length = content_length;
	StreamData->content_remain = content_length;
	StreamData->max_packet_size = max_packet_size;
	StreamData->packet_content_length = 0;
	StreamData->hPacketHandle = NULL;
	StreamData->AkeHandle = akeHandle;

	BLST_S_INSERT_HEAD(&(Session->Stream_list), StreamData, node);
	BDBG_LEAVE(B_DTCP_IP_OpenSourceStream);

	return StreamData;

}
/*! \brief close a stream.
 *  \param[in] hStreamHandle handle to the open stream.
 */
void B_DTCP_IP_CloseStream(B_StreamHandle_T hStreamHandle)
{
	B_AkeCoreSessionData_T * Session;

	struct __b_dtcp_stream_data * pStreamData = NULL;

	BDBG_ASSERT(hStreamHandle);
	BDBG_ENTER(B_DTCP_IP_CloseStream);
	pStreamData = (struct __b_dtcp_stream_data *)hStreamHandle;
	/* 
	 * TODO : In case other thread is sending/receiving 
	 * stream, might need to set content_length to 0 and
	 * sleep for a while?
	 */
	if(pStreamData->hPacketHandle != NULL)
		B_DTCP_Content_ClosePacket(pStreamData->hPacketHandle);
	Session = (B_AkeCoreSessionData_T *)(pStreamData->AkeHandle);
	if(Session != NULL)
	{
		BLST_S_REMOVE(&(Session->Stream_list), pStreamData, __b_dtcp_stream_data, node);
	}
	BKNI_Free(hStreamHandle);
    hStreamHandle = NULL;
	BDBG_LEAVE(B_DTCP_IP_CloseStream);
}
/*! \brief function to open a sink stream, after AKE.
 *  \param[in] akeHandle handle to AKE session.
 *  \param[in] Transport the stream's transport type.
 */
B_StreamHandle_T B_DTCP_IP_OpenSinkStream(B_AkeHandle_T akeHandle, B_StreamTransport_T Transport)
{
	struct __b_dtcp_stream_data * StreamData = NULL;
	B_AkeCoreSessionData_T * Session = (B_AkeCoreSessionData_T *)akeHandle;

	BDBG_ENTER(B_DTCP_IP_OpenSinkStream);
	
	if(akeHandle == NULL) {
		BDBG_ERR(("%s: Invalid AKE handle\n", __FUNCTION__));
		return NULL;
	}
	if((StreamData = (struct __b_dtcp_stream_data *)BKNI_Malloc(sizeof(struct __b_dtcp_stream_data)))
			== NULL)
	{
		BDBG_ERR(("Failed to allocate memory for stream : size=%d\n", sizeof(struct __b_dtcp_stream_data)));
		return NULL;
	}
	BKNI_Memset(StreamData, 0, sizeof(struct __b_dtcp_stream_data));
	/* Initalize stream data */
	StreamData->StreamType = B_Stream_eSink;
	StreamData->Transport = Transport;
	StreamData->hPacketHandle = NULL;
	StreamData->AkeHandle = akeHandle;
	if (Session->pcp_ur_cap == 1)
		BDBG_WRN(("Using PCP_UR mode!\n"));

	BLST_S_INSERT_HEAD(&(Session->Stream_list), StreamData, node);

	BDBG_LEAVE(B_DTCP_IP_OpenSinkStream);

	return StreamData;

}
/*! \brief packetize data, produce PCP stream, called by source device only.
 *  \param[in] hAkeHandle handle to AKE session.
 *  \param[in] clear_buf buffer hold the input data.
 *  \param[in] clear_buf_size size of the input buffer.
 *  \param[in,out] encrypted_buf buffer for output data.
 *  \param[in] encrypted_buf_size size of output buffer.
 *  \param[out] total total bytes processed.
 */
BERR_Code B_DTCP_IP_PacketizeData(B_AkeHandle_T hAkeHandle, B_StreamHandle_T hStreamHandle, 
		unsigned char * clear_buf, unsigned int clear_buf_size, unsigned char ** encrypted_buf, 
		unsigned int * encrypted_buf_size, unsigned int * total)
{
	B_AkeCoreSessionData_T * Session = (B_AkeCoreSessionData_T *)hAkeHandle;
	B_AkeCoreData_T * Core;
	BERR_Code retValue = BERR_SUCCESS;
    unsigned char *buf_ptr = NULL;
	struct __b_dtcp_stream_data * pStreamData = (struct __b_dtcp_stream_data *)hStreamHandle;

	unsigned int content_to_send = 0; 	/* clear content to send */
    unsigned int data_to_send = 0;		/* encrypted content to send */
	unsigned int content_padding = 0;	/* padding for clear content */
	/* unsigned int aligned_buf_size = 0; */

	BDBG_ASSERT(hStreamHandle);
	BDBG_ASSERT(clear_buf);
	BDBG_ASSERT(encrypted_buf);

	if(Session == NULL)
		return BERR_NOT_INITIALIZED;

	if(pStreamData->content_length != DTCP_CONTENT_LENGTH_UNLIMITED && pStreamData->content_remain == 0)
		return retValue;
	
    *total = 0;
    buf_ptr = *encrypted_buf;

	Core = Session->pAkeCoreData;
    if (pStreamData->use_per_packet_pcp)
        pStreamData->max_packet_size = clear_buf_size;
    while(pStreamData->content_length == DTCP_CONTENT_LENGTH_UNLIMITED ||
			(pStreamData->content_remain > 0 && clear_buf_size > *total) )
	{
		content_to_send = 0; 
		content_padding = 0;

		if(pStreamData->hPacketHandle == NULL)
		{
			int pcpl = 0;
			/* 
			 * Build packet header (PCPH), PCPH is needed for starting of 
			 * stream or previous PCP has been transmited out.
			 */
			if( pStreamData->content_length == DTCP_CONTENT_LENGTH_UNLIMITED)
				pcpl = pStreamData->max_packet_size;
			else {
				pcpl = (pStreamData->content_remain > pStreamData->max_packet_size)? 
					pStreamData->max_packet_size : pStreamData->content_remain;
			}
			pStreamData->packet_content_length = pcpl;

			if (!(pStreamData->content_processed & 0x7FFFFFF))
			{
			 	/* Retrieve RealTimeNonce value from AKE core.*/
				B_DTCP_GetSetRealTimeNonce(Core->hMutex, Core->RealTimeNonce, 
					pStreamData->HttpNonce);

				/* Increment nonce value, every 128MB of data transfer */
				B_DTCP_IncrementNonce(Core->hMutex, Core->RealTimeNonce);
			}
			BDBG_MSG(("New PCP Content Remain=%d \n", pStreamData->content_remain));
			retValue = B_DTCP_Content_CreatePacketHeader(pStreamData->AkeHandle, pStreamData->Transport,
					pStreamData, pcpl, buf_ptr);
			if(retValue != BERR_SUCCESS)
			{
				BDBG_ERR(("Failed to create PCP header for stream: %d \n", retValue));
				return retValue;
			}
			pStreamData->packet_content_remain = pStreamData->packet_content_length;
            buf_ptr += DTCP_CONTENT_PACKET_HEADER_SIZE;
            data_to_send += DTCP_CONTENT_PACKET_HEADER_SIZE;			
#ifdef B_HAS_PCP_16_BYTES
			
			/* If NETACCEL is enabled, this 14 bytes header cause great headache for 16 bytes AES block decryption!
			 * so we add 2 more bytes to make it 16 bytes aligned.
			 */
			buf_ptr += 2;
			data_to_send += 2;
#endif
		}
		content_to_send = (pStreamData->packet_content_remain > (int)clear_buf_size)? 
			(int)clear_buf_size : pStreamData->packet_content_remain ;
		/* 
		 * Make sure the output buffer is big enough to hold encrypted data.
		 */
		if(*encrypted_buf_size - data_to_send < 16)
		{
			BDBG_ERR(("NO enough room for content encryption: sending : bytes %d\n", data_to_send));
			retValue = B_ERROR_OUT_OF_MEMORY;
			break;
		}
		/* 
		 * Output buffer must be 16 bytes aligned (AES block size)
		 */
		/* aligned_buf_size = ((*encrypted_buf_size - data_to_send)/16)*16; */
        if (content_to_send >= 16)
            content_to_send = (content_to_send / 16 )*16; 
		/* content_to_send = (content_to_send > aligned_buf_size)? aligned_buf_size : content_to_send;  */
		/*
		 * Add padding.
		 */
		if(pStreamData->packet_content_remain - content_to_send == 0)
		{
			content_padding = (content_to_send % 16) == 0 ? 0: (16 - (content_to_send) % 16);
			if(content_to_send + content_padding + data_to_send > *encrypted_buf_size)
			{
				BDBG_ERR(("No enough room for padding: available=%d ask =%d\n",
							*encrypted_buf_size, content_to_send + content_padding + data_to_send));
				retValue = B_ERROR_OUT_OF_MEMORY;
				break;
			}
			BKNI_Memset(buf_ptr + content_to_send, 0, content_padding);
		}
        /* Encrypt data */
		BDBG_MSG(("clear_buf_size = %d content_to_send=%d content_padding=%d content_remain=%d packet_content_remain=%d\n", 
					clear_buf_size, content_to_send, content_padding, 
					pStreamData->content_remain, pStreamData->packet_content_remain));
		
        retValue = B_DTCP_Content_EncryptData(pStreamData->AkeHandle, pStreamData->hPacketHandle,
				clear_buf, 
				buf_ptr, 
				content_to_send + content_padding);
		if(retValue != BERR_SUCCESS)
		{
            BDBG_ERR(("Failed to encrypt data: %d\n", retValue));
            break;
        }

        buf_ptr += content_to_send + content_padding;
        data_to_send += content_to_send + content_padding;
        *total += content_to_send;
		pStreamData->content_processed += content_to_send;

		pStreamData->packet_content_remain -= content_to_send;
		if(pStreamData->content_length != DTCP_CONTENT_LENGTH_UNLIMITED)
			pStreamData->content_remain -= content_to_send;

        clear_buf += content_to_send;
        clear_buf_size -= content_to_send;

		/* 
		 * if current PCP is done, close packet and free packet handle.
		 */
		if( pStreamData->packet_content_remain == 0)
		{
			B_DTCP_Content_ClosePacket(pStreamData->hPacketHandle);
			pStreamData->hPacketHandle = NULL;
		}

		if (0 == clear_buf_size)
		{
			break;
		}
		
	} /* while loop */
	
	*encrypted_buf_size = data_to_send;
	BDBG_MSG(("Total=%d\n", *total));

	if(pStreamData->hPacketHandle == NULL)
	{
		/* Reschedule the exchange key timer */
		BDBG_MSG(("Rescheduling exchange key timer!\n"));
		retValue = B_DTCP_StartExchKeyTimer(Session, Session->pAkeCoreData, B_DeviceMode_eSource);
	}
	return retValue;
}
/*! \brief Depacketize DTCP PCP data received from source device.
 *  param[in] Akehandle AkeSession handle obtained from AKE procedure.
 *  param[in] encrypted_buf buffer holding the encrypted data.
 *  param[in] encrypted_buf_size size of the encrypted buffer.
 *  param[in] clear_buf buffer to hold the decrypted data.
 *  param[out] clear_buf_size size of decrypted bytes.
 *  param[out] total total length processed.
 *  param[out] pcp_header_found flag indicate this block of data contains PCP header.
 */
BERR_Code B_DTCP_IP_DepacketizeData(B_AkeHandle_T hAkeHandle, B_StreamHandle_T hStreamHandle, 
		unsigned char * encrypted_buf, unsigned int encrypted_buf_size, unsigned char * clear_buf, unsigned int * clear_buf_size, 
		unsigned int * total, bool * pcp_header_found)
{
	B_AkeCoreSessionData_T * Session = (B_AkeCoreSessionData_T *)hAkeHandle;
	B_IpAkeSessionData_T *IpSession = (B_IpAkeSessionData_T *)Session->pProtocolData;
	BERR_Code retValue = BERR_SUCCESS;
	struct __b_dtcp_stream_data * StreamData = (struct __b_dtcp_stream_data *)hStreamHandle;
#ifdef DUMP_STREAM
	char fname[16];
#endif
    unsigned char *buf_ptr = NULL;
    unsigned int data_to_process = 0;
    unsigned int content_processed = 0;
    unsigned int data_processed = 0;
	
	BDBG_ASSERT(hAkeHandle);
	BDBG_ASSERT(StreamData);
	BDBG_ASSERT(encrypted_buf);
	BDBG_ASSERT(clear_buf);
	BDBG_ASSERT(clear_buf_size);

	 *total = 0;
    
    buf_ptr = clear_buf;
    
	if(StreamData->hPacketHandle != NULL)
		*pcp_header_found = false;
    do {
		/*
		 * If packet handle is NULL, it means PCP header hasn't been consumed yet.
		 */
		if(StreamData->hPacketHandle == NULL)
		{
			BDBG_MSG(("\r\n\t #### PCP # %d ###\r\n", pcpNum));
#ifdef DUMP_STREAM
			sprintf(fname, "pcp_%d.bin", pcpNum);
			fout = fopen(fname, "wb");
			fwrite(encrypted_buf, 1, 14, fout);
			fflush(fout);
#endif
			retValue = B_DTCP_Content_ConsumePacketHeader(hAkeHandle, encrypted_buf, StreamData);
			if(retValue != BERR_SUCCESS)
			{
				BDBG_ERR(("Failed to decode PCP header\n"));
				goto ERR_OUT;
			}
			*pcp_header_found = true;
			pcpNum++;
			/*
			 * Figuring out the padding bytes.
			 * V1SE 4.22 specifies that PCP header's Length doesn't include any padding.
			 */
			StreamData->packet_padding_bytes = (StreamData->packet_content_length % 16 != 0)?
				(16 - StreamData->packet_content_length % 16) : 0;
			StreamData->packet_bytes_remain = StreamData->packet_content_length + StreamData->packet_padding_bytes;
            
			encrypted_buf += DTCP_CONTENT_PACKET_HEADER_SIZE;
            *total += DTCP_CONTENT_PACKET_HEADER_SIZE;
            data_processed += DTCP_CONTENT_PACKET_HEADER_SIZE;

#ifdef B_HAS_PCP_16_BYTES
			/*
			 * if Broadcom's NETACCEL is enabled, multiple packets are read once, they must be 16 bytes aligned.
			 * The PacketizeData() added 2 bytes after PCP header for this situation.
			 */
			encrypted_buf += 2;
			*total += 2;
			data_processed += 2;
			/* end of NetAccel fixes */
#endif
			if (Session->pAkeCoreData->ckc_check == true)
			{
				/* 
			 	* If both soruce/sink AL flag is set,
			 	* check if content key confirmation failed. quit decryption if it did.
			 	*/
				if (IpSession->ContKeyConfCount < 5)
					IpSession->StartContKeyConf = true;
				BDBG_ERR(("ContKeyConfCount=%d ContKeyConfirmed=%d\n", IpSession->ContKeyConfCount, IpSession->ContKeyConfirmed));
			}
        } 

        data_to_process = MIN(StreamData->packet_bytes_remain, (int)(encrypted_buf_size - data_processed));

		/* Make sure we don't process data more then output buffer can hold */
        data_to_process = MIN(data_to_process, *clear_buf_size - content_processed);
		
		data_to_process = (data_to_process % 16 != 0) ? 
            (data_to_process - (data_to_process % 16)) : (data_to_process);     

		if (data_to_process == 0)
		{
			retValue = BERR_SUCCESS;
            break;
        }

		retValue = B_DTCP_Content_DecryptData(StreamData->AkeHandle, StreamData->hPacketHandle,
				encrypted_buf, buf_ptr, data_to_process);
		
		if(retValue != BERR_SUCCESS)
		{
			BDBG_ERR(("Failed to decrypt content stream: %d\n", retValue));
			goto ERR_OUT;
		}
#ifdef DUMP_STREAM
		fwrite(encrypted_buf, 1, data_to_process, fout);
		fflush(fout);
#endif
#if 0
		BDBG_MSG(("\r\n\t encrypted_buf_size=%d data_to_process=%d packet_bytes_remain=%d padding_bytes=%d content_length=%d\n",
					encrypted_buf_size, data_to_process, StreamData->packet_bytes_remain, 
					StreamData->packet_padding_bytes, StreamData->content_length));
#endif
        /* book keeping */
        encrypted_buf += data_to_process;
		buf_ptr += data_to_process;			/* advancing clear buffer pointer */
        data_processed += data_to_process;
        StreamData->packet_bytes_remain -= data_to_process;        
        *total += data_to_process;
        StreamData->content_length += data_to_process;
        
        if (StreamData->packet_bytes_remain == 0)
        {
            content_processed += data_to_process - StreamData->packet_padding_bytes;
			/* 
			 * This PCP is done, close packet, if new data come in , expecting a new PCP 
			 * header in it, consume header function will allocate a new handle for it.
			 */
            B_DTCP_Content_ClosePacket(StreamData->hPacketHandle);
            StreamData->hPacketHandle = NULL;
        } else {
            content_processed += data_to_process;
        }
        
        data_to_process = encrypted_buf_size - data_processed;

     } while (0 != data_to_process);

    *clear_buf_size = content_processed;
	if(StreamData->hPacketHandle == NULL)
	{
		/* Reschedule the exchange key timer */
		retValue = B_DTCP_StartExchKeyTimer(Session, Session->pAkeCoreData, B_DeviceMode_eSink);
	}

ERR_OUT:
#ifdef DUMP_STREAM
	fclose(fout);
#endif
	return retValue;
}
/*! \brief force to close packet handle, called by source device's exchange key timer expire function.
 *  \param[in] hStreamHandle sream handle.
 *  \retval none.
 */
void B_DTCP_Stream_ClosePacketHandle(struct __b_dtcp_stream_data  *StreamData)
{
	if(StreamData->hPacketHandle != NULL) 
	{
		B_DTCP_Content_ClosePacket(StreamData->hPacketHandle);
		StreamData->hPacketHandle = NULL;
	}
}
