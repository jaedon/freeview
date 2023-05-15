/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_playback_ip.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/11/08 7:28p $
 *
 * Module Description:
 * File is defines the Settop API Shim layer for Playback IP Interface. 
 * (BSEAV/api/src/pvr/bsettop_playback_ip.h) 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_playback_ip.h $
 * 
 * 1   6/11/08 7:28p ssood
 * PR42739: Settop API Shim layer implementation of Playback IP Interface
 * 
 *
 ***************************************************************************/
#ifndef BSETTOP_PLAYBACK_IP_H__
#define BSETTOP_PLAYBACK_IP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************
 * This module defines the legacy IP STB IP Playback Interface. It internally
 * uses the IP Applib from Nexus to provide IPSTB functionality.
 *
 * It defines Shim interface to:
 *     - open/close of IP sockets 
 *     - playpump start/stop
 *
 ***************************************************************************/
/* IP Playback API */

/*
Summary:
	Open an IP playback channel.
*/
bplayback_t bplayback_ip_p_open(void);

/*
Summary:
	Close a playback channel.
*/
void bplayback_ip_p_close(
	bplayback_t playback /* Handle returned by bplayback_ip_open */
	);

/*
Summary:
	Open a IP playback socket
Description:
	Open a IP playback socket to pass to bplayback_ip_start().

	The bplayback_ip_socket_t can be used for only one IP playback at a time.
*/
bplayback_file_t bplayback_ip_socket_open(
	bplayback_t playback, /* Handle returned by bplayback_ip_open */
	bplayback_socket_params *params
	);

/*
Summary:
	Notify playback ip thread about any play transitions 
Description:
	Application uses this function to notify the playback IP thread
	about any play transitions: play -> trick -> trick -> play
*/
long bplayback_ip_goto_index (
	bplayback_t playback, /* Handle returned by bplayback_ip_open */
	long offset, /* Unused */ 
	int whence   /* Unused */
	); 
	

/*
Summary:
	Close a IP playback socket.
Description:
	Close a IP playback socket opened by bplayback_ip_socket_open().
*/
void bplayback_ip_socket_close(
	bplayback_t playback /* Handle returned by bplayback_ip_open */
	);

/*
Summary:
	Required to initialize the bplayback_ip_socket_params structure.
*/
void
bplayback_ip_socket_params_init(
	bplayback_socket_params *params,	/* [out] */
	bplayback_t playback		/* required for possible resource-dependent defaults */
	);

/*
Summary:
  Configures IP playback to be started.

Description:
	If bplayback_ip_start succeeds, it returns a digital stream handle which can
	be decoded or recorded.
*/
bstream_t bplayback_ip_start(
	bplayback_t 				playback, 
	bplaypump_t 				playpump, 
	const bstream_mpeg 			*mpeg, 
	bplayback_file_t 			socket, 
	const bplayback_params		*params
	);

/*
Summary:
	Stop playback.
Description:
	If you are decoding the stream, the decode will be stopped as well.
*/
bresult bplayback_ip_stop(bplayback_t playback);

/*
Summary:
	Pause playback.
*/
bresult bplayback_ip_pause(bplayback_t playback);

/*
Summary:
	Play/Resume playback.
*/
bresult bplayback_ip_play(bplayback_t playback);

/*
Summary:
	Trickmode playback.
*/
bresult bplayback_ip_trickmode(bplayback_t playback, const bplayback_trickmode_params *trickmode);


#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_PLAYBACK_IP_H__ */

