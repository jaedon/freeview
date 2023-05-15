/***************************************************************************
 *     Copyright (c) 2006-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tcpserver.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/10/07 12:18a $
 *
 * Module Description:
 * Video Streaming Driver for BCM7401
 * This is a application stub for launching all recorders and streamers
 *  
 * Revision History:
 *    
 * $brcm_Log: /BSEAV/lib/netaccel/app/tcpserver.c $
 * 
 * 1   8/10/07 12:18a ssood
 * PR33786: changes to accomodate Net DMA & IP Streamer refactoring into
 * bcmnetaccel driver
 * 
 * 1   10/11/06 3:36p jrubio
 * PR24672:Initial checkin for IPStreamer lib. Yasantha's ip streamer code
 * with some mods.
 * 
 * 
 *************************************************************/ 

void launch_all_streamers(int max_streamers, int chmap_servers);

int main(int argc, char **argv){
	launch_all_streamers(5,5);
}


