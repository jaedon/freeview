/***************************************************************************
*     (c)2003-2012 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
* 
* $brcm_Workfile: b_playback_ip_http.c $
* $brcm_Revision: 223 $
* $brcm_Date: 11/9/12 3:22p $
*
* Description: HTTP protocol implementation of IP Applib 
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_http.c $
* 
* 223   11/9/12 3:22p ssood
* SW7425-4169: allow apps to pass in configurable nexus heap handle
* 
* 222   11/1/12 4:38p ssood
* SW7231-858: coverity fixes
* 
* 221   11/1/12 8:24a ssood
* SW7231-858: free up cookie header if HttpSessionOpen fails
* 
* 220   10/30/12 11:09a ssood
* SW7231-858: free up cookie header
* 
* 219   10/18/12 11:40a ssood
* SW7425-4093: add logic to dynamically compute the HTTP request buffer
*  size so that long URLs (for playing from youtube) can be properly
*  handled
* 
* 218   10/16/12 5:43p ssood
* SW7231-858: non-blocking Ip_HttpSessionOpen support (by making non-
*  blocking connect) + don't treat timeouts fatal during media probe when
*  index is at the end & instead retry read + note current play position
*  when we are playing the last frame thru the playpump only path
* 
* 217   9/15/12 3:10p ssood
* SW7231-1010: fixed runtime buffer depth calculation due to compiler
*  change + dont pre-fill cache from previous point after a seek request
* 
* 216   8/30/12 5:06p ssood
* SW7425-3541: coverity fix: lower stack usage
* 
* 215   8/28/12 2:35p ssood
* SW7231-974: extend playback_ip current position logic to handle PTS
*  discontinuities during normal playback
* 
* 214   8/13/12 2:37p ssood
* SW7346-911: fixed to handle cases where chunk size is < 1400 bytes and
*  server times out right after sending this smaller chunk + index
*  caching logic change where we continue to satisfy cache missed via
*  index cache until media probe is complete (needed for ASF playbacks)
* 
* 213   8/2/12 12:23p ssood
* SW7346-347: enhanced http cache logic to support MKV playback
* 
* 212   7/31/12 4:18p ssood
* SW7346-347: code to cache & re-use HTTP cookie + fixes for Http Xfer
*  encoding + debug related additions
* 
* 211   7/19/12 4:07p gskerl
* SW7346-853: Merged from branch SW7346-853 to main.
* 
* SW7346-853/2   7/10/12 3:20p gskerl
* SW7346-853: Updated branch SW7346-853 from main/LATEST.
* 
* SW7346-853/1   7/9/12 12:34p gskerl
* SW7346-853: Added support for MPEG-DASH. Changed BDBG_MSG to
*  BDBG_MODULE_MSG for socket and cache-related messages (so they don't
*  flood the BDBG_MSG output. Added some temporary code to print sent and
*  received HTTP traffic.
* 
* 210   6/25/12 6:11p ssood
* SW7435-250: coverity fixes exposed by new coverity version
* 
* 209   6/20/12 4:36p ssood
* SW7231-858: suppressing more debug messages under ipVersboseLog
* 
* 208   6/15/12 6:26p ssood
* SW7231-858: more hls related improvements
* 
* 207   6/13/12 1:54p ssood
* SW7231-858: supressing debug logs for HLS playback sessions
* 
* 206   6/8/12 7:11p ssood
* SW7425-3201: Add support for new DLNA PlaySpeed trickmode options in
*  playback ip library
* 
* 205   6/5/12 7:16p ssood
* SW7231-766: enable runtime recording of http av data
* 
* 204   6/5/12 6:46p ssood
* SW7231-766: add support for downloading encrypted HLS keys + support
*  for HLS spec version 3
* 
* 203   5/23/12 3:13p ssood
* SW7425-2894: suppressing extra debug log from ip client modules
* 
* 202   5/18/12 4:56p ssood
* SW7231-830: add support for HTTP proxy
* 
* 201   4/18/12 6:16p celysee
* SW7425-2894: Support for DTCP-IP with robustness rules
* 
* 200   4/11/12 3:06p ssood
* SW7231-766: add support for HTTP redirects for HLS sessions
* 
* 199   4/3/12 12:31p ssood
* SW7425-2783: added support to handle HTTP redirects during HLS sessions
* 
* 198   2/22/12 4:34p ssood
* SW7231-603: further changes to improve the media probing for cases
*  where server sends incorrect content-type HTTP header response
* 
* 197   1/24/12 11:31a ssood
* SW7425-1331: insert encrypted (DTCP/IP) special data while streaming
*  out media files
* 
* 196   1/18/12 3:16p ssood
* SW7231-603: diable use of netaccel for HTTP protocol (due to lack of
*  any performance gains)
* 
* 195   1/17/12 5:40p ssood
* SW7231-603: limit media probe time
* 
* 194   1/12/12 5:22p ssood
* SW7344-251: coverity fix: initializsed http status to default value
* 
* 193   1/11/12 2:50p ssood
* SW7425-2061: wrap simple decoders call for NEXUS_HAS_SIMPLE_DECODER
* 
* 192   1/1/12 8:20a ssood
* SW7425-2061: reconnect to server if connection is already closed during
*  session re-start
* 
* 191   12/27/11 10:07p ssood
* SW7425-2061: add simple decoder support to ip playback
* 
* 190   12/6/11 2:29p ismailk
* SW7425-1900: Don't set the firstPts from the server
* 
* 189   11/23/11 2:10p ssood
* SW7408-109: RVU Trickplay support: added FFWD & FRWD
* 
* 188   10/21/11 1:54p ssood
* SW7231-405: pause support for HLS protocol
* 
* 187   8/30/11 5:04p ssood
* SW7425-1077: dont call eof eventCallback if app has already issued the
*  stop
* 
* 186   8/22/11 12:23p ssood
* SW7425-1040: break out of probing if app changnes channel
* 
* 185   8/7/11 6:37p ssood
* SW7425-1040: more changes to support FCC over IP sessions
* 
* 184   7/11/11 12:12p ssood
* SW7231-268: add flash type handle HTTP redirect even after HTTP session
*  is established (during seeks)
* 
* 183   6/21/11 12:55p ssood
* SWDTV-6859: dont try to fill a complete cacheline if there a timeout
*  during socket read
* 
* 182   6/6/11 7:34p ssood
* SW7231-166: fix avgBitRate calculations for fragmented mp4 streams +
*  allow dataReadCallback for streams w/o index data
* 
* 181   6/6/11 1:28a ssood
* SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
*  eventCallback support for end of stream playback
* 
* 180   5/31/11 10:12a ssood
* SW7231-166: require apps to set contentLengthHint for allowing
*  trickmodes on streaming content from DLNA Servers using HTTP Chunk
*  Xfer Encoding
* 
* 179   5/26/11 11:31p ssood
* SW7231-166: dont create Nexus File Handle for cases where Nexus
*  playpump will get used
* 
* 178   5/26/11 10:26a randyjew
* SW7231-166: Delete unused sockRecvParams variable
* 
* 177   5/25/11 3:35p ssood
* SW7231-166: fixed some bugs during failing trickplay cases
* 
* 176   5/24/11 2:38p ssood
* SW7231-166: more changes to support DLNA trickmodes
* 
* 175   5/24/11 11:36a ssood
* SW7405-4392: Coverity fixes
* 
* 174   5/23/11 3:44p ssood
* SW7231-166: Extend Playback IP trickmodes to be compatible with latest
*  DLNA Guidelines
* 
* 173   5/19/11 4:27p ssood
* SW7420-1774: Add support to decode SVC/MVC videos
* 
* 172   4/22/11 2:28p ssood
* SW7425-365: new tool chain required changing gethostbyname to
*  getaddrinfo
* 
* 171   3/28/11 11:01a ssood
* SW7420-502: cache a private copy of user supplied http headers for
*  later use
* 
* 170   3/8/11 3:59p ssood
* SWDTV-5698: data read offset is not updated when data is present in the
*  index cache
* 
* 169   2/28/11 12:55p ssood
* SW7420-502: reset halt event before starting the live/playback sessions
* 
* 168   2/23/11 11:48a ssood
* SW7420-502: clock recovery support for RVU client
* 
* 167   12/29/10 10:19p ssood
* SW7420-1257: dont sent time seek range request unless starting time is
*  specified
* 
* 166   12/23/10 7:41p ssood
* SW7420-1257: delay returning back to io thread when playback lock is
*  held by another thread
* 
* 165   12/14/10 4:09p ssood
* SW7420-1217: dont use Nexus heap for HTTP cache due Nexus heap size
*  limitations on current chips
* 
* 164   12/14/10 12:07p ssood
* SW7420-1217: add support for encryption key parsing, downloading and
*  receiving encrypted content
* 
* 163   12/8/10 3:57p ssood
* SW7420-1217: refined support for playing unbounded streams
* 
* 162   11/11/10 10:34a ssood
* SW7420-1217: added support for playing unbounded (live) streams
* 
* 161   10/29/10 2:46p ssood
* SW7420-1217: conditionally compile HLS related changes to the existing
*  files
* 
* 160   10/29/10 9:05a ssood
* SW7420-1217: basic support to play both variant & non-variant bounded
*  playlists containing clear content
* 
* 159   10/27/10 8:27p ssood
* SW7420-1217: initial check-in for HLS support
* 
* 158   10/27/10 6:10p ssood
* SW7420-1213: Fixed an error during debug print
* 
* 157   10/6/10 6:27p ssood
* SW7420-1014: sometimes video will restart when changing FF rate + pause
*  macroblocking fix
* 
* 156   9/30/10 10:59a ssood
* SWGERRARD-547: only pause & resume before & after seek if currently in
*  the playing state
* 
* 155   9/24/10 5:42p ssood
* SWGERRARD-547: added support for seeks on ES files (MP3, WMA w/o index)
* 
* 154   9/17/10 8:21a ssood
* SWGERRARD-547: support to expose multiple multiple programs and tracks
*  within a multi-program stream
* 
* 153   9/2/10 2:58p ssood
* SW7420-876: Added support to play html5 video links which store URLs in
*  the QuickTime Redirect format
* 
* 152   8/30/10 5:13p ssood
* SW7420-502: fixed compile errors for the case when netaccel is turned
*  on
* 
* 151   8/27/10 3:59p ssood
* SW7420-502: added partial support to play RVU sessions in live mode
* 
* 150   8/16/10 2:39p ssood
* SW7420-883: return error when offset is beyond content length for seek
*  callbacks
* 
* 149   8/12/10 7:04p ssood
* SW7420-883: return EOF even for error cases, otherwise app is not
*  getting the error callback
* 
* 148   8/12/10 11:31a ssood
* SW7420-502: disable seeks on TS playback when server side trickmodes
*  are supported
* 
* 147   7/30/10 5:22p ssood
* SW7420-502: fixed the quick trickplay transitions
* 
* 146   7/23/10 11:10a ssood
* SW7420-502: RVU client: fixed subtle issues w/ playing DTCP encrypted
*  HTTP Chunk encoded content from HMC30 server
* 
* 145   6/16/10 5:05p ssood
* SW7420-716: initial RVU trickmode related check-ins
* 
* 144   4/21/10 11:13a ssood
* SW7420-561: cleaned up the request & response buffer management logic
* 
* 143   4/14/10 5:02p ssood
* SW7420-561: fixed debug prints
* 
* 142   4/2/10 4:18p ssood
* SW7420-561: add support to handle large URLs (HTML5 URLs can be over
*  256bytes)
* 
* 141   3/8/10 7:08p ssood
* SW7420-561: now really fixed the memory leak
* 
* 140   3/8/10 6:57p ssood
* SW7420-561: fixed a memory leak
* 
* 139   3/8/10 9:04a ssood
* SW7420-561: only detect rewind condition for normal playback and not
*  during trickplays
* 
* 138   3/4/10 2:48p ssood
* SW7420-561: fixing compile error
* 
* 137   3/4/10 12:17p ssood
* SW7420-561: merge from branch to mainline
* 
* SW7420-561/4   3/1/10 7:55a ssood
* SW7420-561: added support to dynamically adjust for DTCP PCP header +
*  plus fixed a bug causing periodica macroblocking when HTTP Chunk Xfer
*  Encoding was used w/ DTCP/IP Decryption
* 
* SW7420-561/3   2/18/10 12:43a ssood
* SW7420-561: added checks to validate IP state during IP API calls
* 
* SW7420-561/2   2/16/10 10:42p ssood
* SW7420-561: Changes to support basic RTSP w/o trickmodes
* 
* SW7420-561/1   2/16/10 10:10a ssood
* SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
*  Philips RTSP requirements
* 
* 136   1/28/10 2:19p ssood
* SW7420-454: client should re-connect to server if app wants to loop
*  around the content
* 
* 135   1/23/10 10:17a ssood
* SW7420-454: further changes to condense the ip playback log
* 
* 134   12/30/09 9:15p dliu
* SW7405-3400: Add RVU specific HTTP header
* 
* 134   12/30/09 9:14p dliu
* SW7405-3400: Add RVU HTTP header
* 
* 133   12/29/09 10:52p ssood
* SW7420-454: clear serverside trick mode flag upon channel change
* 
* 132   12/23/09 4:24p erickson
* SW7420-516: convert non-error BDBG_ERR to BDBG_WRN
* 
* 131   12/17/09 11:19p ssood
* SW7420-454: skip media probe on client if server already sends this
*  info & supports server side trick modes
* 
* 130   12/14/09 3:21p ssood
* SW7420-454: fix coverity reported issues
* 
* 130   12/14/09 3:20p ssood
* SW7420-454: fix coverity reported issues
* 
* 129   12/12/09 6:34p ssood
* SW7420-502: only process current chunk header end & next chunk header
*  begining when >2 bytes have been read
* 
* 128   12/11/09 12:41p ssood
* SW7420-502: fixed non-dtcp compilation
* 
* 127   12/10/09 6:48p ssood
* SW7420-502: support receiving dtcp/ip encrypted content via the HTTP
*  chunk encoding
* 
* 126   11/18/09 12:43p ssood
* SW7420-454: added HTTP parameter on client so that it can indicate
*  DTCP/IP encryption to ip_streamer
* 
* 125   11/17/09 4:04p ssood
* SW7420-340: added support to clone a dtcp/ip session (needed to
*  multiple connects from a client)
* 
* 124   10/28/09 9:47a ssood
* SW7405-2681: added work arounds to support buggy servers (found during
*  DLNA certification)
* 
* 123   10/16/09 1:49p ssood
* SW7405-1689: if rangeEnd is given, only read the needed bytes
* 
* 122   9/23/09 1:31p ssood
* SW7405-2559: fixed a bug in LPCM support
* 
* 121   9/21/09 12:09p ssood
* SW7405-2559: Adding LPCM playback support
* 
* 120   9/18/09 7:15p ssood
* SW7405-2681: disabling use of Range Header when app requests it
* 
* 119   9/16/09 4:49p ssood
* SW7420-340: workaround for getting audio to work on live channels
*  coming over HTTP, delay the playback by 2 sec
* 
* 118   9/1/09 7:34p ssood
* SW7420-340: initial check-in to support streaming of Live IP data
* 
* 117   8/28/09 11:59a ssood
* SW7405-1689: only update media duration if it is successfully
*  determined during media probe
* 
* 116   8/27/09 2:00p ssood
* SW7405-1689: only do detailed payload probing is requested by app
*  (otherwise probe is taking over 5 secs)
* 
* 115   8/26/09 11:02a ssood
* SW7405-1689: fixed a hang condition during error path
* 
* 114   8/25/09 1:12p ssood
* SW7405-1689: got chunk encoding to work w/ detailed format inspection
*  changes
* 
* 113   8/24/09 3:23p ssood
* SW7405-1689: adapting server based trickmodes to Nexus playback changes
*  due to OTF support
* 
* 112   8/21/09 3:06p ssood
* PR50202: disabling probe duration for getting dtcp-ip to work
*  (temporary hack)
* 
* 111   8/21/09 12:39p ssood
* PR55325: added support for playing files where content type doesn't
*  provide a valid hint
* 
* 110   8/14/09 5:00p ssood
* PR50202: First check-in for clientside trickmode support for MPEG2 TS
*  format
* 
* 109   8/6/09 3:38p ssood
* PR48146: add HTTP redirect support
* 
* 108   8/6/09 10:40a ssood
* PR43746: fixing warnings
* 
* 107   8/6/09 10:18a ssood
* PR43746: added support for calculating more precise bitrates for TS &
*  VOB files (support index reads from middle & end of content)
* 
* 106   7/30/09 10:30a ssood
* PR43746: provide current playback position via the IP GetStatus
* 
* 105   7/29/09 6:14p ssood
* PR43746: extend IP trick modes to support Disconnect And Seek based
*  paused operations (DLNA requirement)
* 
* 104   7/27/09 3:18p ssood
* PR43746: added support for allowing apps to receive directly read data
*  w/o nexus playback (image playback)
* 
* 103   7/24/09 12:01p ssood
* PR56519: Allow apps to provide additional HTTP headers fields for
*  outgoing GET requests
* 
* 102   7/21/09 4:18p ssood
* PR43746: Further work for Server based HTTP trick mode support
* 
* 101   7/16/09 12:59p ssood
* PR43746: Add Server based HTTP trick mode support
* 
* 100   7/6/09 4:02p ssood
* PR56519: Add a flag to allow apps to specify disabling of the Range
*  Header
* 
* 100   7/6/09 4:02p ssood
* PR56519: Add a flag to allow apps to specify disabling of the Range
*  Header
* 
* 100   7/6/09 3:55p ssood
* PR56519: Add a flag to allow apps to specify disabling of the Range
*  Header
* 
* 99   6/26/09 9:31a lwhite
* PR53773: Initialize netio's suspend and shutdown ptrs
* 
* 98   6/25/09 2:30a ssood
* PR55417: added option to allocate HTTP precharge buffer either at init
*  or run time
* 
* 97   6/23/09 11:54p ssood
* PR56281: Coverity fixes
* 
* 96   6/22/09 6:47p ssood
* PR55417: taking out a debug log
* 
* 95   6/22/09 6:31p ssood
* PR55417: set HTTP index cache size also based on the user provided size
*  (otherwise, default size is too small) + fix for n/w timeout during
*  index cache reading
* 
* 94   6/22/09 1:44p ssood
* PR55417: Precharging support for TS/VOB formats: moved the HTTP
*  data/index cache allocations to IP Applib Open time, lowered default
*  cache sizes
* 
* 93   6/21/09 8:56p ssood
* PR55417: enabling caching of TS+VOB formats & switching to 1 data cache
*  model to lower memory usage to enable caching of HD TS
* 
* 92   6/18/09 10:24a ssood
* PR55417: provide http return code code via the GetStatus to app
* 
* 91   6/18/09 8:23a ssood
* PR55417: minof fix for ASF wrap around case using chunk xfer encoding
* 
* 90   6/17/09 7:29p ssood
* PR55417: complete code for wrap around support for HTTP mode using
*  chunk xfer encoding
* 
* 89   6/17/09 2:48p ssood
* PR55417: partial code for wrap around support for HTTP mode using chunk
*  xfer encoding
* 
* 88   6/16/09 3:00p ssood
* PR55417: accounting for fudge factor in max buffer depth calculations
* 
* 87   6/16/09 9:00a ssood
* PR55417: fixing another rewind related bug
* 
* 86   6/15/09 11:33p ssood
* PR55417: fixing rewind related bug
* 
* 85   6/12/09 9:51a lwhite
* PR53773: Fixed RadEa compilation error
* 
* 84   6/10/09 10:25p lwhite
* PR53773: Add RadEa time/byte offset
* 
* 83   6/8/09 3:55p lwhite
* PR53773: Handle EINTR and EAGAIN in http_socket_read
* 
* 82   6/7/09 11:45p ssood
* PR55417: Adding support for runtime buffering of HTTP content (used
*  during seeks & n/w loss events)
* 
* 81   6/5/09 1:41p ssood
* PR55626: DLNA guidelines mandate client to only maintain no more than 1
*  connection to server, we had one connx per data cache, changed that to
*  closing/opening 1 connx
* 
* 81   6/5/09 1:35p ssood
* PR55626: DLNA guidelines mandate client to only maintain no more than 1
*  connection to server, we had one connx per data cache, changed that to
*  closing/opening 1 connx
* 
* 80   6/3/09 10:55a lwhite
* PR54142: Coverity fixes
* 
* 79   5/29/09 6:19p ssood
* PR55325: run time buffer management: support reading less than a data
*  chunk (can happen due to temporary n/w loss)
* 
* 78   5/28/09 4:54p ssood
* PR55325: allow app to specify n/w timeout & buffer cache size
* 
* 77   5/28/09 11:38a ssood
* PR55325: added additional states to IP Applib to provide flexibility in
*  the order of API calls
* 
* 76   5/26/09 11:49a ssood
* PR53773: added debug code to track a crash during quit
* 
* 75   5/25/09 11:31p ssood
* PR55325: Add support for pre-charging network buffer for HTTP playback
* 
* 74   5/18/09 12:52p ssood
* PR53773: in some cases, file was not being played till the end
* 
* 73   5/15/09 1:08p ssood
* PR53773: adding support for skipping Media probe: needed for RAD/EA
*  playback where app apriori have media info
* 
* 72   5/14/09 10:14p ssood
* PR53773: adding support for MP3 files which have content information
*  towards the end of file
* 
* 71   5/14/09 4:26p ssood
* PR53773: enable the session cloning code for RAD/ES
* 
* 70   5/14/09 4:21p ssood
* PR53773: content length was getting set during seek + gracefully handle
*  File not found error during seek
* 
* 69   5/14/09 11:49a ssood
* PR53773: content length was not getting set correctly a/f the HTTP
*  helper function refactoring
* 
* 68   5/13/09 10:32p ssood
* PR48146: make content type search more generic
* 
* 67   5/13/09 11:22a ssood
* PR53773: securityHandle for RAD/EA was used incorrectly
* 
* 66   5/13/09 10:39a ssood
* PR53773: created utitlity functions for HTTP parsing so that RAD/EA
*  module can use it as well
* 
* 65   5/12/09 5:36a ssood
* PR54955: content length > 2GB was not being handled correctly
* 
* 64   5/11/09 5:25p ssood
* PR54955: Record TTS streams over HTTP: parse HTTP header to determine
*  if server is sending a TTS stream
* 
* 63   5/1/09 4:39p ssood
* PR53773: adding support for WMA playback over HTTP
* 
* 62   4/24/09 7:00p ssood
* PR53773: added audio playback support over HTTP for MP3 files (ASF WMA
*  w/o index needs more work)
* 
* 61   4/21/09 3:32p ssood
* PR53773: handle cases where some security protocols dont encrypt HTTP
*  response header
* 
* 60   4/21/09 10:24a lwhite
* PR53373: Close security in cache miss processing
* 
* 59   4/20/09 3:13p ssood
* PR50311: Added support for playing longer duration (> 1hr) MP4 files:
*  need to keep two data caches (one each for Audio & Video)
* 
* 59   4/20/09 3:02p ssood
* PR50311: Added support for playing longer duration (> 1hr) MP4 files:
*  need to keep two data caches (one each for Audio & Video)
* 
* 58   4/16/09 5:18p lwhite
* PR53373: Clone session support
* 
* 57   4/13/09 10:43a ssood
* PR50311: seek optimization: dont reconnect unless new offset is outside
*  the complete cache
* 
* 56   4/10/09 5:25p lwhite
* PR53773: Suspend support added
* 
* 55   4/10/09 4:56p ssood
* PR50311: adding seek support for ASF content: need to cache both Index
*  & Data
* 
* 54   4/9/09 1:53p ssood
* PR50202: code changes to support seek operation
* 
* 53   4/7/09 10:01a ssood
* PR53773 : fixing compilation errors
* 
* 52   4/6/09 6:09p ssood
* PR53773 : refactored basic TCP socket functions for reuse by other
*  internal modules
* 
* 51   4/6/09 9:17a ssood
* PR53773 : expose media duration to the applib caller
* 
* 50   4/3/09 3:09p lwhite
* PR53773: Added SSL support
* 
* 49   4/2/09 11:36p ssood
* PR53773 : changes to support security framework for SSL, DTCP-IP,
*  RAD/ES protocols
* 
* 48   4/2/09 11:30a ssood
* PR53773 : refactored low level read & select related code inorder to
*  support security framework
* 
* 47   3/27/09 1:52p ssood
* PR48146: add support for MP4 that have index data (MDAT object) at the
*  end
* 
* 46   3/19/09 2:16p ssood
* PR48146: add rewind support for MP4
* 
* 45   3/18/09 2:39p ssood
* PR50311: minor code cleanup
* 
* 44   3/18/09 10:02a lwhite
* PR53329: Coverity fix - NULL return and tainted arg
* 
* 43   3/13/09 6:31p ssood
* PR50311: fixing a debug message
* 
* 42   3/13/09 5:21p ssood
* PR50311: socket was not being closed during rewind + removed extra
*  sleep before read
* 
* 41   3/10/09 5:07p ssood
* PR50311: rewind related fix for netaccel
* 
* 40   3/10/09 3:08p ssood
* PR50311: added support to rewind HTTP session
* 
* 39   3/6/09 7:18a ssood
* PR50311: increase the select timeout to match DLNA inactivity timeout
*  requirement of 30sec
* 
* 38   3/5/09 9:59p ssood
* PR50311: fix for differentiating between EOF & other network errors
* 
* 37   3/5/09 3:13p ssood
* PR50311: fix for compatibility with DLNA chunk xfer encoding test
* 
* 36   3/5/09 10:44a ssood
* PR50311: Fixed compatibility issues w/ Twonky Media Server: removed TE
*  header from request
* 
* 35   3/5/09 10:40a ssood
* PR50311: Adding support for handling larger response messages
* 
* 34   3/4/09 12:53p ssood
* PR50311: fixed a chunk xfer encoding related bug
* 
* 33   3/3/09 12:13p ssood
* PR50311: Adding support for processing HTTP 1xx status codes & larger
*  response messages
* 
* 32   3/2/09 2:44p ssood
* PR50311: brutus quit crashes when we quit from a HTTP channel where
*  connect attempt had failed
* 
* 31   3/1/09 10:38a ssood
* PR50311: added support for HTTP playback of content w/ extra 4 byte
*  DLNA timestamps
* 
* 30   2/27/09 2:27p lwhite
* PR50687: Enhanced psi probe by providing pseudo name
* 
* 30   2/27/09 2:24p lwhite
* PR50687: Enhanced psi probe by providing pseudo name
* 
* 29   2/27/09 11:55a ssood
* PR50311: Adding support for HTTP Chunk Transfer Encoding
* 
* 28   1/20/09 5:15p lwhite
* PR49537: Coverity fixes
* 
* 27   1/15/09 5:54p lwhite
* PR50687: Probe payload for psi info
* 
* 26   12/16/08 4:56p lwhite
* PR50203: Fixed WMV and MP4 streaming
* 
* 25   12/5/08 10:03a lwhite
* PR50000: Fixed HTTP memory leak
* 
* 24   11/26/08 3:30p lwhite
* PR49692: Set PSI probe defaults to false
* 
* 23   11/13/08 5:39p lwhite
* PR47685: Coverity fixes
* 
* 22   11/9/08 12:22p ssood
* PR48146: upon network error, change state to stopping only if not
*  already in stopped state
* 
* 21   11/9/08 11:45a ssood
* PR48133: added case-insensative searches for supporting more HTTP
*  servers
* 
* 20   11/7/08 5:30p ssood
* PR48146: HTTP processing now happens in the content of Nexus Playback
*  Player thread, so removing the local thread logic (as it was causing
*  sync issues during quit)
* 
* 19   11/7/08 11:29a ssood
* PR48146: After fix in bsettop_decode, no need to hardcode the PCR pid
*  (removing a workaround)
* 
* 18   11/5/08 11:25p ssood
* PR48146: missing variable in debug printing
* 
* 17   11/5/08 10:59p ssood
* PR48146: detect PCR PID as well during PSI discovery using Media Probe
*  Interface
* 
* 16   11/5/08 4:12p ssood
* PR48146: more work on support needed for MP4/ASF playback
* 
* 15   10/28/08 1:47p ssood
* PR48146: conditionally compiling HTTP MP4 support w/ HTTP_MP4_SUPPORT
*  env variable
* 
* 14   10/28/08 10:51a lwhite
* PR47685: Coverity fix
* 
* 15   10/28/08 1:43p ssood
* PR48146: conditionally compiling HTTP MP4 support w/ HTTP_MP4_SUPPORT
*  env variable
* 
* 12   10/16/08 10:53a lwhite
* PR44324: Coverity fixes
* 
* 11   10/13/08 5:38p ssood
* PR47521: code cleanup due to api changes
* 
* 11   10/13/08 5:31p ssood
* PR47521: code cleanup due to api changes
* 
* 10   9/3/08 1:59p lwhite
* PR42739: Http standard socket fixes
* 
* 9   8/15/08 9:04a lwhite
* PR42739: fixed DEBUG=n warnings
* 
* 8   8/11/08 10:13a ssood
* PR45431: Add HTTP playback support for ASF content
* 
* 7   8/5/08 1:12p ssood
* PR42739: splitting up UDP & RTP Processing code into separate sub-
*  modules
* 
* 6   8/5/08 7:59a ssood
* PR45400: make code compilable with -std=c89 for maximum compatibility
* 
* PR45400/1   8/5/08 12:44p dyzhang
* PR45400: kylin should compile with -std=c89 for maximum compatibility
* 
* 5   7/29/08 3:49p ssood
* PR42739: fixing HTTP decode related errors
* 
* 4   7/28/08 3:27p ssood
* PR42739: further changes for supporting example app for IP Applib
* 
* 3   7/21/08 4:06p ssood
* PR42739: code changes to simplify ip app lib interface
* 
* 3   7/21/08 4:06p ssood
* PR42739: code changes to simplify ip app lib interface
* 
* 2   6/13/08 9:52p ssood
* PR42739: Added check to monitor CDB depth & throttle sender for HTTP
*  playback
* 
* 1   6/13/08 7:27p ssood
* PR42739: Adding new files for HTTP playback
* 
***************************************************************************/

#if defined(LINUX) || defined(__vxworks)

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/sockios.h>

#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_utils.h"
#ifdef B_HAS_DTCP_IP
#include "b_dtcp_constants.h"
#endif
#ifdef B_HAS_HTTP_MP4_SUPPORT
#include "bmedia_probe.h"
#include "bmp4_probe.h"
#include "bmpeg2ts_probe.h"
#endif
#include "bfile_buffered.h"
#include "bsettop_types.h"
#if 0
#define BDBG_MSG_FLOW(x)  BDBG_WRN (x) ;
#else
#define BDBG_MSG_FLOW(x)  
#endif

BDBG_MODULE(b_playback_ip_http);
BDBG_FILE_MODULE(b_playback_ip_http_cache);
BDBG_FILE_MODULE(b_playback_ip_http_sockets);

#define PRINT_CACHE_MSG(bdbg_args)   BDBG_MODULE_MSG(b_playback_ip_http_cache,   bdbg_args)
#define PRINT_SOCKET_MSG(bdbg_args)  BDBG_MODULE_MSG(b_playback_ip_http_sockets, bdbg_args)

#define TMP_STRING_SIZE 512
#define DLNA_MAX_HTTP_RESP_SIZE (128*1024)  /* DLNA 1.5 guideline: 7.4.47 */

int B_PlaybackIp_ClearSessionOpen( B_PlaybackIpSessionOpenSettings *, int , B_PlaybackIpSecurityOpenOutputParams *);
void B_PlaybackIp_ClearSessionClose(void *);
#ifdef B_HAS_SSL
int B_PlaybackIp_SslSessionOpen( B_PlaybackIpSessionOpenSettings *, int , B_PlaybackIpSecurityOpenOutputParams *);
int B_PlaybackIp_SslCloneSessionOpen(int fd, void *securityHandle, void **newSecurityHandle);
#endif
#ifdef B_HAS_DTCP_IP
int B_PlaybackIp_DtcpIpSessionOpen( B_PlaybackIpSessionOpenSettings *, int , B_PlaybackIpSecurityOpenOutputParams *);
int B_PlaybackIp_DtcpIpCloneSessionOpen(int fd, void *securityHandle, void **newSecurityHandle);
int B_PlaybackIp_DtcpIpDecryptionEnable(void *securityHandle, char *initialPayload, int initialPayloadLength);
int B_PlaybackIp_DtcpIpDecryptionDisable(void *securityHandle);
#endif
#ifdef B_HAS_RAD_EA
int B_PlaybackIp_RadEaSessionOpen( B_PlaybackIpSessionOpenSettings *, int , B_PlaybackIpSecurityOpenOutputParams *securityOpenOutputParams); 
int B_PlaybackIp_RadEaCloneSessionOpen(int fd, void *securityHandle, void **newSecurityHandle);
int B_PlaybackIp_RadEaDecryptionEnable(void *securityHandle, char *initialPayload, int initialPayloadLength);
#endif
#ifdef B_HAS_HTTP_AES_SUPPORT
int B_PlaybackIp_AesSessionOpen(B_PlaybackIpSessionOpenSettings *, int , B_PlaybackIpSecurityOpenOutputParams *);
#endif
extern void B_PlaybackIp_HttpSetDefaultTrickModeSettings(B_PlaybackIpTrickModesSettings *ipTrickModeSettings);
B_PlaybackIpError B_PlaybackIp_HttpGetCurrentPlaybackPosition( B_PlaybackIpHandle playback_ip, NEXUS_PlaybackPosition *currentPosition);
extern B_PlaybackIpError http_send_time_seek_request(B_PlaybackIpHandle playback_ip);
int B_PlaybackIp_HlsSessionSetup(B_PlaybackIpHandle playback_ip, char *http_hdr);
void B_PlaybackIp_HlsSessionDestroy(B_PlaybackIpHandle playback_ip);
int B_PlaybackIp_MpegDashSessionSetup(B_PlaybackIpHandle playback_ip, char *http_hdr);
void B_PlaybackIp_MpegDashSessionDestroy(B_PlaybackIpHandle playback_ip);

char *MY_BKNI_Realloc(char *curBuffer, int curBufferSize, int newBufferSize)
{
    char *newBuffer;
    if ((newBuffer = (char *) BKNI_Malloc(newBufferSize)) == NULL) {
        BDBG_ERR(("%s: ERROR: Failed to realloc buffer, current size %d, new size %d\n", __FUNCTION__, curBufferSize, newBufferSize));
        return NULL;
    }
    /* now copy over the data */
    memcpy(newBuffer, curBuffer, curBufferSize);
    BKNI_Free(curBuffer);
    BDBG_MSG(("%s: realloced buffer from %d to %d bytes\n", __FUNCTION__, curBufferSize, newBufferSize));
    return newBuffer;
}

char *
find_base_uri(char *curUrl)
{
    char *mediaSegmentBaseUri, *tmp;
    char *endPtr = NULL;

    mediaSegmentBaseUri = curUrl;
    tmp = curUrl;
    while ((tmp = strstr(tmp, "/")) != NULL) {
        endPtr = tmp;
        tmp += 1; /* move past "/" char */
    }
    if (endPtr) {
        *(endPtr+1) = '\0'; /* null terminate mediaSegmentBaseUri string */
        mediaSegmentBaseUri = curUrl;
    }
    else {
        /* current url itself is relative, so replace it w/ the new one */
        mediaSegmentBaseUri = NULL;
    }
    return mediaSegmentBaseUri;
}

bool
http_absolute_uri(char *url)
{
    if (B_PlaybackIp_UtilsStristr(url, "http://") || B_PlaybackIp_UtilsStristr(url, "https://"))
        /* URI starts w/ protocol header, so it is a abslute URI */
        return true;
    else
        /* relative URI */
        return false;
}

int 
http_parse_url(char *server, unsigned *portPtr, char **uri, char *url)
{
    char *tmp1, *tmp2, *tmp3;
    char *newUri;

    if ( (tmp1 = B_PlaybackIp_UtilsStristr(url, "http:")) == NULL) {
        /* relative url: just replace the last path of the current url w/ the new url, rest server name & port dont change */
        char *mediaSegmentBaseUri;
        int newUriLen;
        BDBG_MSG(("%s: current uri %s, new relative URI (%s)", __FUNCTION__, *uri, url));
        if ( (mediaSegmentBaseUri = find_base_uri(*uri)) != 0) {
            newUriLen = strlen(mediaSegmentBaseUri) + strlen(url) + 1;
            if ( (newUri = (char *)malloc(newUriLen)) == NULL) {
                BDBG_ERR(("%s: Malloc failure for %d bytes", __FUNCTION__, newUriLen));
                return -1;
            }
            memset(newUri, 0, newUriLen);
            strncpy(newUri, mediaSegmentBaseUri, strlen(mediaSegmentBaseUri));
            strncat(newUri, url, strlen(url));
            free(*uri);  /* as it was allocated via strdup of original application passed url */
            *uri = newUri;
        }
        else {
            /* base url itself is relative, so replace it w/ new one */
            newUri = strdup(url);
            free(*uri);  /* as it was allocated via strdup of original application passed url */
            *uri = newUri;
        }
        BDBG_MSG(("%s: updated relative URI %s", __FUNCTION__, *uri));
        return 0;
    }
    /* http protocol is being used, parse it further */
    tmp1 += strlen("http://");

    /* now take out the server string from the url */
    tmp2 = strstr(tmp1, "/");
    if (tmp2) {
        strncpy(server, tmp1, (tmp2 - tmp1));
        server[tmp2-tmp1] = 0;

        /* Check to see if a port value was specified */
        tmp3 = strstr(server, ":");
        if (tmp3) {
            tmp3[0] = '\0'; /* this null terminates the server name string */
            tmp3++;
            *portPtr = strtoul(tmp3, (char **) NULL, 10);
        }
        else 
            *portPtr = 80;

        /* now get the uri */
        newUri = strdup(tmp2);
        free(*uri);  /* as it was allocated via strdup of original application passed url */
        *uri = newUri;
        BDBG_MSG(("server %s url %s", server, *uri));
        return 0;
    }
    else {
        BDBG_ERR(("%s: Failed to find the server part from the redirected URL %s", __FUNCTION__, url));
    }

    return -1;
}

int http_redirect_get_full_location_header(char **urlPtr, char *http_hdr)
{
    char *tmp1, *tmp2;

    if ( (tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Location: http:")) == NULL) {
        BDBG_ERR(("No Location header in the HTTP redirect\n"));
        return -1;
    }
    /* redirect header is present, move past it */
    tmp1 += strlen("Location:");
    if ( ((tmp2 = B_PlaybackIp_UtilsStristr(tmp1, "\r\n")) != NULL) || ((tmp2 = B_PlaybackIp_UtilsStristr(tmp1, "\n")) != NULL)) {
        *tmp2 = '\0';
        free(*urlPtr);  /* as it was allocated via strdup of original application passed url */
        *urlPtr = strdup(tmp1);
        BDBG_MSG(("%s: full location header url %s", __FUNCTION__, *urlPtr));
        return 0;
    }
    
    return -1;
}

int http_parse_redirect(char *server, unsigned *portPtr, B_PlaybackIpProtocol *protocol, char **urlPtr, char **cookie, char *http_hdr)
{
    char *tmp1, *tmp2, *tmp3;
    *cookie = NULL;

    if ( (tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Location:")) == NULL) {
        BDBG_ERR(("No Location header in the HTTP redirect"));
        return -1;
    }
    /* redirect header is present, move past it */
    tmp1 += strlen("Location:");

    /* now look for protocol: supported ones are either HTTP or HTTPs */
    if ((tmp2 = B_PlaybackIp_UtilsStristr(tmp1, "http://")) != NULL) {
        tmp1 = tmp2 + strlen("http://");
        *protocol = B_PlaybackIpProtocol_eHttp;
    }
    else if ((tmp2 = B_PlaybackIp_UtilsStristr(tmp1, "https://")) != NULL) {
        tmp1 = tmp2 + strlen("https://");
        *protocol = B_PlaybackIpProtocol_eHttps;
    }
    else {
        BDBG_ERR(("Redirect URI contains a non-supported protocol: http header %s", http_hdr));
        return -1;
    }

    /* now take out the server string from the Location header */
    tmp2 = strstr(tmp1, "/");
    if (tmp2) {
        strncpy(server, tmp1, (tmp2 - tmp1));
        server[tmp2-tmp1] = 0;

        /* Check to see if a port value was specified */
        tmp3 = strstr(server, ":");
        if (tmp3) {
            tmp3[0] = '\0'; /* this null terminates the server name string */
            tmp3++;
            *portPtr = strtoul(tmp3, (char **) NULL, 10);
        }

        /* now get the redirected URL */
        if ( (tmp1 = B_PlaybackIp_UtilsStristr(tmp2, "\r\n")) != NULL) {
            if (*urlPtr)
                free(*urlPtr);  /* as it was allocated via strdup of original application passed url */
            *tmp1 = '\0';
            *urlPtr = strdup(tmp2);
            BDBG_MSG(("%s: server %s:%d url %s", __FUNCTION__, server, *portPtr, *urlPtr));
            *tmp1 = '\r';
            if ( (tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Set-Cookie:")) != NULL) {
                tmp1 += strlen("Set-Cookie:");
                if ( (tmp2 = B_PlaybackIp_UtilsStristr(tmp1, "\r\n")) != NULL) {
                    int cookieHeaderLength;
                    *tmp2 = '\0';
                    cookieHeaderLength = strlen(tmp1) + 1; /* +1 for the string terminator */
                    if ((*cookie = BKNI_Malloc(cookieHeaderLength)) == NULL) {
                        BDBG_ERR(("%s: ERROR: Failed to alloc buffer of size %d for cookie header", __FUNCTION__, cookieHeaderLength));
                        return -1;
                    }
                    BKNI_Memset(*cookie, 0, cookieHeaderLength);
                    strncpy(*cookie, tmp1, cookieHeaderLength-1);
                    *tmp2 = '\r';
                    BDBG_MSG(("%s: found cookie in the redirect reponse: %s", __FUNCTION__, *cookie));
                }
                else {
                    BDBG_ERR(("Failed to find CRNL in the Set-Cookie header of the HTTP redirect response"));
                    return -1;
                }
            }
            return 0;
        }
        else {
            BDBG_ERR(("Failed to find CRNL in the Location header of the HTTP response\n"));
        }
    }
    else 
    {
        BDBG_ERR(("Failed to find the server part from the Location string of HTTP response\n"));
    }

    return -1;
}

/* returns the URL type */
http_url_type_t http_get_url_type(char *http_hdr, char *url)
{
    char *tmp1;
    char tmp2[TMP_STRING_SIZE+1];
    /* search for Location tag in the http response, it indicates redirection */
    if (B_PlaybackIp_UtilsStristr(http_hdr, "Location: http://") != NULL) {
        /* redirection */
        BDBG_MSG(("Response contains a HTTP Redirect\n"));
        return HTTP_URL_IS_REDIRECT;
    }

    /* find the URL extension, e.g. /download.asp */
    tmp1 = strstr(url, ".");
    if (!tmp1)
        return HTTP_URL_IS_DIRECT;
    strncpy(tmp2, tmp1, TMP_STRING_SIZE);
    /* if URL extension contains any URL modifiers, exclude them */
    tmp1 = NULL;
    tmp1 = strstr(tmp2, "?");
    if (tmp1) 
        *tmp1 = 0;
    BDBG_MSG(("extension is %s\n", tmp2));
    if (
        !strcmp(tmp2, ".asp") ||
        !strcmp(tmp2, ".ASP") ||
        !strcmp(tmp2, ".asx") ||
        !strcmp(tmp2, ".ASX")
        )
        return HTTP_URL_IS_ASX;
    else
        return HTTP_URL_IS_DIRECT;
}

/* returns server & url string from the ASX header */
int http_parse_asx_payload(char *server, char **urlPtr, char *asx_hdr)
{
    char *tmp1, *tmp2;

    if ( (tmp1 = B_PlaybackIp_UtilsStristr(asx_hdr, "ENTRYREF HREF=\"http://")) != NULL) {
        /* ENTRYREF element is present, its HREF attribute points an external ASX file, get is URL */
        tmp1 += strlen("ENTRYREF HREF=\"http://");
    }
    else if ( (tmp1 = strstr(asx_hdr, "Ref href=\"http://")) != NULL) {
        /* no ENTRYREF element, so this ASX payload must contain the REF element */
        /* its HREF attribute will point to the URL of the content */
        tmp1 += strlen("Ref href=\"http://");
    }
    else {
        BDBG_ERR(("ERROR: Neither ENTRYREF HREF nor Ref href elements found\n"));
        return -1;
    }
    if ( (tmp2 = strstr(tmp1, "/")) != NULL ) {
        strncpy(server, tmp1, (tmp2 - tmp1));
        server[tmp2-tmp1] = 0;

        if ( (tmp1 = strstr(tmp2, "\"")) != NULL) {
            free(*urlPtr);  /* as it was allocated via strdup of original application passed url */
            *tmp1 = '\0';
            *urlPtr = strdup(tmp2);
            BDBG_MSG(("server %s url %s\n", server, *urlPtr));
            return 0;
        }
    }

    return -1;
}

NEXUS_TransportType 
http_get_payload_content_type(char *http_hdr) 
{
    char *tmp1;

    tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Content-Type: video/");
    if (tmp1) {
        tmp1 += strlen("Content-type: video/");
        if (B_PlaybackIp_UtilsStristr(tmp1, "x-ms-wmv") || B_PlaybackIp_UtilsStristr(tmp1, "x-ms-asf")) {
            BDBG_MSG(("%s:WMV Content", __FUNCTION__));
            return NEXUS_TransportType_eAsf;
        }
        else if (B_PlaybackIp_UtilsStristr(tmp1, "mpeg") || B_PlaybackIp_UtilsStristr(tmp1, "mpg") || B_PlaybackIp_UtilsStristr(tmp1, "mp2t")) {
            BDBG_MSG(("%s:MPEG2 TS Content", __FUNCTION__));
            return NEXUS_TransportType_eTs;
        }
        else if (B_PlaybackIp_UtilsStristr(tmp1, "mp4")) {
            BDBG_MSG(("%s:MP4 Content", __FUNCTION__));
            return NEXUS_TransportType_eMp4;
        }
    }
    tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Content-Type: application/flv");
    if (tmp1) {
        BDBG_MSG(("%s:MP4 Content", __FUNCTION__));
        return NEXUS_TransportType_eMp4;
    }
    tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Content-Type: video/x-flv");
    if (tmp1) {
        BDBG_MSG(("%s:Flash Content", __FUNCTION__));
        return NEXUS_TransportType_eFlv;
    }
    tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Content-Type: audio/");
    if (tmp1) {
        if (B_PlaybackIp_UtilsStristr(tmp1, "x-ms-wma") || B_PlaybackIp_UtilsStristr(tmp1, "x-ms-asf")) {
            BDBG_MSG(("%s:WMA Content", __FUNCTION__));
            return NEXUS_TransportType_eAsf;
        }
        else if (B_PlaybackIp_UtilsStristr(tmp1, "mpeg") || B_PlaybackIp_UtilsStristr(tmp1, "mpg") || 
                 B_PlaybackIp_UtilsStristr(tmp1, "x-aac") || B_PlaybackIp_UtilsStristr(tmp1, "mp3")) {
            BDBG_MSG(("%s:MPEG Audio Content", __FUNCTION__));
            return NEXUS_TransportType_eEs;
        }
    }
    /* some HTTP server specify MP4 movie content under audio */
    tmp1 = B_PlaybackIp_UtilsStristr(http_hdr, "Content-Type: ");
    if (tmp1) {
        tmp1 += strlen("Content-type: ");
        if (B_PlaybackIp_UtilsStristr(tmp1, "x-ms-wmv") || B_PlaybackIp_UtilsStristr(tmp1, "x-ms-asf")) {
            BDBG_MSG(("%s:WMV Content", __FUNCTION__));
            return NEXUS_TransportType_eAsf;
        }
        else if (B_PlaybackIp_UtilsStristr(tmp1, "mpeg") || B_PlaybackIp_UtilsStristr(tmp1, "mpg")) {
            BDBG_MSG(("%s:MPEG2 TS Content", __FUNCTION__));
            return NEXUS_TransportType_eTs;
        }
        else if (B_PlaybackIp_UtilsStristr(tmp1, "mp4")) {
            BDBG_MSG(("%s:MP4 Content", __FUNCTION__));
            return NEXUS_TransportType_eMp4;
        }
    }
    return NEXUS_TransportType_eUnknown;
}

/* 
 * This function writes the given data to the socket and returns any errors or the bytes wrote.
 * It returns: 
 *  =-1: for errors other than EINTR & EAGAIN during write call or when channel change occurs
 *  = 0: for EOF where server closed the TCP connection
 *  > 0: for success indicating number of bytes written to the socket
 */
static int _http_socket_write(void *voidHandle, volatile B_PlaybackIpState *playbackIpState, int sd, char *wbuf, int wbuf_len)
{
    int rc;

    BSTD_UNUSED(voidHandle);

    rc = B_PlaybackIp_UtilsTcpSocketWrite(playbackIpState, sd, wbuf, wbuf_len);
    if (rc <= 0) {
        BDBG_ERR(("%s: write System Call interrupted or timed out, retrying (rc %d, errno %d)\n", __FUNCTION__, rc, errno));
        return rc;
    }
    return wbuf_len;
}

/* builds HTTP get request */
int http_build_get_req(char **write_buf_pptr, B_PlaybackIpHandle playback_ip, int speed, double timeSeekRangeBegin, double timeSeekRangeEnd, off_t byteRangeStart, off_t byteRangeEnd)
{
    B_PlaybackIpSocketOpenSettingsHandle socketOpenSettings = &playback_ip->openSettings.socketOpenSettings; 
    B_PlaybackIpSessionOpenSettings *openSettings = &playback_ip->openSettings;
    unsigned int bytesLeft;
    int bytesWrote;
    int write_buf_size;
    char *write_buf;
    char *header;

    write_buf_size = strlen(socketOpenSettings->ipAddr) + strlen(socketOpenSettings->url);
    if (openSettings->u.http.userAgent) 
        write_buf_size += strlen(openSettings->u.http.userAgent);
    if (openSettings->u.http.additionalHeaders)
        write_buf_size += strlen(openSettings->u.http.additionalHeaders);
    write_buf_size += 512; /* add room for rest of HTTP headers which have fixed sizes */
    write_buf = (char *)BKNI_Malloc(write_buf_size);
    if (!write_buf) {
        BDBG_ERR(("%s: ERROR: failed to allocate memory\n", __FUNCTION__));
        return -1;
    }
    memset(write_buf, 0, write_buf_size);
    bytesLeft = write_buf_size;
    bytesWrote = 0;
    header = write_buf;
    *write_buf_pptr = write_buf;

    bytesWrote = snprintf(header, bytesLeft,
            "GET %s HTTP/1.1\r\n"
            "User-Agent: %s\r\n"
            "Connection: Close\r\n"
            "%s"
            "EncEnabled: %s\r\n"
            ,
            socketOpenSettings->url,
            (openSettings->u.http.userAgent ? openSettings->u.http.userAgent : "BRCM HTTP Client/2.0"),
            (openSettings->u.http.additionalHeaders ? openSettings->u.http.additionalHeaders : "") ,
            openSettings->security.securityProtocol == B_PlaybackIpSecurityProtocol_DtcpIp? "Yes":"No"
            );
    bytesLeft -= bytesWrote;
    header += bytesWrote;

    if (!playback_ip->openSettings.socketOpenSettings.useProxy) {
        if (socketOpenSettings->port == HTTP_DEFAULT_PORT) {
            bytesWrote = snprintf(header, bytesLeft, "Host: %s\r\n", socketOpenSettings->ipAddr);
        }
        else {
            /* not using the default HTTP port, so Host Header needs to include the port # */
            bytesWrote = snprintf(header, bytesLeft, "Host: %s:%d\r\n", socketOpenSettings->ipAddr, socketOpenSettings->port);
        }
    }
    else {
        char *tmp1, *tmp2;
        tmp1 = strstr(socketOpenSettings->url, "://"); /* host name starts after this sequence */
        if (tmp1) {
            tmp1 += 3; /* move past "://" */
            tmp2 = strstr(tmp1, "/");   /* host name ends here and URI starts after this */
            if (tmp2) {
                *tmp2 = '\0';
                bytesWrote = snprintf(header, bytesLeft, "Host: %s\r\n", tmp1);
                *tmp2 = '/';
            }
        }
    }
    bytesLeft -= bytesWrote;
    header += bytesWrote;
    if (openSettings->u.http.rvuCompliant) {
        /* RVU server always requires timeSeekRange header */
        if (timeSeekRangeEnd != 0.)
            bytesWrote = snprintf(header, bytesLeft, "TimeSeekRange.dlna.org: npt=%.3f-%.3f\r\n", timeSeekRangeBegin, timeSeekRangeEnd);
        else
            bytesWrote = snprintf(header, bytesLeft, "TimeSeekRange.dlna.org: npt=%.3f-\r\n", timeSeekRangeBegin);
        bytesLeft -= bytesWrote;
        header += bytesWrote;

        if (speed == 1) {
            /* RVU server only requires play speed for normal play mode. Also, speed keyword is not required in the syntax */
            bytesWrote = snprintf(header, bytesLeft, "PlaySpeed.dlna.org: speed=%d\r\n", speed);
        }
        else {
            /* include the Frame Count header */
            bytesWrote = snprintf(header, bytesLeft, "frameCount.rvualliance.org: 1\r\n");
        }
        bytesLeft -= bytesWrote;
        header += bytesWrote;
    }
    else {
        if (timeSeekRangeBegin > 0.) {
            if (timeSeekRangeEnd > timeSeekRangeBegin) {
                bytesWrote = snprintf(header, bytesLeft, "TimeSeekRange.dlna.org: npt=%.3f-%.3f\r\n", timeSeekRangeBegin, timeSeekRangeEnd);
            }
            else {
                bytesWrote = snprintf(header, bytesLeft, "TimeSeekRange.dlna.org: npt=%.3f-\r\n", timeSeekRangeBegin);
            }
            bytesLeft -= bytesWrote;
            header += bytesWrote;
        }
        if (speed != 1) {
            bytesWrote = snprintf(header, bytesLeft, "PlaySpeed.dlna.org: speed=%d\r\n", speed);
            bytesLeft -= bytesWrote;
            header += bytesWrote;
        }
    }
    if (byteRangeStart != 0) {
        char *rangeString;
        if (openSettings->security.securityProtocol == B_PlaybackIpSecurityProtocol_DtcpIp)
            rangeString = "Range.dtcp.com:";
        else
            rangeString = "Range:";
        if (byteRangeEnd > byteRangeStart)
            bytesWrote = snprintf(header, bytesLeft, "%s bytes=%lld-%lld\r\n", rangeString, byteRangeStart, byteRangeEnd);
        else {
            if (playback_ip->contentLength != 0)
                bytesWrote = snprintf(header, bytesLeft, "%s bytes=%lld-%lld\r\n", rangeString, byteRangeStart, playback_ip->contentLength-1);
            else
                bytesWrote = snprintf(header, bytesLeft, "%s bytes=%lld-\r\n", rangeString, byteRangeStart);
        }
        bytesLeft -= bytesWrote;
        header += bytesWrote;
    }
    if (playback_ip->cookieFoundViaHttpRedirect) {
        /* add the cookie header */
        bytesWrote = snprintf(header, bytesLeft, "Cookie: %s\r\n", playback_ip->cookieFoundViaHttpRedirect);
        bytesLeft -= bytesWrote;
        header += bytesWrote;
    }
    bytesWrote = snprintf(header, bytesLeft, "\r\n");
    bytesLeft -= bytesWrote;
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("Requesting URL: http://%s:%d%s", socketOpenSettings->ipAddr, socketOpenSettings->port, socketOpenSettings->url));
#endif
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog) {
        BDBG_WRN(("Complete HTTP Get request:"));
        fprintf(stdout, "%s", write_buf);
        fflush(stdout);
    }
#endif
    return 0;
}

/* 
 * This function tries to read the requested amount from the socket and returns any errors or bytes read.
 * It returns: 
 *  =-1: for select timeout a/f HTTP_SELECT_TIMEOUT secs, errors other than EINTR, or when channel change occurs
 *  = 0: for read ready event on the socket when some data is available for reading.
 */
int 
_http_socket_select(B_PlaybackIpHandle playback_ip, int fd, int timeout) 
{
    int rc = -1;
    fd_set rfds;
    struct timeval tv;
    int timeoutInterval = 200; /* 200msec */
    int timeoutIterations = (timeout * 1000) / timeoutInterval;
    int numTimeoutInterations = 0;

    while (numTimeoutInterations++ < timeoutIterations) {
        if ((playback_ip->playback_state == B_PlaybackIpState_eStopping) || (playback_ip->playback_state == B_PlaybackIpState_eStopped) || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)) {
            /* user changed the channel, so return */
            PRINT_SOCKET_MSG(("%s: breaking out of select loop due to state (%d) change, # of timeout iterations %d, total timeout %d seconds\n", __FUNCTION__, playback_ip->playback_state, numTimeoutInterations, (numTimeoutInterations*timeoutInterval)/1000));
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)
                playback_ip->selectTimeout = true;
            else
                playback_ip->serverClosed = true;
            return -1;
        }
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = 0; 
        tv.tv_usec = timeoutInterval * 1000; /* in micro-secs */

        rc = select(fd +1, &rfds, NULL, NULL, &tv);
        if (rc < 0) {
            if (errno == EINTR) {
                BDBG_MSG(("%s: select System Call interrupted, retrying\n", __FUNCTION__));
                continue;
            }
            BDBG_ERR(("%s: ERROR: select(): rc %d, errno %d, fd %d", __FUNCTION__, rc, errno, fd));
            /* got some socket error, return failure */
            playback_ip->serverClosed = true;
            return -1;
        }

        if (rc == 0 || !FD_ISSET(fd, &rfds)) {
            /* select timeout or some select event but our FD not set: No more data - wait */
            playback_ip->selectTimeout = true;
            PRINT_SOCKET_MSG(("%s: selectTimeout is true", __FUNCTION__));
            continue;
        }
        /* ready ready event on socket, return succcess */
        PRINT_SOCKET_MSG(("%s: select read ready event (before the timeout of %d sec), iterations: cur %d, max %d", __FUNCTION__, timeout, numTimeoutInterations, timeoutIterations));
        return 0;
    }
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: select timed out after %d sec, iterations: cur %d, max %d\n", __FUNCTION__, timeout, numTimeoutInterations, timeoutIterations));
#endif
    /* select had just timed out due to no data being available on socket */
    playback_ip->selectTimeout = true;
    return -1;
}

/* 
 * This function tries to read the requested amount from the socket and returns any errors or bytes read.
 * It returns: 
 *  =-1: for errors other than EINTR & EAGAIN during read call or when channel change occurs
 *  = 0: for EOF where server closed the TCP connection
 *  > 0: for success indicating number of bytes read from the socket
 */
int 
_http_socket_read(void *voidHandle, B_PlaybackIpHandle playback_ip, int sd, char *rbuf, int rbuf_len)
{
    int rc;
    BSTD_UNUSED(voidHandle);    /* not needed for clear net operations */
    while (true) {
        if ((playback_ip->playback_state == B_PlaybackIpState_eStopping) || (playback_ip->playback_state == B_PlaybackIpState_eStopped) || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of read loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
                playback_ip->selectTimeout = true;
                errno = EINTR;
            }
            else {
                playback_ip->serverClosed = true;
            }
            return -1;
        }
        rc = read(sd, rbuf, rbuf_len);
        if (rc < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                BDBG_ERR(("%s: Read System Call interrupted or timed out, sleeping for 100msec & then retrying (errno %d)", __FUNCTION__, errno));
                BKNI_Sleep(100);
                continue;
            }
            BDBG_ERR(("%s: read ERROR:%d", __FUNCTION__, errno));
            return -1;
        }
        else if (rc == 0) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: Reached EOF, server closed the connection!", __FUNCTION__));
#endif
            return 0;
        }
        BDBG_MSG_FLOW(("%s: bytes read %d", __FUNCTION__, rc));
        return rc;
    }
    /* we shouldn't get here */
    BDBG_ASSERT(NULL);
}

/* This function loops until requested amount is read from the socket unless there is a socket error.
 * or channel is changed. In addition, it performs all relevent error checking.
 * It returns: 
 *  =-1: for errors other than EINTR & EAGAIN during read call or when channel change occurs
 *  = 0: for EOF where server closed the TCP connection, serverClosed flag is set
 *  > 0: for success indicating number of bytes read from the socket. 
 *       if (bytesRead != bytesToRead), serverClosed flag is also set.
 */
int 
http_socket_read(B_PlaybackIpHandle playback_ip, void *securityHandle, int sd, char *buf, int bytesToRead, int *bytesRead, int timeout)
{
    int rc = -1;

    /* loop until requested data is read from socket */
    *bytesRead = 0;
    playback_ip->serverClosed = false;
    playback_ip->selectTimeout = false;
    while (*bytesRead < bytesToRead) {
        if  ((playback_ip->playback_state == B_PlaybackIpState_eStopping) || (playback_ip->playback_state == B_PlaybackIpState_eStopped) || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode))  {
            /* user changed the channel, so return */
            PRINT_SOCKET_MSG(("%s: breaking out of read loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)
                playback_ip->selectTimeout = true;
            else
                playback_ip->serverClosed = true;
            break;
        }

        /* wait for some data to be available on this socket */
        if (_http_socket_select(playback_ip, sd, timeout)) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: select ERROR:%d, state %d, local socket info: ip:port = %s:%d",
                        __FUNCTION__, errno, playback_ip->playback_state, inet_ntoa(playback_ip->socketState.local_addr.sin_addr),
                        ntohs(playback_ip->socketState.local_addr.sin_port)));
#endif
            rc = -1;
            break;
        }

        /* read from the socket */
        PRINT_SOCKET_MSG(("%s: before reading: bytes read %d, asked %d, remaining %d, chunkSize %lld", __FUNCTION__, *bytesRead, bytesToRead, (bytesToRead - *bytesRead), playback_ip->chunkSize));
        if ( (rc = playback_ip->netIo.read(securityHandle, playback_ip, sd, (buf+*bytesRead), (bytesToRead - *bytesRead))) <= 0) {
            if (rc != 0 && (errno == EINTR || errno == EAGAIN)) {
                playback_ip->selectTimeout = true;
                rc = -1;
                PRINT_SOCKET_MSG(("%s: read ERROR:%d, selectTimeout is set to true\n", __FUNCTION__, errno));
#ifdef B_HAS_DTCP_IP
                if (playback_ip->dtcpPcpHeaderFound) {
                    goto dtcpHeaderAdjustment;
                }
#endif
                break;
            }
            if (rc == 0) { /* I dont think we need to wait for last chunk to arrive before settings the server closed flag */
                playback_ip->serverClosed = true;
                PRINT_SOCKET_MSG(("%s: read ERROR:%d, setting serverClosed flag to true, rc %d", __FUNCTION__, errno, rc));
            }
            break;
        }
        *bytesRead += rc;
        PRINT_SOCKET_MSG(("after reading: bytes read: cur %d, total %d, asked %d", rc, *bytesRead, bytesToRead));
#ifdef B_HAS_DTCP_IP
        if (playback_ip->dtcpPcpHeaderFound) {
dtcpHeaderAdjustment:
            playback_ip->chunkSize -= DTCP_CONTENT_PACKET_HEADER_SIZE;
            playback_ip->bytesLeftInCurChunk -= DTCP_CONTENT_PACKET_HEADER_SIZE;
            if (playback_ip->adjustBytesToRead) {
                bytesToRead -= DTCP_CONTENT_PACKET_HEADER_SIZE;
                playback_ip->adjustBytesToRead = false;
            }
            if (playback_ip->chunkSize < 0 || playback_ip->bytesLeftInCurChunk < 0 || bytesToRead < 0) {
                BDBG_ERR(("BUG: chunSize %dll is < 0, bytesLeftInChunk %lld, bytesToRead %d", playback_ip->chunkSize, playback_ip->bytesLeftInCurChunk, bytesToRead));
                playback_ip->chunkSize = 0;
                BDBG_ASSERT(NULL);
            }
            playback_ip->dtcpPcpHeaderFound = false;
            PRINT_SOCKET_MSG(("%s: bytes read %d, asked %d, adjusting by DTCP PCP Header length of 14 bytes, bytesLeftInCurCHunk %lld, chunkSize %lld\n", 
                        __FUNCTION__, *bytesRead, bytesToRead, playback_ip->bytesLeftInCurChunk, playback_ip->chunkSize));
        }
#endif
        /* if select timeout was set during the last read, break out from the
         * read loop. This is done to avoid getting stuck in read loop for
         * cases when low bitrate content is being received.
         */
        if (playback_ip->selectTimeout) {
            PRINT_SOCKET_MSG(("%s: breaking from reading loop due to select timeout, bytes read %d", __FUNCTION__, rc));
            break;
        }
    }
    if (*bytesRead) {
        playback_ip->totalRead += *bytesRead;
        PRINT_SOCKET_MSG(("%s: fd %d, bytes read %d, asked %d\n", __FUNCTION__, sd, *bytesRead, bytesToRead));
        return *bytesRead;
    }
    else 
        return rc;
}

int B_PlaybackIp_ClearSessionOpen(
    B_PlaybackIpSessionOpenSettings *openSettings,
    int sd,                                                     /* input: socket descriptor */
    B_PlaybackIpSecurityOpenOutputParams *securityOpenParams) /* output: security return parameter */
{
    /* For clear content, we only need to setup the netIo interface */
    BSTD_UNUSED(openSettings);
    BSTD_UNUSED(sd);

    securityOpenParams->byteRangeOffset = 0;

    BDBG_MSG(("%s: setting up the netIo interface for socket read & write\n", __FUNCTION__));
    securityOpenParams->netIoPtr->read = _http_socket_read;
    securityOpenParams->netIoPtr->write = _http_socket_write;
    securityOpenParams->netIoPtr->close = B_PlaybackIp_ClearSessionClose;
    securityOpenParams->netIoPtr->suspend = NULL;
    securityOpenParams->netIoPtr->shutdown = NULL;
    return 0;
}

void B_PlaybackIp_ClearSessionClose(
    void *securityHandle)                                      /* input: security module specific handle */
{
    /* For clear content, there is nothing to do */
    BSTD_UNUSED(securityHandle);
}

int 
B_PlaybackIp_SecuritySessionOpen(
    B_PlaybackIpHandle playback_ip, 
    B_PlaybackIpSessionOpenSettings *openSettings,
    int fd,
    void **newSecurityHandle)
{
    int rc = 0;
    B_PlaybackIpSecurityOpenOutputParams securityOpenOutputParams;
    BDBG_ASSERT(playback_ip);

    securityOpenOutputParams.netIoPtr = &playback_ip->netIo;
    securityOpenOutputParams.securityHandle = newSecurityHandle;
    securityOpenOutputParams.byteRangeOffset = 0; 

    /* set the default session settings */
    B_PlaybackIp_ClearSessionOpen(openSettings, fd, &securityOpenOutputParams);
    switch (openSettings->security.securityProtocol) {
        case B_PlaybackIpSecurityProtocol_None:    /* Clear Content */
            rc = 0;
            break;
#ifdef B_HAS_SSL
        case B_PlaybackIpSecurityProtocol_Ssl:     /* SSL/TLS protected Content */
            BDBG_MSG(("%s: Using SSL/TLS protocol", __FUNCTION__));
            openSettings->security.enableDecryption = true;    /* For SSL/TLS, HTTP Response is also encrypted */
            rc = B_PlaybackIp_SslSessionOpen(openSettings, fd, &securityOpenOutputParams );
            break;
#endif
#ifdef B_HAS_DTCP_IP
        case B_PlaybackIpSecurityProtocol_DtcpIp:  /* DTCP-IP protected Content */
            openSettings->security.enableDecryption = false;    /* For DTCP/IP, HTTP Response is in clear */
            BDBG_MSG(("%s: Using DTCP-IP protocol", __FUNCTION__));
            rc = B_PlaybackIp_DtcpIpSessionOpen(openSettings, fd, &securityOpenOutputParams );
            break;
#endif
#ifdef B_HAS_RAD_EA
        case B_PlaybackIpSecurityProtocol_RadEa:   /* Rhapsody's RAD-EA protected Content */
            openSettings->security.enableDecryption = false;    /* For RAD/EA, HTTP Response is in clear */
            BDBG_MSG(("%s: Using Rhapsody's RAD-EA protocol", __FUNCTION__));
            rc = B_PlaybackIp_RadEaSessionOpen(openSettings, fd, &securityOpenOutputParams );
            break;
#endif
#ifdef B_HAS_HTTP_AES_SUPPORT
        case B_PlaybackIpSecurityProtocol_Aes128:  /* AES protected Content */
            openSettings->security.enableDecryption = false;    /* For AES, HTTP Response is in clear */
            BDBG_MSG(("%s: Using AES protocol", __FUNCTION__));
            rc = B_PlaybackIp_AesSessionOpen(openSettings, fd, &securityOpenOutputParams );
            break;
#endif
        default: 
            BDBG_ERR(("%s: Security protocol (%d) is not supported\n", __FUNCTION__, openSettings->security.securityProtocol));
            return -1;
    }

    playback_ip->byteRangeOffset = securityOpenOutputParams.byteRangeOffset;

    return rc;
}

int B_PlaybackIp_SecurityCloneSessionOpen(
    B_PlaybackIpHandle playback_ip, 
    int fd,
    void *securityHandle,
    void **newSecurityHandle)
{
    int rc = 0;
    BDBG_ASSERT(playback_ip);

    switch (playback_ip->openSettings.security.securityProtocol) {
        case B_PlaybackIpSecurityProtocol_None:    /* Clear Content */
            BSTD_UNUSED(securityHandle);
            BSTD_UNUSED(newSecurityHandle);
            BSTD_UNUSED(fd);
            rc = 0;
            break;
#ifdef B_HAS_SSL
        case B_PlaybackIpSecurityProtocol_Ssl:     /* SSL/TLS protected Content */
            BDBG_MSG(("%s: Using SSL/TLS protocol\n", __FUNCTION__));
            rc = B_PlaybackIp_SslCloneSessionOpen(fd, securityHandle, newSecurityHandle);
            break;
#endif
#ifdef B_HAS_DTCP_IP
        case B_PlaybackIpSecurityProtocol_DtcpIp:  /* DTCP-IP protected Content */
            BDBG_MSG(("%s: Using DTCP-IP protocol\n", __FUNCTION__));
            rc = B_PlaybackIp_DtcpIpCloneSessionOpen(fd, securityHandle, newSecurityHandle);
            break;
#endif
#ifdef B_HAS_RAD_EA
        case B_PlaybackIpSecurityProtocol_RadEa:   /* Rhapsody's RAD-EA protected Content */
            BDBG_MSG(("%s: Using Rhapsody's RAD-EA protocol\n", __FUNCTION__));
            rc = B_PlaybackIp_RadEaCloneSessionOpen(fd, securityHandle, newSecurityHandle);
            break;
#endif
        default: 
            BDBG_ERR(("%s: Security protocol (%d) is not supported\n", __FUNCTION__, playback_ip->openSettings.security.securityProtocol));
            return -1;
    }
    return rc;
}

int B_PlaybackIp_SecurityDecryptionDisable(
    B_PlaybackIpHandle playback_ip
    )
{
    int rc = 0;
    BDBG_ASSERT(playback_ip);

    /* set the default session settings */
    switch (playback_ip->openSettings.security.securityProtocol) {
        case B_PlaybackIpSecurityProtocol_None:    /* Clear Content */
            break;
#ifdef B_HAS_SSL
        case B_PlaybackIpSecurityProtocol_Ssl:     /* SSL/TLS protected Content */
            /* No action needs to be taken as all payload is already decrypted before it gets to this HTTP layer */
            break;
#endif
#ifdef B_HAS_DTCP_IP
        case B_PlaybackIpSecurityProtocol_DtcpIp:  /* DTCP-IP protected Content */
            rc = B_PlaybackIp_DtcpIpDecryptionDisable(playback_ip->securityHandle);
            BDBG_MSG(("%s: Disabled DTCP/IP Decryption \n", __FUNCTION__));
            break;
#endif
#ifdef B_HAS_RAD_EA
        case B_PlaybackIpSecurityProtocol_RadEa:   /* Rhapsody's RAD-EA protected Content */
            break;
#endif
        default: 
            BDBG_ERR(("%s: Security protocol (%d) is not supported\n", __FUNCTION__, playback_ip->openSettings.security.securityProtocol));
            return -1;
    }
    return rc;
}

int B_PlaybackIp_SecurityDecryptionEnable(
    B_PlaybackIpHandle playback_ip, 
    char *initialPayload,
    int *initialPayloadLength)
{
    int rc = 0;
    BDBG_ASSERT(playback_ip);

    /* set the default session settings */
    switch (playback_ip->openSettings.security.securityProtocol) {
        case B_PlaybackIpSecurityProtocol_None:    /* Clear Content */
            BSTD_UNUSED(initialPayloadLength);
            BSTD_UNUSED(initialPayload);
            rc = 0;
            break;
#ifdef B_HAS_SSL
        case B_PlaybackIpSecurityProtocol_Ssl:     /* SSL/TLS protected Content */
        case B_PlaybackIpSecurityProtocol_Aes128:  /* AES128 protected Content */
            BSTD_UNUSED(initialPayloadLength);
            BSTD_UNUSED(initialPayload);
            /* No action needs to be taken as Decryption is already enabled during SSL Session Open */
            rc = 0;
            break;
#endif
#ifdef B_HAS_DTCP_IP
        case B_PlaybackIpSecurityProtocol_DtcpIp:  /* DTCP-IP protected Content */
            rc = B_PlaybackIp_DtcpIpDecryptionEnable(playback_ip->securityHandle, initialPayload, *initialPayloadLength);
            BDBG_MSG(("%s: Enabled DTCP/IP Decryption and passed in intial payload of %d length to security module\n", __FUNCTION__, *initialPayloadLength));
            /* since initial payload is handled over the dtcp security layer, return 0 length to caller */
            *initialPayloadLength = 0;
            break;
#endif
#ifdef B_HAS_RAD_EA
        case B_PlaybackIpSecurityProtocol_RadEa:   /* Rhapsody's RAD-EA protected Content */
            rc = B_PlaybackIp_RadEaDecryptionEnable(playback_ip->securityHandle, initialPayload, *initialPayloadLength);
            BDBG_MSG(("%s: Enabled RAD/EA Decryption and passed in intial payload of %d length to security module\n", __FUNCTION__, *initialPayloadLength));
            /* since initial payload is handled over the security layer, return 0 length to caller */
            *initialPayloadLength = 0;
            break;
#endif
        default: 
            BDBG_ERR(("%s: Security protocol (%d) is not supported\n", __FUNCTION__, playback_ip->openSettings.security.securityProtocol));
            return -1;
    }
    return rc;
}

int 
http_read_response(B_PlaybackIpHandle playback_ip, void *securityHandle, int fd, char **rbufp, int rbuf_size, char **http_hdr, char **http_payload, int *payload_len)
{
    int bytesRead;
    char *tmp_ptr;
    char *rbuf;
#ifdef B_HAS_NETACCEL1
    STRM_SockRecvParams_t sockRecvParams = STRM_SOCK_RECV_PARAMS_HTTP_DEFAULT;
#endif
    int rc = -1;
    int initial_payload_len = 0;
    int chunk_hdr_size;
    B_PlaybackIpHttpMsgFields httpFields;
    int loopCnt = 0;

    BDBG_MSG(("%s: state %d, serverClosed %d\n", __FUNCTION__, playback_ip->playback_state, playback_ip->serverClosed));
    httpFields.parsingResult = B_PlaybackIpHttpHdrParsingResult_eStatusNotSupported;
    rbuf = *rbufp;
    if (!rbuf) {
        BDBG_ERR(("rbuf error"));
        return -1;
    }

read_again:
    memset(rbuf, 0, rbuf_size+1);
    /* coverity[ -tainted_data_argument : arg-1 ] */
    *http_hdr = NULL;
    bytesRead = 0;
    /* read response until end of response header is found or max buffer size exceeded */
    while (rbuf_size <= DLNA_MAX_HTTP_RESP_SIZE && playback_ip->serverClosed == false) {
        if ((playback_ip->playback_state == B_PlaybackIpState_eStopping) || (playback_ip->playback_state == B_PlaybackIpState_eStopped) || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of while loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            return -1;
        }
        if (_http_socket_select(playback_ip, fd, HTTP_SELECT_TIMEOUT)) {
            BDBG_ERR(("%s: select ERROR:%d\n", __FUNCTION__, errno));
            return -1;
        }

        /* Assume sizeof(rbuf) >= rbuf_size  */
        /* coverity[ -tainted_data_argument : arg-2 ] */
        if ((rc = playback_ip->netIo.read(securityHandle, playback_ip, fd, rbuf+bytesRead, rbuf_size - bytesRead)) <= 0) {
            if (errno == EINTR || errno == EAGAIN) {
                if (++loopCnt > 1) {
                    playback_ip->selectTimeout = true;
                    BDBG_ERR(("%s: read ERROR:%d, selectTimeout is set to true\n", __FUNCTION__, errno));
                }
                else {
                    BDBG_MSG(("%s: read ERROR:%d, select timeout, sleeping for 500 msec and retrying once", __FUNCTION__, errno));
                    BKNI_Sleep(500);
                    continue;
                }
            }
            else
                playback_ip->serverClosed = true;
            BDBG_ERR(("%s: read failed, rc %d, read %d, asked %d", __FUNCTION__, rc, bytesRead, rbuf_size));
            return -1;
        }
        bytesRead += rc;
        playback_ip->totalRead += rc;
        if (B_PlaybackIp_UtilsHttpResponseParse(rbuf, bytesRead, &httpFields) == B_ERROR_SUCCESS)
            break;

        playback_ip->statusCode = httpFields.statusCode;
        /* Error in HTTP parsing, check for parsing results */
        switch (httpFields.parsingResult) {
        case B_PlaybackIpHttpHdrParsingResult_eIncompleteHdr:
            BDBG_MSG(("%s: Haven't yet received complete HTTP message header (bytesRead %d), reading more data into buffer of size %d\n", 
                        __FUNCTION__, bytesRead, rbuf_size));
            if (bytesRead == rbuf_size) {
                int newSize;
                BDBG_MSG(("%s: current buffer size (%d) not big enough to read complete HTTP response message, reallocing it by %d bytes to total of %d bytes\n", 
                            __FUNCTION__, rbuf_size, TMP_BUF_SIZE, rbuf_size+TMP_BUF_SIZE));
                newSize = rbuf_size + TMP_BUF_SIZE;
                if (newSize > DLNA_MAX_HTTP_RESP_SIZE) {
                    BDBG_ERR(("%s: ERROR: Did not receive complete HTTP Header Response in max allowed size (max size %d, bytes read %d)\n", __FUNCTION__, newSize, bytesRead));
                    playback_ip->serverClosed = true;
                    return -1;
                }
                if ((rbuf = MY_BKNI_Realloc(rbuf, rbuf_size, newSize)) == NULL) {
                    BDBG_ERR(("%s: Failed to reallocate memory by %d amount, errno = %d\n", __FUNCTION__, newSize, errno));
                    playback_ip->serverClosed = true;
                    return -1;
                }
                rbuf_size = newSize;
                *rbufp = rbuf;
            }
            continue;
        case B_PlaybackIpHttpHdrParsingResult_eStatusBadRequest:
            BDBG_ERR(("%s: ERROR: Client send a bad HTTP request to server (status code %d)!!\n", __FUNCTION__, httpFields.statusCode));
            playback_ip->serverClosed = true;
            return -1;
        case B_PlaybackIpHttpHdrParsingResult_eStatusServerError:
            BDBG_ERR(("%s: ERROR: Server couldn't satisfy a valid HTTP Request (status code %d)!!\n", __FUNCTION__, httpFields.statusCode));
            playback_ip->serverClosed = true;
            return -1;
        case B_PlaybackIpHttpHdrParsingResult_eStatusNotSupported:
            BDBG_ERR(("%s: ERROR: Unsupported HTTP Message status code %d in HTTP Response!!\n", __FUNCTION__, httpFields.statusCode));
            playback_ip->serverClosed = true;
            return -1;
        case B_PlaybackIpHttpHdrParsingResult_eReadNextHdr:
            BDBG_ERR(("%s: Received HTTP 1xx status code (%d), going back to receiving next response\n", __FUNCTION__, httpFields.statusCode));
            goto read_again;
        case B_PlaybackIpHttpHdrParsingResult_eStatusRedirect:
            BDBG_MSG(("%s: Received HTTP redirect (status code %d) in HTTP Response!!\n", __FUNCTION__, httpFields.statusCode));
            playback_ip->serverClosed = false;
            *http_hdr = httpFields.httpHdr;
            *http_payload = httpFields.httpPayload;
            /* caller will take the necessary steps to handle the redirection */
            return 0;
        case B_PlaybackIpHttpHdrParsingResult_eIncorrectHdr:
        default:
            BDBG_ERR(("%s: received Incorrect/Invalid Header\n", __FUNCTION__));
            playback_ip->serverClosed = true;
            return -1;
        }
    }

#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog) {
        char *tmpPtr;
        if ( (tmpPtr = strstr(rbuf, "\n\r\n")) || /* covers both CRNL CRNL & NL NL cases */
             (tmpPtr = strstr(rbuf, "\n\n")) ||   /* covers NL NL case */
             (tmpPtr = strstr(rbuf, "\r\r"))      /* covers CR CR case */
           ) {
            char tmpChar = *tmpPtr;
            *tmpPtr = '\0';
            BDBG_WRN(("HTTP response:"));
            fprintf(stdout, "%s", rbuf);
            fflush(stdout);
            *tmpPtr = tmpChar;
        }
    }
#endif
    if (httpFields.parsingResult != B_PlaybackIpHttpHdrParsingResult_eSuccess)  {
        BDBG_ERR(("%s: received Incorrect/Invalid Reponse Header\n", __FUNCTION__));
        if (!playback_ip->selectTimeout)
            playback_ip->serverClosed = true;
        return -1;
    }

    /* Complete HTTP Message Header is found */
    playback_ip->statusCode = httpFields.statusCode;
    *http_hdr = httpFields.httpHdr;
    *http_payload = httpFields.httpPayload;
    initial_payload_len = httpFields.httpPayloadLength;
    playback_ip->chunkEncoding = httpFields.chunkEncoding;
    if (!playback_ip->contentLength) {
        if (httpFields.contentLength)
            playback_ip->contentLength = httpFields.contentLength;
        else if (playback_ip->setupSettings.u.http.contentLengthHint)
            playback_ip->contentLength = playback_ip->setupSettings.u.http.contentLengthHint;
    }
    BDBG_MSG(("%s: Content Length %lld, initial payload len %d, bytesRead %d\n", __FUNCTION__, playback_ip->contentLength, initial_payload_len, bytesRead));

    /* adjust the total bytes read */
    playback_ip->totalRead -= httpFields.httpPayload - httpFields.httpHdr;
    *payload_len = initial_payload_len;
    
    /* 
    * If server is using HTTP Chunked Transfer-Encoding to send the content,
    * we will need to skip the chunk headers also from the payload pointer.
    */
    if (playback_ip->chunkEncoding) {
        /* if we haven't read any initial payload data or read data doesn't yet contain CRNL (which marks end of chunk size) */
        /* then read another 32 bytes as it must contain the chunk size */
        rbuf[bytesRead] = '\0';
        while (!initial_payload_len || ((tmp_ptr = strstr(*http_payload, "\r\n")) == NULL)) {
            if (bytesRead >= rbuf_size) {
                BDBG_ERR(("%s: ERROR: Initial read of %d bytes is not BIG enough to read chunk size header, increase it from %d", 
                    __FUNCTION__, bytesRead));
                playback_ip->serverClosed = true;
                return -1;
            }
            if (_http_socket_select(playback_ip, fd, HTTP_SELECT_TIMEOUT)) {
                BDBG_ERR(("%s: select ERROR:%d, but continue\n", __FUNCTION__, errno));
                return -1;
            }

            if ((rc = playback_ip->netIo.read(securityHandle, playback_ip, fd, rbuf+bytesRead, rbuf_size - bytesRead)) <= 0) {
                BDBG_ERR(("%s: read failed: rc %d", __FUNCTION__, rc));
                if (errno == EINTR || errno == EAGAIN) {
                    playback_ip->selectTimeout = true;
                    BDBG_ERR(("%s: read ERROR:%d, selectTimeout is set to true\n", __FUNCTION__, errno));
                }
                else
                    playback_ip->serverClosed = true;
                return -1;
            }
            BDBG_MSG(("%s: read %d bytes, bytesRead %d, initial_payload_len %d\n", __FUNCTION__, rc, bytesRead, initial_payload_len));
            playback_ip->totalRead += rc;
            bytesRead += rc;
            initial_payload_len += rc;
            rbuf[bytesRead] = '\0';
        }
        BDBG_MSG(("%s: initial payload length including chunk size header %d\n", __FUNCTION__, initial_payload_len));
        /* 
         * So we have found the valid chunk-size header, now read the chunk size. It is format:
         *  <ascii hex string indicating chunk size> <CR> <NL>
         */
        if (playback_ip->totalRead)
            playback_ip->totalRead -= 2;
        *tmp_ptr = '\0';
        playback_ip->chunkSize = strtol(*http_payload, (char **)NULL, 16);    /* base 16 */
        if (playback_ip->chunkSize == 0) {
            BDBG_MSG(("%s: received 0 chunk size in response, there is no data at the requested range, return EOF\n", __FUNCTION__));
            return 0;
        }

        *tmp_ptr = '\r';
        chunk_hdr_size = (tmp_ptr - *http_payload) + strlen("\r\n");
        initial_payload_len -= chunk_hdr_size;
        *http_payload += chunk_hdr_size;
        playback_ip->totalRead -= chunk_hdr_size;
        playback_ip->chunkEncoding = true;

        /* chunk encoding case: if DTCP encryption is being used, initial payload is now fed to the dtcp module, else it is passed to the caller. */
        /* Since for some security protocols, initial HTTP response can be in clear, now indicate to security layer */
        /* that HTTP header processing is over and it can now start decrypting the incoming data */
        /* Note: payload_len becomes 0 for DTCP as this data is handled over the dtcp_ip layer for later decryption during 1st read call */
        if ((rc = B_PlaybackIp_SecurityDecryptionEnable(playback_ip, *http_payload, &initial_payload_len)) != 0) {
            BDBG_ERR(("%s: Failed (rc %d) for Enabled Decryption for security protocol %d:", __FUNCTION__, rc, playback_ip->openSettings.security.securityProtocol));
            playback_ip->serverClosed = true;
            return -1;
        }
        
        if (initial_payload_len != 0) {
            /* Security layer didn't consume the data: either security module provides clear data only (complete transport encryption including chhunk headers) or its clear data */
            /* Since chunk xfer encoding is being used, we process initial data part of the chunk header processing, so copy this data and set appropriate state */
            memcpy(playback_ip->temp_buf, *http_payload, initial_payload_len);
            playback_ip->chunkPayloadLength = initial_payload_len;
            playback_ip->chunkPlayloadStartPtr = playback_ip->temp_buf;
        }
        else {
            /* initial_payload_len is made 0, so either no initial data was read or was handled over to the security module for later decryption during next read call */
            playback_ip->chunkPayloadLength = 0;
        }
        /* now set the bytes left in the chunk: we start w/ the current chunk size */
        playback_ip->bytesLeftInCurChunk = playback_ip->chunkSize;

        /* return 0 payload_len as for chunk encoding all initial data is either handled over to the security layer or handled via the chunkPlayloadStartPtr */
        *payload_len = 0;

        BDBG_MSG(("%s: chunk size %lld, chunk header size %d, initial payload size %d, bytesLeftInCurChunk %d\n", 
                    __FUNCTION__, playback_ip->chunkSize, chunk_hdr_size, playback_ip->chunkPayloadLength, playback_ip->bytesLeftInCurChunk));
    }
    else {
        /* non-chunk encoding case: if DTCP encryption is being used, initial payload is now fed to the dtcp module, else it is passed to the caller. */
        /* Since for some security protocols, initial HTTP response can be in clear, now indicate to security layer */
        /* that HTTP header processing is over and it can now start decrypting the incoming data */
        /* Note: payload_len becomes 0 for DTCP as this data is handled over the dtcp_ip layer for later decryption during 1st read call */
        if ((rc = B_PlaybackIp_SecurityDecryptionEnable(playback_ip, *http_payload, payload_len)) != 0) {
            BDBG_ERR(("%s: Failed (rc %d) for Enabled Decryption for security protocol %d:", __FUNCTION__, rc, playback_ip->openSettings.security.securityProtocol));
            playback_ip->serverClosed = true;
            return -1;
        }
    }

#ifdef B_HAS_NETACCEL1
    /* disabling using NETACCEL for HTTP as there aren't real gains + it is not working w/ 2.6.37 */
    /* now that we have read the HTTP response, increase the # of pkts per read */
    memset((char *)&sockRecvParams, 0, sizeof(sockRecvParams));
    sockRecvParams.pktsPerRecv = PKTS_PER_READ;
    sockRecvParams.pktOffset = 0;
    sockRecvParams.hdrOffset = 0;
    sockRecvParams.recvTimeout = 100;
    sockRecvParams.useCpuCopy = true;
    if (setsockopt(fd, SOCK_BRCM_STREAM, STRM_SOCK_RECV_PARAMS, &sockRecvParams, sizeof(sockRecvParams)) != 0)
    {
        BDBG_ERR(("%s: setsockopt() ERROR:", __FUNCTION__));
        /* in case of failure (shouldn't happen), read 1 pkt at a time */
    }
    BDBG_MSG(("Modified the default pkts per recvfrom to %d\n", PKTS_PER_READ));
#else 
    B_PlaybackIp_UtilsTuneNetworkStack(fd);
#endif
    return (0);
}

static void 
reset_playback_ip_state(B_PlaybackIpHandle playback_ip) 
{
    playback_ip->chunkEncoding = false;
    playback_ip->serverClosed = false;
    playback_ip->reOpenSocket = false;
    playback_ip->socketClosed = false;
    playback_ip->chunkSize = 0;
    playback_ip->bytesLeftInCurChunk = 0;
    playback_ip->totalRead = 0;
    playback_ip->totalConsumed = 0;
    playback_ip->chunkPayloadLength = 0;
    playback_ip->rewind = false;
    playback_ip->statusCode = 0;
    playback_ip->tempBytesRead = 0;
    playback_ip->tempBytesToRead = 0;
    memset(playback_ip->temp_buf, 0, TMP_BUF_SIZE);
    playback_ip->num_chunks_received = 0;
}

/* 
 * This is the high level function to read any data from a socket of a HTTP session. It is called by multiple upper
 * layer modules such as HTTP caching logic (which is used when data is being fed thru the nexus playback), direct 
 * nexus playpump based feeder (which is used when server side trickmodes are being used), HLS & DASH Players, etc.
 *
 * It tries to read data requested amount of data from a network socket into the caller specified buffer. 
 * It takes care of HTTP Chunk Transfer Encoding protocols where it removes the Chunk headers & trailers from the AV
 * stream (which are inter-mixed in the stream). In addition, it can also take care of decryption of data if it is 
 * part of encrypted sessions for the support security protocols (DTCP, HLS-AES, SSL).
 *
 * Return values:
 *   >  0: # of bytes upto buf_size 
 *   == 0: server has closed the connection 
 *   <  0: network error (caller needs to use the selectTimeout flag to check if the failure is due to temporary socket timeout)
 *
 * Note: function sets following two playback_ip state that callers need to be aware of: serverClosed & selectTimeout
 *   serverClosed: when server has closed the connection (either has sent the whole file, reached EOF, and thus closed the socket)
 *   selectTimeout: when there is no data to read on the socket for networkTimeout duration (caller may retry)
 *
 */
int 
playback_ip_read_socket(
    B_PlaybackIpHandle playback_ip, 
    void *securityHandle,
    int fd, 
    char *buf, 
    int buf_size,
    int timeout      /* indicates the timeout in seconds for network events */
    )
{
    int bytesRead = 0;
    int totalBytesRead = 0;
    int bytesToRead = 0;
    char *chunk_hdr_ptr = NULL;
    int chunk_hdr_size;
    char *tmp_ptr;

    PRINT_SOCKET_MSG(("%s: read %d bytes", __FUNCTION__, buf_size));
    playback_ip->selectTimeout = false;
    if (!playback_ip->chunkEncoding) {
        /* server is sending content w/o using HTTP xfer chunk encoding */
        bytesToRead = buf_size;
        if (http_socket_read(playback_ip, securityHandle, fd, buf, bytesToRead, &bytesRead, timeout) < 0) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: ERROR:%d, state %d, select timeout %d, server closed %d", __FUNCTION__, errno, playback_ip->playback_state, playback_ip->selectTimeout, playback_ip->serverClosed));
#endif
            return -1;
        }
        if (bytesRead != bytesToRead) {
            if  ((playback_ip->playback_state == B_PlaybackIpState_eStopping) || (playback_ip->playback_state == B_PlaybackIpState_eStopped)) {
                /* can happen in some cases where http_socket_read() takes long time to timeout and app has already decided to close the playback channel */
                return -1;
            }
            /* otherwise, we should consume this data even though we didn't read all that we had wanted */
            BDBG_MSG(("%s:%d: didn't read (%d) the requested amount (%d) from socket, server closed %d", __FUNCTION__, __LINE__, bytesRead, bytesToRead, playback_ip->serverClosed));
        }
        return bytesRead;
    }

    /* 
     * Server is sending content using HTTP xfer chunk encoding. Chunk headers are interspersed with data. 
     * Format is: 
     *  <HTTP Resp Hdr>
     *  <chunk size string>CRNL
     *  <chunk data>CRNL
     *  <chunk size string>CRNL
     *  <chunk data>CRNL
     *  ...
     * 
     *  <0>CRNL #indicates end of chunk headers
     * Our approach is like this: Read from socket until current chunk is completely read. 
     * Read the next chunk header and then the content again. 
     * Loop until all requested bytes are read or n/w error happens.
     */
    while (totalBytesRead < buf_size && playback_ip->serverClosed == false) {
        if ((playback_ip->playback_state == B_PlaybackIpState_eStopping) || (playback_ip->playback_state == B_PlaybackIpState_eStopped) || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of while loop due to state (%d) change, totalBytesRead %d\n", __FUNCTION__, playback_ip->playback_state, totalBytesRead));
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
                playback_ip->selectTimeout = true;
                errno = EINTR;
            }
            else {
                playback_ip->serverClosed = true;
            }
            return totalBytesRead;
        }
        if (playback_ip->bytesLeftInCurChunk) {
            /* 
            * There are still bytes left in the current chunk, so read that much data first.
            * This data can either be pre-read in the temp buf or needs to be read from socket.
            */
            bytesToRead = buf_size - totalBytesRead;
            if (playback_ip->chunkPayloadLength) {
                /* copy lower of remaining bytes or what was read in the temp buffer to caller's buffer */
                bytesToRead = (bytesToRead <= playback_ip->chunkPayloadLength) ? bytesToRead : playback_ip->chunkPayloadLength;
                if (bytesToRead > playback_ip->bytesLeftInCurChunk) {
                    BDBG_MSG(("bytesToRead %d > bytesLeftInChunk %lld, chunkPayloadLength %d",
                            bytesToRead, playback_ip->bytesLeftInCurChunk, playback_ip->chunkPayloadLength));
                    bytesToRead = playback_ip->bytesLeftInCurChunk;
                }
                /* remaining bytes are available in the temp buffer, copy them and return */
                BDBG_MSG(("%s: copying %d bytes from temp buffer, bytes remaining in current chunk %lld\n", 
                            __FUNCTION__, bytesToRead, playback_ip->bytesLeftInCurChunk));
                memcpy(buf, playback_ip->chunkPlayloadStartPtr, bytesToRead);
                bytesRead = bytesToRead;
                playback_ip->chunkPlayloadStartPtr += bytesRead;
                playback_ip->chunkPayloadLength -= bytesRead;
                if (playback_ip->chunkPayloadLength < 0) {
                    BDBG_ERR(("%s: ERROR: SW bug: remaining bytes (%d) in temp buf can't be < 0, copied %d\n", 
                                __FUNCTION__, playback_ip->chunkPayloadLength, bytesRead));
                    return -1;
                }
            }
            else {
                /* no bytes left in the temp buffer, so read from the socket */
                /* determine how much to read: read lower of the remaining data in current request or remaining data in current chunk */
                if (bytesToRead > playback_ip->bytesLeftInCurChunk) {
                    bytesToRead = playback_ip->bytesLeftInCurChunk;
                    /* since bytesToRead are now based on the bytes left in the current chunk, set a flag to indicate so */
                    /* as we may need to adjust bytesToRead if PCP header is found for DTCP */
                    BDBG_MSG(("%s: set flag to indicate that we may need to adjust bytesToRead if PCP header is found for DTCP", __FUNCTION__));
                    playback_ip->adjustBytesToRead = true;
                }
                else {
                    playback_ip->adjustBytesToRead = false;
                }

                BDBG_MSG(("%s: reading %d bytes from socket, total read so far %d, total asked %d, bytesLeftInCurChunk %lld", 
                            __FUNCTION__, bytesToRead, totalBytesRead, buf_size, playback_ip->bytesLeftInCurChunk));
                if (http_socket_read(playback_ip, securityHandle, fd, buf, bytesToRead, &bytesRead, timeout) < 0) {
                    BDBG_ERR(("%s: ERROR:%d, bytes left in chunk %lld\n", __FUNCTION__, errno, playback_ip->bytesLeftInCurChunk));
                    return -1;
                }
                if (bytesRead != bytesToRead && playback_ip->serverClosed == false) {
                    BDBG_MSG(("%s:%d didn't read (%d) the requested amount (%d) from socket, select timeout %d, bytes left in chunk %lld, chunk size %lld\n", 
                                __FUNCTION__, __LINE__, bytesRead, bytesToRead, playback_ip->selectTimeout, playback_ip->bytesLeftInCurChunk, playback_ip->chunkSize));
                }
            }
            totalBytesRead += bytesRead;
            buf += bytesRead;
            playback_ip->bytesLeftInCurChunk -= bytesRead;
            if (playback_ip->selectTimeout) {
                BDBG_MSG(("%s:%d select timeout(%d): returning read (%d), the requested amount (%d), bytes left in chunk %lld\n", 
                           __FUNCTION__, __LINE__, playback_ip->selectTimeout, totalBytesRead, bytesToRead, playback_ip->bytesLeftInCurChunk));
                return totalBytesRead;
            }
            if (playback_ip->bytesLeftInCurChunk < 0) {
                BDBG_ERR(("%s:%d ERROR: SW bug: bytes left in a chunk (%d) can't be < 0, just read %d, totalRead %d, total asked %d\n", 
                            __FUNCTION__, __LINE__, playback_ip->bytesLeftInCurChunk, bytesRead, totalBytesRead, buf_size));
                return -1;
            }
            BDBG_MSG(("%s: after update: chunk size %lld, bytes left in chunk %lld, bytes read %d, to read %d, totalBytesRead %d", __FUNCTION__, playback_ip->chunkSize, playback_ip->bytesLeftInCurChunk, bytesRead, bytesToRead, totalBytesRead));
            continue;
        }
        else {
            /* bytesLeftInCurChunk == 0 case */
            /* 
            * So we finished reading the current chunk. Now read the next chunk header. 
            * The extra data read while reading the chunk header is stored in the temp_buf.
            */
            if (playback_ip->chunkPayloadLength > 0) {
                /* there is still some data left in the temporary buffer when previous chunk header was read into */
                /* so skip the network read and instead move these bytes into the begining of the buffer */
                memmove(playback_ip->temp_buf, playback_ip->chunkPlayloadStartPtr, playback_ip->chunkPayloadLength);
                bytesRead = playback_ip->chunkPayloadLength;
                /* clear the remaining bytes of the tempBuf */
                memset(playback_ip->temp_buf+bytesRead, 0, TMP_BUF_SIZE-bytesRead);
                playback_ip->tempBytesToRead = TMP_BUF_SIZE; /* start w/ total tmp buf size and it gets adjusted by bytesRead below after the else case */
                playback_ip->chunkPayloadLength = 0;
                BDBG_MSG(("%s: Finished reading current data chunk, next chunk header may already be previously read (read so far %d, total %d), # of chunk rcvd %d\n", __FUNCTION__, bytesRead, totalBytesRead, playback_ip->num_chunks_received));
                playback_ip->num_chunks_received++;
            }
            else {
                BDBG_MSG(("%s: Finished reading current data chunk, now read next chunk header (total read so far %d), # of chunk rcvd %d\n", __FUNCTION__, totalBytesRead, playback_ip->num_chunks_received));
                if (playback_ip->tempBytesRead == 0) {
                    /* we haven't read anything so far for this new chunk, so try to read enough to process a complete chunk header */
                    playback_ip->tempBytesToRead = TMP_BUF_SIZE;
                    memset(playback_ip->temp_buf, 0, TMP_BUF_SIZE);
                    playback_ip->num_chunks_received++;
                }
                else {
                    /* we have read tempBytesRead so far for this new chunk, so try to read enough to process a complete chunk header */
                    memset(playback_ip->temp_buf+playback_ip->tempBytesRead, 0, TMP_BUF_SIZE-playback_ip->tempBytesRead);
                    playback_ip->num_chunks_received++;
                    BDBG_MSG(("tempBytesRead %d, tempBytesToRead %d", playback_ip->tempBytesRead, playback_ip->tempBytesToRead));
                }
            
                /* Since some security protocols like DTCP/IP only encrypt the AV content, we will need to disable the decryption */
                /* so that we can successfully read the current chunk header terminators & next chunk headers (which are both in clear) */
                if (B_PlaybackIp_SecurityDecryptionDisable(playback_ip) != 0) {
                    BDBG_ERR(("%s: Failed to Disable the Decryption during next chunk xfer header processing", __FUNCTION__));
                    return -1;
                }

                /* Note: we are reading into a temp buffer as we dont want to pass in the chunk header fields (control data) back to the playback */
                if (http_socket_read(playback_ip, securityHandle, fd, playback_ip->temp_buf+playback_ip->tempBytesRead, playback_ip->tempBytesToRead, &bytesRead, timeout) <= 0) {
                    BDBG_ERR(("%s: read ERROR:%d", __FUNCTION__, errno));
                    return -1;
                }
            }
            if (bytesRead + playback_ip->tempBytesRead < 5) {
                /* unless we read minimum of 5 bytes, we shouldn't do any chunk header processing, so save these bytes and read more (this can most likely happen due to network congestion shown by select timeout) */
                BDBG_MSG(("%s:%d : didn't read (%d) the requested amount (%d) from socket, select timeout %d\n", __FUNCTION__, __LINE__, bytesRead, playback_ip->tempBytesToRead, playback_ip->selectTimeout));
                playback_ip->tempBytesToRead -= bytesRead;
                playback_ip->tempBytesRead += bytesRead;
                playback_ip->selectTimeout = true;
                /* we return here as we expect to read the remaining bytesToRead whenever the next read call may succeed */
                if (totalBytesRead > 0)
                    return totalBytesRead;
                else
                    return -1;
            }
            else {
                /* we have read atleast 5 bytes */
                /* First, make sure the end of chunk marker (CRNL) for previous chunk are present  as each chunk data ends w/ CRNL */
                if (playback_ip->temp_buf[0] != '\r' || playback_ip->temp_buf[1] != '\n') {
                    BDBG_ERR(("%s: ERROR in HTTP chunk header processing: server didn't send End Markers (CR NL) of current data chunk, instead we got: 0x%x, 0x%x\n", 
                        __FUNCTION__, playback_ip->temp_buf[0], playback_ip->temp_buf[1]));
                    return -1;
                }
                /* now check if next chunk header is completely read, meaning we have read a string w/ some hex digits (atleast one 0) followed by CRNL markers */
                if (strstr(&playback_ip->temp_buf[2], "\r\n") == 0) {
                    BDBG_MSG(("%s: End Markers for next Chunk Header are not yet read, go back & read more (bytesRead %d)\n", __FUNCTION__, bytesRead));
                    playback_ip->tempBytesToRead -= bytesRead;
                    playback_ip->tempBytesRead += bytesRead;
                    if (playback_ip->tempBytesRead == TMP_BUF_SIZE) {
                        /* if we haven't found the end of chunk markers in TMP_BUF_SIZE bytes, there is an error in chunk processing, bail out */
                        BDBG_ERR(("%s: ERROR: End Markers (CRNL) for next Chunk Header are not found in %d bytes\n", __FUNCTION__, playback_ip->tempBytesRead));
                        return -1;
                    }
                    /* we return here as we expect to read the remaining bytesToRead whenever the next read call may succeed */
                    playback_ip->selectTimeout = true;
                    if (totalBytesRead > 0)
                        return totalBytesRead;
                    else
                        return -1;
                }
                BDBG_MSG(("%s: Chunk Header Start End Markers are read, so process the chunk header (bytes read %d, temp read %d)\n", __FUNCTION__, bytesRead, playback_ip->tempBytesRead));

                /* adjust total bytes read */
                playback_ip->tempBytesRead += bytesRead;

                /* at this point we have gotten end markers for the previous chunk & next chunk header as well, so do the math to account for them */
                playback_ip->chunkPayloadLength = playback_ip->tempBytesRead - 2; /* 2 bytes for CRNL of previous chunk end */
                chunk_hdr_ptr = &playback_ip->temp_buf[2];
                if (playback_ip->totalRead)
                    playback_ip->totalRead -= 2;
                playback_ip->tempBytesRead = 0; /* resets temp bytes read  for processing of next chunk header */

                /* now get the size of next chunk, format is <hex string of chunk size>CRNL  */
                tmp_ptr = strstr(chunk_hdr_ptr, "\r\n");
                /* tmp_ptr can't be NULL as we have already verified above that CRNL are in the read buffer */
                BDBG_ASSERT(tmp_ptr);
                *tmp_ptr = '\0';
                playback_ip->chunkSize = strtol(chunk_hdr_ptr, (char **)NULL, 16); /* base 16 */
                *tmp_ptr = '\r';
                chunk_hdr_size = (tmp_ptr - chunk_hdr_ptr) + strlen("\r\n");
                playback_ip->chunkPayloadLength -= chunk_hdr_size;
                playback_ip->chunkPlayloadStartPtr = chunk_hdr_ptr + chunk_hdr_size;
                playback_ip->totalRead -= chunk_hdr_size;
                if (playback_ip->chunkSize == 0) {
                    playback_ip->serverClosed = true;
                    playback_ip->selectTimeout = false;
                    BDBG_MSG(("%s: received chunk size of 0, received all data (%lld) from server, setting serverClosed flag\n", __FUNCTION__, playback_ip->totalRead));
                    /* NOTE: we cannot set the contentLength to totalRead here as for trick modes or ASF playback case, we may be getting EOF even though we haven't read whole file */
                }
                else {
                    /* now re-enable the decryption */
                    if (B_PlaybackIp_SecurityDecryptionEnable(playback_ip, playback_ip->chunkPlayloadStartPtr, &playback_ip->chunkPayloadLength) != 0) {
                        BDBG_ERR(("%s: Failed for Enabled Decryption for security protocol %d:", __FUNCTION__, playback_ip->openSettings.security.securityProtocol));
                        return -1;
                    }
                    playback_ip->bytesLeftInCurChunk = playback_ip->chunkSize; /* start of new chunk */
                    BDBG_MSG(("%s: read new chunk: chunk size %lld, chunk header size %d, initial payload size %d, bytesLeftInCurChunk %d\n", 
                        __FUNCTION__, playback_ip->chunkSize, chunk_hdr_size, playback_ip->chunkPayloadLength, playback_ip->bytesLeftInCurChunk));
                }
            }
        }
    }
    if (totalBytesRead > buf_size) {
        BDBG_ERR(("%s: ERROR: SW bug: read bytes (%d) are more than total requested (%d), bytes remaining in current chunk %d",
                __FUNCTION__, totalBytesRead, buf_size, playback_ip->bytesLeftInCurChunk));
    }
    BDBG_MSG(("%s: bytes read %d, asked %d, remaining in cur chunk %d, total read %lld\n", 
            __FUNCTION__, totalBytesRead, buf_size, playback_ip->bytesLeftInCurChunk, playback_ip->totalRead));
    return totalBytesRead;
}

B_PlaybackIpError 
http_do_server_trick_modes_socket_transition(
    B_PlaybackIpHandle playback_ip, 
    double timeSeekRangeBegin,
    double timeSeekRangeEnd,
    int rate
    )
{
    B_PlaybackIpError rc = B_ERROR_SUCCESS;
    void *securityHandle;
    char *http_hdr = NULL;
    char *http_payload = NULL;
    int http_payload_length;

#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: rate %d, timeSeekRangeBegin %0.3f, timeSeekRangeEnd %0.3f, state %d", __FUNCTION__, rate, timeSeekRangeBegin, timeSeekRangeEnd, playback_ip->playback_state));
#endif
    /* 
     * Here are the high level steps:
     * -prepare & send new HTTP request for server w/ new rate (speed) and time position
     * -process the HTTP response
     */

    /* shutdown the current TCP connection, this way we dont need to worry about having any stale data from previous speed */
    if (playback_ip->netIo.shutdown)
        playback_ip->netIo.shutdown(playback_ip->securityHandle, 0);
    close(playback_ip->socketState.fd);

    /* prepare the HTTP Get Request with new speed & time seek range */
    memset(playback_ip->responseMessage, 0, TMP_BUF_SIZE+1);

    /* note: range parameters are set to 0 as they are un-unsed in server based trick modes */
    if (http_build_get_req(&playback_ip->requestMessage, playback_ip, rate, timeSeekRangeBegin, timeSeekRangeEnd, 0, 0) < 0) {
        BDBG_ERR(("%s: ERROR: failed to build HTTP Get Request", __FUNCTION__));
        goto error;
    }
    
    /* now open the new socket connection to the server */
    rc = B_PlaybackIp_UtilsTcpSocketConnect(&playback_ip->playback_state, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, true, &playback_ip->socketState.fd);
    if (rc != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s: ERROR: failed to send Socket Connect Request to Server: %s:%d\n", 
               __FUNCTION__, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port));
        goto error;
    }

    /* SECURITY: clone the new security session using the current one */
    /* Idea is to avoid any re-authentication to the server and instead reuse the session info from the current session */
    securityHandle = playback_ip->securityHandle;
    if (B_PlaybackIp_SecurityCloneSessionOpen(playback_ip, playback_ip->socketState.fd, securityHandle, &playback_ip->securityHandle) < 0) {
        rc = B_ERROR_UNKNOWN;
        BDBG_ERR(("%s: ERROR: failed to clone the security session, rc %d\n", __FUNCTION__, rc));
        goto error;
    }
    /* free up the older security session */
    playback_ip->netIo.close(securityHandle);

    /* now send the GET request */
    if (playback_ip->netIo.write(playback_ip->securityHandle, &playback_ip->playback_state, playback_ip->socketState.fd, playback_ip->requestMessage, strlen(playback_ip->requestMessage)) < 0) {
        rc = B_ERROR_UNKNOWN;
        BDBG_ERR(("%s: ERROR: failed to send HTTP Get request to Server: %s:%d\n", __FUNCTION__, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port));
        goto error;
    }
    BDBG_MSG(("%s: HTTP Get Request--->: \n%s", __FUNCTION__, playback_ip->requestMessage));
    BKNI_Free(playback_ip->requestMessage);
    playback_ip->requestMessage = NULL;

    /* reset the playback ip state as we starting from a new point in stream */
    reset_playback_ip_state(playback_ip);

    /* now read the http response */
    /* RVU TODO: may need to parse the FrameMap in the RVU response for exact frame time and use it for next time seek range */
    if (http_read_response(playback_ip, playback_ip->securityHandle, playback_ip->socketState.fd, &playback_ip->responseMessage, TMP_BUF_SIZE, &http_hdr, &http_payload, &http_payload_length) < 0 ) {
        rc = B_ERROR_UNKNOWN;
        goto error;
    }
    if (playback_ip->openSettings.u.http.rvuCompliant) {
        /* check for end of stream header from the server */
        if (strstr(playback_ip->responseMessage, "eom-indicator.rvualliance.org:")) {
            BDBG_MSG(("%s: Got End of Media Indicator from RVU Server, closing session", __FUNCTION__));
            playback_ip->serverClosed = true;
            rc = B_ERROR_UNKNOWN;
            goto error;
        }
    }
    if (!playback_ip->useNexusPlaypump) {
        /* re-initialze the cache for pull mode */
        memcpy(playback_ip->dataCache[0].cache, http_payload, http_payload_length);
        playback_ip->dataCache[0].startOffset = 0;
        playback_ip->dataCache[0].readIndex = 0;
        playback_ip->dataCache[0].endOffset = playback_ip->dataCache[0].startOffset - 1;
        playback_ip->dataCache[0].depth = http_payload_length;
        playback_ip->dataCache[0].writeIndex = http_payload_length;
        playback_ip->dataCache[0].inUse = true;
    }
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        /* TODO: is http_payload_length amount of data getting lost? should it be in the playback_ip->initial_data_len? Look into this */
        BDBG_WRN(("%s: done, initial bytes read %d\n", __FUNCTION__, http_payload_length));
#endif
    rc = B_ERROR_SUCCESS;
error:
    if (playback_ip->requestMessage) {
        BKNI_Free(playback_ip->requestMessage);
        playback_ip->requestMessage = NULL;
    }
    return rc;
}

/* This code runs in following 3 cases: normal rewind, separate connection for index when it is at end (e.g. for some MP4 & ASF) */
/* and 3rd case is when we seek to a way different offset where it doesn't make sense to keep receiving data from current location */
ssize_t
B_PlaybackIp_HttpNetRangeReq(
    B_PlaybackIpHandle playback_ip, 
    void *buf, 
    size_t length, 
    off_t byteRangeStart, 
    off_t byteRangeEnd,
    int prevFd,
    int *newFd
    )
{
    unsigned int bytesRead = 0;
    ssize_t rc = -1;
    char *http_hdr = NULL;
    char *http_payload = NULL;
    int initialLen = 0;
    void *securityHandle = playback_ip->securityHandle;
    void **newSecurityHandle = &playback_ip->securityHandle;
    char *requestMessage, *responseMessage;
    int loopCnt = 0;
    http_url_type_t http_url_type;

#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: read %d bytes of content data at range start %llu end %lld, prevFd %d, fd %d, securityHandle %p", __FUNCTION__, length, byteRangeStart, byteRangeEnd, prevFd, playback_ip->socketState.fd, securityHandle));
#endif

    if (byteRangeEnd) {
        length = byteRangeEnd - byteRangeStart + 1;
    }

    /* prepare initial Get request */
    *newSecurityHandle = NULL;
    responseMessage = (char *) BKNI_Malloc(TMP_BUF_SIZE+1);
    if (!responseMessage) {
        BDBG_ERR(("%s: ERROR: failed to allocate memory\n", __FUNCTION__));
        goto error;
    }
    for (;;) {
        memset(responseMessage, 0, TMP_BUF_SIZE+1);

        if (!playback_ip->socketClosed && prevFd) {
            /* if security layer has defined the shutdown callback, we call it to start the shutdown of the security handle */
            /* this basically tells security module to free up an security context w/o destroying the ability to clone it */
            if (playback_ip->netIo.shutdown && !playback_ip->serverClosed)
                playback_ip->netIo.shutdown(securityHandle, 0);
            if (close(prevFd)) {
                BDBG_MSG(("%s: failed to close prev socket (%d), ignore it\n", __FUNCTION__, prevFd));
                perror("close");
            }
            else
                BDBG_MSG(("%s: closed prev socket (fd %d)", __FUNCTION__, prevFd));
            /* reset the socketClosed flag */
            playback_ip->socketClosed = false;
        }

        if (http_build_get_req(&requestMessage, playback_ip, 1, 0., 0., byteRangeStart, byteRangeEnd) < 0) {
            BDBG_ERR(("%s: ERROR: failed to build HTTP Get request to Server", __FUNCTION__));
            goto error;
        }

        /* setup the socket connection to the server & send GET request */
        rc = B_PlaybackIp_UtilsTcpSocketConnect(&playback_ip->playback_state, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, true, newFd);
        if (rc != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to send Socket Connect Request to Server: %s:%d\n", 
                        __FUNCTION__, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port));
            rc = -1;
            goto error;
        }
        prevFd = *newFd; /* incase we get a redirect, so we will need to close this newly opened socket */

        /* SECURITY: we need to clone the new security session using the current one */
        /* Idea is to avoid any re-authentication to the server and instead reuse the session info from the current session */
        rc = B_PlaybackIp_SecurityCloneSessionOpen(playback_ip, *newFd, securityHandle, newSecurityHandle);
        if (rc < 0) {
            BDBG_ERR(("%s: ERROR: failed to clone the security session, rc %d\n", __FUNCTION__, rc));
            goto error;
        }

        if (playback_ip->netIo.close) {
            /* free up the current security session */
            playback_ip->netIo.close(securityHandle);
            BDBG_MSG(("%s: freed up the previous (%p) security session, new session %p\n", __FUNCTION__, securityHandle, *newSecurityHandle));
        }
        securityHandle = *newSecurityHandle;

        /* now send the GET request */
        rc = playback_ip->netIo.write(securityHandle, &playback_ip->playback_state, *newFd, requestMessage, strlen(requestMessage));
        if (rc < 0) {
            BDBG_ERR(("%s: ERROR: failed to send HTTP Get request to Server: %s:%d\n", __FUNCTION__, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port));
            goto error;
        }
        BDBG_MSG(("%s: Sent HTTP Get Request (fd %d)", __FUNCTION__, *newFd));
        BKNI_Free(requestMessage);
        requestMessage = NULL;

        reset_playback_ip_state(playback_ip);

        /* now read the http response */
        if ((rc = http_read_response(playback_ip, securityHandle, *newFd, &responseMessage, TMP_BUF_SIZE, &http_hdr, &http_payload, &initialLen)) < 0 ) {
            goto error;
        }

        http_url_type = http_get_url_type(http_hdr, playback_ip->openSettings.socketOpenSettings.url);
        BDBG_MSG(("http url type %d\n", http_url_type));
        if (http_url_type == HTTP_URL_IS_REDIRECT) {
            /* parse HTTP redirect and extract the new URL & Server */
            /* free up the previous cookie */
            if (playback_ip->cookieFoundViaHttpRedirect)
                BKNI_Free(playback_ip->cookieFoundViaHttpRedirect);
            playback_ip->cookieFoundViaHttpRedirect = NULL;
            if (http_parse_redirect(playback_ip->openSettings.socketOpenSettings.ipAddr, &playback_ip->openSettings.socketOpenSettings.port, &playback_ip->openSettings.socketOpenSettings.protocol, &playback_ip->openSettings.socketOpenSettings.url, &playback_ip->cookieFoundViaHttpRedirect, http_hdr) != 0) {
                BDBG_ERR(("Incorrect HTTP Redirect response or parsing error\n'"));
                goto error;
            }
            /* previous function gets the new URL & server information and we send another GET request to this server */
            playback_ip->serverClosed = false; /* clear this flag as server anyway closes socket during redirect and we want to call netIo.shutdown above */
            BDBG_MSG(("%s: HTTP redirect case, (resetting ranges) caching the redirected URL: http://%s:%d%s", __FUNCTION__, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, playback_ip->openSettings.socketOpenSettings.url));
            byteRangeEnd = byteRangeStart = 0;
        }
        else {
            /* actual content URL, get the content attributes from HTTP response header */
            BDBG_MSG(("GOT ACTUAL CONTENT\n"));
            break;
        }

        if (loopCnt++ >5) {
            BDBG_ERR(("%s: ERROR: Can't resolve a URL Link in 5 attempts", __FUNCTION__));
            goto error;
        }
    }

    /* copy any payload bytes into a temp buffer */
    if (initialLen) {
        /* initial data was read part of the HTTP response */
        memcpy(buf, http_payload, initialLen);
        bytesRead = initialLen;
        BDBG_MSG(("%s: COPYING initial data of %d bytes\n", __FUNCTION__, initialLen));
    }

    if (length && length-bytesRead > 0) {
        BDBG_MSG(("%s: remaining %d, bytesRead %d, length %d, initial len %d\n", __FUNCTION__, (length-bytesRead), bytesRead, length, initialLen));
        /* read next chunk of data from socket */
        if ((rc = playback_ip_read_socket(playback_ip, securityHandle, *newFd, ((char *)buf)+bytesRead, length-bytesRead, playback_ip->networkTimeout)) <= 0 ) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: Network Read Error: serverClosed %d, selectTimeout %d", __FUNCTION__, playback_ip->serverClosed, playback_ip->selectTimeout));
#endif
            if (playback_ip->serverClosed || playback_ip->selectTimeout)
                rc = bytesRead; /* hard coding to allow player to consume all data before closing */
            goto error;
        }
    }
    bytesRead += rc;

    BDBG_MSG(("%s: buf %p, asked %d, read %d, start %llu, end %llu\n",
                __FUNCTION__, buf, length, bytesRead, byteRangeStart, byteRangeEnd));
    rc = bytesRead;

error:
    if (responseMessage)
        BKNI_Free(responseMessage);
    if (requestMessage)
        BKNI_Free(requestMessage);
    return rc;
}

int http_read_data_from_index_cache(
    B_PlaybackIpHandle playback_ip, 
    off_t byteRangeStart, 
    off_t byteRangeEnd, 
    char *buf,
    size_t length
    )
{
    unsigned int readIndexOffset = 0;
    BDBG_MSG(("%s: Cache hit: range start %llu, end %llu, i cache depth %d, off: start %lld, end %lld, idx %d, length %u\n", 
            __FUNCTION__, byteRangeStart, byteRangeEnd, playback_ip->indexCacheDepth, playback_ip->indexCacheStartOffset, playback_ip->indexCacheEndOffset, playback_ip->indexCacheReadIndex, length));
    readIndexOffset = playback_ip->indexCacheReadIndex + (byteRangeStart - playback_ip->indexCacheStartOffset);
    readIndexOffset %= playback_ip->indexCacheSize;

    /* look for wrap around */
    if (readIndexOffset + length <= playback_ip->indexCacheSize) {
        /* all index data is sequentially available in the buffer w/o any wrap around, copy it */
        BDBG_MSG(("%s: single copy from index cache offset %d, bytes copied %d\n", __FUNCTION__, readIndexOffset, length));
        memcpy(buf, playback_ip->indexCache+readIndexOffset, length);
        return length;
    }
    else {
        /* initial part of the data is at the end of cache and other is at the begining (due to cache wrap around) of cache */
        int bytesAtCacheEnd;
        int bytesRemaining;
        /* wrap around case, first copy data that is available at the end of the buffer */
        bytesAtCacheEnd = playback_ip->indexCacheSize - readIndexOffset;
        bytesRemaining = length - bytesAtCacheEnd;
        /* first copy of bytes at the end of the cache */
        memcpy(buf, playback_ip->indexCache+readIndexOffset, bytesAtCacheEnd);

        /* and copy remaining bytes from the begining of the cache */
        memcpy((char *)buf+bytesAtCacheEnd, playback_ip->indexCache, bytesRemaining);
        BDBG_MSG(("%s: Two copies: first of %d bytes from %d index to %d, 2nd of %d bytes from cache begining, copied %d, bytes\n",
                __FUNCTION__, bytesAtCacheEnd, readIndexOffset, playback_ip->indexCacheSize-1, bytesRemaining, length));
        return length;
    }
}


typedef enum HttpCacheLookupResult {
    HTTP_CACHE_HIT,
    HTTP_CACHE_PARTIAL_HIT,
    HTTP_CACHE_RANGE_CACHEABLE,
    HTTP_CACHE_MISS
}HttpCacheLookupResult;

/* checks if a requested byte range is available in the cache or not. It returns the lookup result & cache index (in case of hit). */
HttpCacheLookupResult 
httpAvCache_lookup(
    B_PlaybackIpHandle playback_ip, 
    off_t byteRangeStart, 
    off_t byteRangeEnd, 
    int *cacheIndex)
{
    int i;
    B_Time curTime;

    *cacheIndex = HTTP_MAX_DATA_CACHES;

    for (i=0; i < HTTP_MAX_DATA_CACHES; i++) {
        if (!playback_ip->dataCache[i].inUse)
            continue;
        if (byteRangeStart >= playback_ip->dataCache[i].startOffset && 
                byteRangeEnd <= playback_ip->dataCache[i].endOffset) {
            /* cache hit: requested byte range is completely in the data cache */
            *cacheIndex = i;
            playback_ip->dataCache[i].stats.hits++;
            PRINT_CACHE_MSG(("%s: cache[%d] hit (total %d): byte range start %lld, end %lld, cache offset start %lld, end %lld\n", __FUNCTION__, i, 
                        playback_ip->dataCache[i].stats.hits, byteRangeStart, byteRangeEnd, 
                        playback_ip->dataCache[i].startOffset, playback_ip->dataCache[i].endOffset));
            B_Time_Get(&playback_ip->dataCache[i].lastUsedTime);
            return HTTP_CACHE_HIT;
        }
        if (byteRangeStart >= playback_ip->dataCache[i].startOffset && 
                byteRangeStart <= playback_ip->dataCache[i].endOffset) {
            /* partial cache hit: some initial part of the requested byte range is in the data cache */
            *cacheIndex = i;
            playback_ip->dataCache[i].stats.partialHits++;
            PRINT_CACHE_MSG(("%s: cache[%d] partial hit (total %d): byte range start %lld, end %lld, cache offset start %lld, end %lld\n", __FUNCTION__, i,
                        playback_ip->dataCache[i].stats.partialHits, byteRangeStart, byteRangeEnd, 
                        playback_ip->dataCache[i].startOffset, playback_ip->dataCache[i].endOffset));
            B_Time_Get(&playback_ip->dataCache[i].lastUsedTime);
            return HTTP_CACHE_PARTIAL_HIT;
        }
        /* if we are not doing trick modes, then check if the requested range can be cached with whole cache size */
        /* if so, we try to read data sequentially */
        if (!playback_ip->lastSeekPositionSet && playback_ip->playback_state != B_PlaybackIpState_eTrickMode && byteRangeStart >= playback_ip->dataCache[i].startOffset && 
                byteRangeStart <= (playback_ip->dataCache[i].endOffset + playback_ip->dataCache[i].size)) {
            /* byte range start is not currently cached, but can fit in the cache and thus is cacheable */
            *cacheIndex = i;
            playback_ip->dataCache[i].stats.cacheable++;
            PRINT_CACHE_MSG(("%s: cache[%d] miss, but range is cacheable (# %d): byte range start %lld, end %lld, cache offset start %lld, end %lld\n", __FUNCTION__, i,
                        playback_ip->dataCache[i].stats.cacheable, byteRangeStart, byteRangeEnd, 
                        playback_ip->dataCache[i].startOffset, playback_ip->dataCache[i].endOffset));
            B_Time_Get(&playback_ip->dataCache[i].lastUsedTime);
            return HTTP_CACHE_RANGE_CACHEABLE;
        }
    }

    /* if we reach here, that means the requested range is not currently in the cache */ 
    /* or cacheable in the current cache range, so this is CACHE_MISS scenario */

    /* thus caller will need to purge the least recently used data cache and refill it with requested data range */
    /* so we determine the least recently used cache index here */
    B_Time_Get(&curTime);
    for (i=0; i < HTTP_MAX_DATA_CACHES; i++) {
        int maxIdleTime=0, curIdleTime;
        if (!playback_ip->dataCache[i].inUse) {
            /* found a unused cache, use it */
            *cacheIndex = i;
            break;
        }
        if (i == 0) {
            maxIdleTime = B_Time_Diff(&curTime, &playback_ip->dataCache[i].lastUsedTime);
            *cacheIndex = i;
        }
        else {
            curIdleTime = B_Time_Diff(&curTime, &playback_ip->dataCache[i].lastUsedTime);
            if (curIdleTime > maxIdleTime) {
                maxIdleTime = curIdleTime;
                *cacheIndex = i;
            }
        }
    }
    if (*cacheIndex >= HTTP_MAX_DATA_CACHES) {
        if (HTTP_MAX_DATA_CACHES == 1) {
            PRINT_CACHE_MSG(("%s: only using one data cache, so overwriting its content w/ new request\n", __FUNCTION__));
        }
        else {
            BDBG_ERR(("%s: Error: we should always find (%d) one the data caches for use, defaulting it to cache 0\n", __FUNCTION__, *cacheIndex));
        }
        *cacheIndex = 0;
    }
    playback_ip->dataCache[*cacheIndex].stats.miss++;
    PRINT_CACHE_MSG(("%s: cache[%d] miss, range is outside cacheable range (# %d) for byte range start %lld, end %lld, cache offset start %lld, end %lld\n", __FUNCTION__, *cacheIndex,
        playback_ip->dataCache[*cacheIndex].stats.miss, byteRangeStart, byteRangeEnd, playback_ip->dataCache[*cacheIndex].startOffset, playback_ip->dataCache[*cacheIndex].endOffset));
    return HTTP_CACHE_MISS;
}

ssize_t 
http_avCacheMissProcessing(
    B_PlaybackIpHandle playback_ip, 
    off_t byteRangeStart, 
    off_t byteRangeEnd, 
    size_t length,
    int cacheIndex)
{
    int rc;
    ssize_t bytesRead;
    /* this function handles the cache miss event where the requested data is not in the data cache */
    /* this can happen in two cases: during normal rewind of content that is cached or when user issues */
    /* seek operation to a range that is outside of cacheable range */
    /* Our action is to close the current connection, setup a new one & ask the server to send data starting from new range */
    PRINT_CACHE_MSG(("%s: rewind/seek case: requested data is not in the cache, so resending the HTTP req to server, first data offset %lld, total read %lld, consumed %lld, rewind %d\n", 
            __FUNCTION__, playback_ip->dataCache[cacheIndex].firstOffset, playback_ip->totalRead, playback_ip->totalConsumed, playback_ip->rewind));
    PRINT_CACHE_MSG(("%s: byte range start %lld, end %lld, length %d, offset start %lld, end %lld\n",
            __FUNCTION__, byteRangeStart, byteRangeEnd, length, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset));

    if (playback_ip->reOpenSocket)
        playback_ip->reOpenSocket = false;
    /* send new HTTP Range request on a new socket */
    rc = B_PlaybackIp_HttpNetRangeReq(playback_ip, playback_ip->dataCache[cacheIndex].cache, HTTP_DATA_CACHE_CHUNK_SIZE, byteRangeStart, 0, playback_ip->socketState.fd, &playback_ip->socketState.fd);
    if (rc < 0) {
        BDBG_ERR(("%s: Failed to rewind the content, read %d, asked %u\n", __FUNCTION__, rc, HTTP_DATA_CACHE_CHUNK_SIZE));
        return -1 ;
    }
    bytesRead = rc;

    if (bytesRead != HTTP_DATA_CACHE_CHUNK_SIZE || (size_t)bytesRead < length) {
        PRINT_CACHE_MSG(("%s: during rewind/re-seek operation, data cache case: not able to read %d the requested amount %d, remaining amount %d\n", 
                __FUNCTION__, bytesRead, HTTP_DATA_CACHE_CHUNK_SIZE, length));
    }
    /* update the data cache pointers */
    playback_ip->rewind = false;
    playback_ip->dataCache[cacheIndex].startOffset = byteRangeStart;
    /* indicate addition of this new chunk by moving forward the end offset by a chunk size */
    playback_ip->dataCache[cacheIndex].endOffset = playback_ip->dataCache[cacheIndex].startOffset + bytesRead - 1;

    playback_ip->dataCache[cacheIndex].readIndex = 0;
    /* increase data cache depth by 1 chunk size */
    playback_ip->dataCache[cacheIndex].depth = bytesRead;

    /* increment data cache write index */
    playback_ip->dataCache[cacheIndex].writeIndex = bytesRead;

    /* now copy the remaining data from data cache into the requested buffer */
    PRINT_CACHE_MSG(("%s: during rewind/seek operation, copied %d bytes (asked %d) into data cache index %d\n", __FUNCTION__, bytesRead, length, cacheIndex));
    playback_ip->dataCache[cacheIndex].inUse = true;
    PRINT_CACHE_MSG(("%s: Data Cache[%d] Setup after rewind/seek: dcache offset start %lld, end %lld, size %d, index cache end offset %lld\n",
            __FUNCTION__, cacheIndex, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset, 
            playback_ip->dataCache[cacheIndex].size, playback_ip->indexCacheEndOffset));
    PRINT_CACHE_MSG(("%s: dcache: hit range start %lld, end %lld, cache start %lld, end %lld depth %d, rd idx %d, wr idx %d, read %u\n", 
            __FUNCTION__, byteRangeStart, byteRangeEnd, 
            playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
            playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, playback_ip->dataCache[cacheIndex].writeIndex, bytesRead));
    return bytesRead;
}

int 
http_avCachePartialHitProcessing(
    B_PlaybackIpHandle playback_ip, 
    off_t byteRangeStart, 
    off_t byteRangeEnd, 
    size_t length,
    int cacheIndex)
{
    int rc =-1;
    ssize_t bytesRead = 0;
    int cacheMissCount = 0;
    int bytesToRead = 0;
    int spaceAvailableAtEnd;

    BSTD_UNUSED(length);

    /* the requested data range is not completely in the data cache, read chunk at a time from network until we can service this range */
    while (byteRangeEnd > playback_ip->dataCache[cacheIndex].endOffset) {
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode)) {
            PRINT_CACHE_MSG(("%s: breaking of the while loop due to state change (state %d)", __FUNCTION__, playback_ip->playback_state));
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
                playback_ip->selectTimeout = true;
                errno = EINTR;
            }
            else {
                playback_ip->serverClosed = true;
            }
            break;
        }
        /* See if there is room in cache to read n/w data */ 
        /* else retire one older chunk from cache to make space for new one */
        if (playback_ip->dataCache[cacheIndex].depth >= playback_ip->dataCache[cacheIndex].size) {
            /* cache full, wrap around and retire one chunk, this is the least recently used chunk. */
            /* drop this oldest chunk of data by moving forward the start offset: increases linearly */
            playback_ip->dataCache[cacheIndex].startOffset += HTTP_DATA_CACHE_CHUNK_SIZE;
            /* move the read index forward as well so that it aligns with the StartOffset */
            playback_ip->dataCache[cacheIndex].readIndex += HTTP_DATA_CACHE_CHUNK_SIZE;
            if (playback_ip->dataCache[cacheIndex].readIndex >= playback_ip->dataCache[cacheIndex].size) {
                playback_ip->dataCache[cacheIndex].readIndex %= playback_ip->dataCache[cacheIndex].size;
                PRINT_CACHE_MSG(("%s: read index Wrap around: rd idx %d, depth %d, size %d\n", 
                        __FUNCTION__, playback_ip->dataCache[cacheIndex].readIndex, 
                        playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].size));
            }

            /* data cache depth is reduced by 1 chunk size */
            playback_ip->dataCache[cacheIndex].depth -= HTTP_DATA_CACHE_CHUNK_SIZE;
            PRINT_CACHE_MSG(("%s: dCache Wrap around: Retire %d byte chunk, start offset %lld, rd idx %d, depth %d, size %d\n", 
                        __FUNCTION__, HTTP_DATA_CACHE_CHUNK_SIZE, 
                        playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].readIndex, 
                        playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].size));
        }

        /* read lower of data chunk size or space available at end */
        spaceAvailableAtEnd = playback_ip->dataCache[cacheIndex].size - playback_ip->dataCache[cacheIndex].writeIndex;
        if (spaceAvailableAtEnd < HTTP_DATA_CACHE_CHUNK_SIZE) {
            PRINT_CACHE_MSG(("%s: reached towards end of data cache, so reading (%d) less than chunk size (%d), cache size %d, cache wr idx %d\n",
                        __FUNCTION__, spaceAvailableAtEnd, HTTP_DATA_CACHE_CHUNK_SIZE, playback_ip->dataCache[cacheIndex].size, playback_ip->dataCache[cacheIndex].writeIndex));
            bytesToRead = spaceAvailableAtEnd;
        }
        else 
            bytesToRead = HTTP_DATA_CACHE_CHUNK_SIZE;
        /* read 1 chunk worth data from n/w */
        if (playback_ip->reOpenSocket /* true if index was at the end (e.g. for ASF) and thus server had closed socket a/f sending the index part || pause is done using disconnect & seek */
                || playback_ip->cacheIndexUsingSocket != cacheIndex /* true if we are switching the data cache */
           ) {
            /* in both cases, we need to close the current socket & send a new request to server */
            /* both are done in NetRangeReq()) */
            BDBG_MSG(("%s: re-setup the connection for cache idx %d, prev cache idx %d, re-open socket %d, fd %d\n", __FUNCTION__, cacheIndex, playback_ip->cacheIndexUsingSocket, playback_ip->reOpenSocket, playback_ip->socketState.fd));
            playback_ip->reOpenSocket = false;
            playback_ip->cacheIndexUsingSocket = cacheIndex; 
            /* send new HTTP Range request on a new socket */
            rc = B_PlaybackIp_HttpNetRangeReq(playback_ip, playback_ip->dataCache[cacheIndex].cache + playback_ip->dataCache[cacheIndex].writeIndex, bytesToRead, playback_ip->dataCache[cacheIndex].endOffset+1, 0, playback_ip->socketState.fd, &playback_ip->socketState.fd);
        }
        else {
            rc = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, playback_ip->socketState.fd, playback_ip->dataCache[cacheIndex].cache + playback_ip->dataCache[cacheIndex].writeIndex, bytesToRead, playback_ip->networkTimeout);
        }
        if (rc < 0) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: Network Read Error, rc %d", __FUNCTION__, rc));
#endif
            /* return what ever has been read so far */
            goto error;
        }
        else if (rc == 0 && bytesRead == 0) {
            PRINT_CACHE_MSG(("%s: read 0 bytes from socket, server has already closed\n", __FUNCTION__, rc));
            goto error;
        }
        bytesRead += rc;

        /* increment data cache write index */
        playback_ip->dataCache[cacheIndex].writeIndex += rc;
        playback_ip->dataCache[cacheIndex].writeIndex %= playback_ip->dataCache[cacheIndex].size;
        /* indicate addition of this new chunk by moving forward the end offset by a chunk size */
        playback_ip->dataCache[cacheIndex].endOffset == 0 ? 
            playback_ip->dataCache[cacheIndex].endOffset = rc - 1 :
            (playback_ip->dataCache[cacheIndex].endOffset += rc);
        /* increase data cache depth by bytesRead */
        playback_ip->dataCache[cacheIndex].depth += rc;

        cacheMissCount++;
        if (playback_ip->serverClosed) {
            PRINT_CACHE_MSG(("%s: breaking from reading loop due to server closing connection, bytes read %d", __FUNCTION__, rc));
            break;
        }
        if (playback_ip->selectTimeout) {
            PRINT_CACHE_MSG(("%s: breaking from reading loop due to select timeout, bytes read %d", __FUNCTION__, rc));
            break;
        }

        /* otherwise, go back to see if the byte range request can be serviced */
    }
error:
    PRINT_CACHE_MSG(("%s: dCache: fd %d, asked %d, read %d: miss %d, cache start %lld, end %lld depth %d, wr idx %d\n",
            __FUNCTION__, playback_ip->socketState.fd, bytesToRead, bytesRead, cacheMissCount,
            playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
            playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].writeIndex));
    if (bytesRead) {
        /* we have read something: now make sure if we have satisfied all conditions */
        if (byteRangeEnd <= playback_ip->dataCache[cacheIndex].endOffset) {
            /* we have read all requested data from byteRangeStart to End */
            PRINT_CACHE_MSG(("%s: dCache: requested data (%d) was read (%d) w/o interrutions, server closed %d, select timeout %d\n", __FUNCTION__, length, bytesRead, playback_ip->serverClosed, playback_ip->selectTimeout));
            return (int)bytesRead;
        }
        /* now we know that end range is outside the current cache end, check where is the start range, we know that it is > startRange */
        else if (byteRangeStart > playback_ip->dataCache[cacheIndex].endOffset) {
            /* startRange is outside what has been currently read */
            /* even though we have read some data, requested byteRangeStart is still not read possibly due to server close or timeout event */
            PRINT_CACHE_MSG(("%s: dCache: some requested data (%d) read (%d), but still before start range, server closed %d, select timeout %d\n", __FUNCTION__, length, bytesRead, playback_ip->serverClosed, playback_ip->selectTimeout));
            return -1;
        } else {
            /* this means that start is <= endOffset, so we return data that is in the cache */
            rc = playback_ip->dataCache[cacheIndex].endOffset - byteRangeStart + 1;
            PRINT_CACHE_MSG(("%s: dCache: requested data (%d) was partially read (%d), so returning %d, server closed %d, select timeout %d\n", __FUNCTION__, length, bytesRead, rc, playback_ip->serverClosed, playback_ip->selectTimeout));
            return rc;
        }
    }
    else
        return rc;
}

int 
http_avCacheHitProcessing(
    B_PlaybackIpHandle playback_ip, 
    off_t byteRangeStart, 
    off_t byteRangeEnd, 
    char *buf,
    size_t length,
    int cacheIndex)
{
    unsigned int readIndexOffset = 0;
    int bytesCopied = 0;
    /* make sure asked byte range is in data cache & then copy it into the requested buffer */
    if (byteRangeEnd <= playback_ip->dataCache[cacheIndex].endOffset && byteRangeStart >= playback_ip->dataCache[cacheIndex].startOffset) {
        /* calculate the read index into the cache for this byte range */
        readIndexOffset = playback_ip->dataCache[cacheIndex].readIndex + (byteRangeStart - playback_ip->dataCache[cacheIndex].startOffset);
        readIndexOffset %= playback_ip->dataCache[cacheIndex].size;

        /* look for wrap around */
        if ((readIndexOffset + length) <= playback_ip->dataCache[cacheIndex].size) {
            /* all data range is linealy present in the cache before its end, so do a straight copy */
            memcpy((unsigned char *)buf, playback_ip->dataCache[cacheIndex].cache+readIndexOffset, length);
            BDBG_MSG(("%s: One copy of %d bytes from %d index\n", __FUNCTION__, length, readIndexOffset));
            bytesCopied = length;
        }
        else {
            /* initial part of the data is at the end and other is at the begining (due to cache wrap around) of data cache */
            int bytesAtCacheEnd;
            int bytesRemaining;
            /* wrap around case, first copy data that is available at the end of the buffer */
            bytesAtCacheEnd = (playback_ip->dataCache[cacheIndex].size - readIndexOffset);
            bytesRemaining = length - bytesAtCacheEnd;
            /* first copy of bytes at the end of the cache */
            memcpy(buf, playback_ip->dataCache[cacheIndex].cache+readIndexOffset, bytesAtCacheEnd);

            /* and copy remaining bytes from the begining of the cache */
            memcpy(buf+bytesAtCacheEnd, playback_ip->dataCache[cacheIndex].cache, bytesRemaining);
            bytesCopied = bytesAtCacheEnd + bytesRemaining;
            BDBG_MSG(("%s: Two copies: first of %d bytes from %d index to %d, 2nd of %d bytes from cache begining, copied %d, asked %d bytes\n",
                        __FUNCTION__, bytesAtCacheEnd, readIndexOffset, playback_ip->dataCache[cacheIndex].size-1, 
                        bytesRemaining, bytesCopied, length));
        }
        BDBG_MSG(("dcache: hit range start %lld, end %lld, cache start %lld, end %lld depth %d, rd idx %d, rd offset %d, read %u\n", 
                    byteRangeStart, byteRangeEnd, 
                    playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, readIndexOffset, length));
        if (playback_ip->setupSettings.u.http.convertLpcmToWave) {
            /* LPCM data is in the BE format, but Audio FW only understands LE format WAV. So convert into LE by byte-swapping the data */
            B_PlaybackIp_UtilsByteSwap((char *)buf, bytesCopied);
        }
        return bytesCopied;
    }
    else {
        BDBG_ERR(("%s: SW bug, requested range should have been in data cache : range start %lld, end %lld, cache start %lld, end %lld depth %d, rd idx %d, rd offset %d, read %u\n", 
                    __FUNCTION__, byteRangeStart, byteRangeEnd, 
                    playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, readIndexOffset, length));
        return -1;
    }
}

int http_validate_and_adjust_byte_ranges(B_PlaybackIpHandle playback_ip, off_t *byteRangeStartPtr, off_t *byteRangeEndPtr, size_t *lengthPtr)
{
    off_t byteRangeStart;
    off_t byteRangeEnd;
    size_t length;
    int cacheIndex;

    length = *lengthPtr;
    byteRangeStart = *byteRangeStartPtr;
    byteRangeEnd = *byteRangeEndPtr;
    cacheIndex = playback_ip->lastUsedCacheIndex;
    if (!playback_ip->dataCache[cacheIndex].inUse) /* since caches are not even yet setup, ignore byte range validation for now */
        return 0;
    if (playback_ip->serverClosed) {
        /* we may need to adjust the requested if server is no longer sending data & player is asking for more data outside the cache */
        cacheIndex = playback_ip->cacheIndexUsingSocket;
        if (byteRangeStart > playback_ip->dataCache[cacheIndex].endOffset) {
            /* TODO: this check makes us potentially not reconnect to the server when we haven't cached enough data, may need to take this out */
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: server closed case: requested range start is outside data cache end, returning ERROR : range start %lld, end %lld, cache start %lld, end %lld depth %d, rd idx %d, read %u\n", 
                    __FUNCTION__, byteRangeStart, byteRangeEnd, 
                    playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, length));
#endif
            return -1;
        }
        /* now byteRangeStart is within cache, check for end for it being outside and trim it */
        if (byteRangeEnd > playback_ip->dataCache[cacheIndex].endOffset) {
            byteRangeEnd = playback_ip->dataCache[cacheIndex].endOffset;
            length = byteRangeEnd - byteRangeStart +1;
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog) {
                BDBG_ERR(("%s: server closed case: requested range start %lld, end %lld is outside data cache start %lld end %lld, trimming it\n", 
                    __FUNCTION__, byteRangeStart, byteRangeEnd, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset));
                BDBG_ERR(("%s: new range start %lld, new end %lld, cache start %lld, end %lld depth %d, rd idx %d, read %u\n", 
                    __FUNCTION__, byteRangeStart, byteRangeEnd, 
                    playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, length));
            }
#endif
        }
    }
    else if (playback_ip->contentLength) {
        /* we may need to adjust the requested range if player is asking for more data outside the content length */
        cacheIndex = playback_ip->lastUsedCacheIndex;
        if (byteRangeStart >= playback_ip->contentLength) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: requested range start is outside content length %lld, returning ERROR : range start %lld, end %lld, cache start %lld, end %lld depth %d, rd idx %d, read %u\n", 
                    __FUNCTION__, playback_ip->contentLength, byteRangeStart, byteRangeEnd, 
                    playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, length));
#endif
            return -1;
        }
        /* now byteRangeStart is within cache, check for end for it being outside and trim it */
        if (byteRangeEnd >= playback_ip->contentLength) {
            byteRangeEnd = playback_ip->contentLength -1;
            length = byteRangeEnd - byteRangeStart +1;
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog) {
                BDBG_ERR(("%s: range end is after content length %lld: requested range start %lld, end %lld is outside data cache start %lld end %lld, trimming it\n", 
                        __FUNCTION__, playback_ip->contentLength, byteRangeStart, byteRangeEnd, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset));
                BDBG_ERR(("%s: new range start %lld, new end %lld, cache start %lld, end %lld depth %d, rd idx %d, read %u\n", 
                        __FUNCTION__, byteRangeStart, byteRangeEnd, 
                        playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
                        playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, length));
            }
#endif
        }
    }
    *lengthPtr = length;
    *byteRangeStartPtr = byteRangeStart;
    *byteRangeEndPtr = byteRangeEnd;
    return 0;
}

int 
http_read_data_from_data_cache(
    B_PlaybackIpHandle playback_ip, 
    struct bfile_io_read_net *file,
    off_t byteRangeStart, 
    off_t byteRangeEnd, 
    char *buf,
    size_t length
    )
{
    int rc = -1;
    int bytesRead = 0;
    unsigned int bytesToCopy;
    size_t origLength;
    int cacheIndex;
    HttpCacheLookupResult cacheLookupResult;
    static FILE * fclear = NULL;

            
    origLength = length;
    if (http_validate_and_adjust_byte_ranges(playback_ip, &byteRangeStart, &byteRangeEnd, &length)) {
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_ERR(("%s: Invalid byte range requested, returning EOF", __FUNCTION__));
#endif
        BKNI_ReleaseMutex(playback_ip->lock);
        return (ssize_t)0;
    }

    /* set a flag to indicate that read call back in progress */
    /* Note: this flag needs to be cleared before we return from this function, so can't have any more return statements until end of this function (instead use goto out/error) */
    playback_ip->readCallBackInProgress = true;

    /* special check for playing LPCM files: since Audio decoder doesn't have support for native LPCM files, */
    /* we convert LPCM files into WAV by inserting WAV header in the begining. */
    if (playback_ip->setupSettings.u.http.convertLpcmToWave) {
        /* length needs to be 2byte aligned as we will need to convert the data from BE to LE format */
        length -= length%2;
        if (!playback_ip->waveHeaderInserted) {
            rc = B_PlaybackIp_UtilsBuildWavHeader((void *)buf, length, playback_ip->setupSettings.u.http.bitsPerSample, playback_ip->setupSettings.u.http.sampleRate, playback_ip->setupSettings.u.http.numChannels);
            if (rc < 0) {
                BDBG_ERR(("%s: Failed to build Wave Header for LPCM files\n", __FUNCTION__));
                goto error;
            }
            buf = (char *)buf + rc;
            length -= rc;
            byteRangeEnd = byteRangeStart+length-1;
            bytesRead = rc;
            playback_ip->waveHeaderInserted = true;
            BDBG_MSG(("%s: Built & Inserted Wave Header ( bits/sample %d, rate %d, #ch %d) for LPCM files\n", __FUNCTION__,
                playback_ip->setupSettings.u.http.bitsPerSample, playback_ip->setupSettings.u.http.sampleRate, playback_ip->setupSettings.u.http.numChannels));
        }
    }

    if (!playback_ip->dataCache[0].inUse) {
        if (playback_ip->initial_data_len) {
            /* some data got read part of reading the HTTP response, copy that data into data cache & initialize cache state variables */
            memcpy(playback_ip->dataCache[0].cache, playback_ip->temp_buf, playback_ip->initial_data_len);
            playback_ip->dataCache[0].depth = playback_ip->initial_data_len;
            playback_ip->dataCache[0].startOffset = 0;
            playback_ip->dataCache[0].endOffset = playback_ip->initial_data_len -1;
            playback_ip->dataCache[0].readIndex = 0;
            playback_ip->dataCache[0].writeIndex = playback_ip->initial_data_len;
            BDBG_MSG(("%s: Copied initial payload of %d bytes into the data cache\n", __FUNCTION__, playback_ip->initial_data_len));
            playback_ip->initial_data_len = 0;
        }
        else {
            /* initialize data cache */
            /* starting offset of the data data: it should be the next byte after what is present in the index cache */
            if (playback_ip->indexCacheInit && playback_ip->indexCacheEndOffset > 0)
                playback_ip->dataCache[0].startOffset = playback_ip->indexCacheEndOffset + 1;
            else
                playback_ip->dataCache[0].startOffset = 0;
            playback_ip->dataCache[0].readIndex = 0;
            playback_ip->dataCache[0].endOffset = playback_ip->dataCache[0].startOffset - 1;
            playback_ip->dataCache[0].depth = 0;
            playback_ip->dataCache[0].writeIndex = 0;
        }
        BDBG_MSG(("%s: Data Cache Setup: dcache offset start %lld, end %lld, size %d, index cache end offset %lld\n",
                    __FUNCTION__, playback_ip->dataCache[0].startOffset, playback_ip->dataCache[0].endOffset, 
                    playback_ip->dataCache[0].size, playback_ip->indexCacheEndOffset));
        playback_ip->dataCache[0].inUse = true;
        playback_ip->dataCache[0].firstOffset = file->offset;
    }
    else {
        /* data Cache is initialized, check if offset has been rewind, then reset state */ 
        if (file->offset <= playback_ip->dataCache[0].firstOffset && playback_ip->playback_state != B_PlaybackIpState_eTrickMode) {
            /* rewind flag is only used for cases where we are not caching the data as we dont know when the wrap around happens in those cases */
            playback_ip->rewind = true;
            playback_ip->totalConsumed = 0;
            BDBG_MSG(("%s: rewinding: first data offset %lld, cur %lld, total read %lld, consumed %lld\n", 
                        __FUNCTION__, playback_ip->dataCache[0].firstOffset, file->offset, playback_ip->totalRead, playback_ip->totalConsumed));
        }
    }

    /* 
     * Look if asked byte range is in the index cache. This is only possible when index is in the beginging.
     * For container formats like MPEG TS, there is no separate index and psi information is only derived from parsing the content.
     * When index is at the end (possible for ASF & some MP4 content), data can't be in the index cache.
     */
    if (!playback_ip->indexAtEnd && playback_ip->indexCacheEndOffset ) {
        /* index is not at the end of stream (thus is available in index cache) and is valid */
        if (byteRangeStart >= playback_ip->indexCacheStartOffset && byteRangeEnd <= playback_ip->indexCacheEndOffset) {
            /* asked byte range is completely in index cache */
            BDBG_MSG(("%s: Cache hit in icache: range start %llu, end %llu, icache end off %lld, asked %u, total read %lld, consumed %lld\n", 
                     __FUNCTION__, byteRangeStart, byteRangeEnd, playback_ip->indexCacheEndOffset, length, playback_ip->totalRead, playback_ip->totalConsumed));
            bytesRead = http_read_data_from_index_cache(playback_ip, byteRangeStart, byteRangeEnd, buf, length);
            goto out;
        }
        else if (byteRangeStart <= playback_ip->indexCacheEndOffset && byteRangeEnd > playback_ip->indexCacheEndOffset) {
            /* asked byte range is partially in index cache */
            unsigned int idx;
            idx = (unsigned)byteRangeStart;
            bytesToCopy = playback_ip->indexCacheEndOffset - byteRangeStart + 1;
            bytesRead = http_read_data_from_index_cache(playback_ip, byteRangeStart, byteRangeEnd, buf, bytesToCopy);
            bytesRead = bytesToCopy;

            length -= bytesRead;
            byteRangeStart += bytesRead;
            buf = (char *)buf + bytesRead;
            playback_ip->totalConsumed += bytesRead;
            BDBG_MSG(("%s: Partial Cache hit in icache: range start %llu, end %llu, icache depth %lld, copied %u, remaining %u\n", 
                     __FUNCTION__, byteRangeStart, byteRangeEnd, playback_ip->indexCacheEndOffset, bytesToCopy, length));
        }
    }

    /* At this point, requested range is not in the index cache. */

    /* 
     * If we are here, then we are always caching movie data. This is currently true for both MP4 & ASF formats.
     * Note: we always cache the index related data.
     */

    /* check if requested range is in one of the data caches */
    cacheLookupResult = httpAvCache_lookup(playback_ip, byteRangeStart, byteRangeEnd, &cacheIndex);
    switch (cacheLookupResult) {
    case HTTP_CACHE_MISS:
        /* requested range is not in cache, restart connection to server and refill cache from the requested range */
        rc = http_avCacheMissProcessing(playback_ip, byteRangeStart, byteRangeEnd, length, cacheIndex);
        if (rc < 0) {
            BDBG_ERR(("%s: failed to restart connection to server to refill cache from new range, rc %d\n", __FUNCTION__, rc));
            goto error;
        }
        else if (rc < (int)length) {
            BDBG_MSG(("%s: lowered length to rc asked %d, read %d\n",__FUNCTION__, length, rc));
            length = rc;
        }
        /* now copy the read data */
        bytesToCopy = length;
        memcpy(buf, playback_ip->dataCache[cacheIndex].cache, bytesToCopy);
        if (playback_ip->setupSettings.u.http.convertLpcmToWave) {
            /* LPCM data is in the BE format, but Audio FW only understands LE format WAV. So convert into LE by byte-swapping the data */
            B_PlaybackIp_UtilsByteSwap((char *)buf, bytesToCopy);
        }
        bytesRead += bytesToCopy;
        playback_ip->totalConsumed += bytesToCopy;
        playback_ip->cacheIndexUsingSocket = cacheIndex;
        playback_ip->lastUsedCacheIndex = cacheIndex;
        BDBG_MSG(("%s: returning %d bytes, orig length %u, length %u, byteRangeStart %lld, byteRangeEnd %lld, total read %lld\n", __FUNCTION__, bytesRead, origLength, length, byteRangeStart, byteRangeEnd, playback_ip->totalRead));
        goto out;
    case HTTP_CACHE_PARTIAL_HIT:
    case HTTP_CACHE_RANGE_CACHEABLE:
        /* since initial part of the data is in cache or atleast the requested range is cacheable */
        /* meaning that it fits in the cache size, we try to read more data from network until we satify the request */
        rc = http_avCachePartialHitProcessing(playback_ip, byteRangeStart, byteRangeEnd, length, cacheIndex);
        if (rc < 0) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_MSG(("%s: failed to fill up the current data cache, rc %d", __FUNCTION__, rc));
#endif
            goto error;
        }
        else if (rc < (int)length && byteRangeEnd > playback_ip->dataCache[cacheIndex].endOffset) {
            BDBG_MSG(("%s: couldn't read all requested amount from n/w, read %d, asked %u, server has closed %d, select timeout %d\n", __FUNCTION__, rc, length, playback_ip->serverClosed, playback_ip->selectTimeout));
            if (byteRangeStart > playback_ip->dataCache[cacheIndex].endOffset) {
                BDBG_MSG(("%s: didn't get to read any data, byteRate start %lld, end %lld, cache end %lld, selectTImeout %d",
                            __FUNCTION__, byteRangeStart, byteRangeEnd, playback_ip->dataCache[cacheIndex].endOffset, playback_ip->selectTimeout));
                goto error;
            }
            byteRangeEnd = playback_ip->dataCache[cacheIndex].endOffset;
            length = byteRangeEnd - byteRangeStart + 1;
            BDBG_MSG(("%s: modified new byteRange end to %lld, start %lld\n", __FUNCTION__, byteRangeEnd, byteRangeStart));
        }
        /* at this point, we should have the requested byte range in the cache, so we drop into the cache hit case below */
    case HTTP_CACHE_HIT:
        /* requested range is in the cache, copy it to the caller's buffer and return */
        rc = http_avCacheHitProcessing(playback_ip, byteRangeStart, byteRangeEnd, buf, length, cacheIndex);
        if (rc <= 0) {
            BDBG_ERR(("%s: failed to return requested data, rc %d\n", __FUNCTION__, rc));
            goto error;
        }
        bytesRead += rc;
        playback_ip->lastUsedCacheIndex = cacheIndex;
        BDBG_MSG(("%s: returning %d bytes, orig length %u, length %u, byteRangeStart %lld, byteRangeEnd %lld, total Read %d\n", __FUNCTION__, bytesRead, origLength, length, byteRangeStart, byteRangeEnd, playback_ip->totalRead));
        goto out;
    default:
        BDBG_ASSERT(NULL);
    }

error:
    bytesRead = rc; 
out:
    if (bytesRead > 0) {
        file->offset += bytesRead;
        if (fclear) {
            fwrite(buf, 1, bytesRead , fclear); 
            fflush(fclear);
        }
    }
    
    playback_ip->readCallBackInProgress = false;
    BKNI_ReleaseMutex(playback_ip->lock);
    if (playback_ip->serverClosed && playback_ip->contentLength == 0) {
        /* content length is not specified when server is sending data using chunk xfer encoding */
        /* we can set the content length once we have reached the end of stream as it helps w/ rewind purposes */
        playback_ip->contentLength = playback_ip->dataCache[playback_ip->lastUsedCacheIndex].endOffset + 1;
        BDBG_MSG(("%s: for chunk encoding case, set the content length %lld\n", __FUNCTION__, playback_ip->contentLength));
    }
    if (bytesRead > 0)
        return (ssize_t)bytesRead;
    else {
         if (playback_ip->selectTimeout) {
#ifdef BDBG_DEBUG_BUILD
             if (playback_ip->ipVerboseLog)
                 BDBG_ERR(("%s: select timeout, tell playback to retry read\n", __FUNCTION__));
#endif
            playback_ip->selectTimeout = false;
            return BFILE_ERROR_NO_DATA;
         }
         else if (bytesRead == 0) {
             /* EOF */
             return 0;
         }
         else {
            BDBG_ERR(("%s: ERROR: read callback failed, rc %d, returing EOF", __FUNCTION__, bytesRead));
            return (ssize_t)0;
         }
    }
}

static ssize_t
B_PlaybackIp_HttpNetDataRead(bfile_io_read_t self, void *buf, size_t length)
{
    struct bfile_io_read_net *file = (struct bfile_io_read_net *) self;
    B_PlaybackIpHandle playback_ip;
    off_t byteRangeStart;
    off_t byteRangeEnd;
    size_t origLength;
    static FILE * fclear = NULL;
    static int fileNameSuffix = 0;
    char recordFileName[32];
    if (!file || !file->playback_ip) {
        return -1;
    }
    playback_ip = file->playback_ip;

    BDBG_MSG(("%s: read %d bytes at offset %lld", __FUNCTION__, length, file->offset));
    if (BKNI_TryAcquireMutex(playback_ip->lock) == BERR_TIMEOUT) {
        BDBG_MSG(("%s: can't get the lock, returning"));
        BKNI_Sleep(80); /* delay returning back to the caller (nexus io thread) otherwise it re-invokes this call rightaway and thus wastes cycles in preventing the thread holding the lock to run */
        return BFILE_ERROR_NO_DATA;
    }
    if (playback_ip->enableRecording && !fclear) {
        memset(recordFileName, 0, sizeof(recordFileName));
        snprintf(recordFileName, sizeof(recordFileName)-1, "./videos/http_playback_rec%d.ts", fileNameSuffix++);
        fclear = fopen(recordFileName, "w+b");
    }
    if ((((struct bfile_in_net *)playback_ip->nexusFileHandle)->self.file.index != NULL) &&
        (playback_ip->playback_state == B_PlaybackIpState_eBuffering ||  /* HTTP thread is currently buffering */
        playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode || /* app is waiting to initiate a trickmode operation, but can't proceed until IO thread completes */
        (playback_ip->playback_state == B_PlaybackIpState_ePaused && playback_ip->ipTrickModeSettings.pauseMethod == B_PlaybackIpPauseMethod_UseDisconnectAndSeek) || /* If pausing using disconnect & seek method, we can't return any data until pause-release (otherwise, this function will reconnect to server) */
        playback_ip->playback_state == B_PlaybackIpState_eEnteringTrickMode)) /* a user thread called trickmode function which is being setup */
    {
        if (!playback_ip->printedOnce) {
            BDBG_MSG(("%s: playback is requesting data when %s, state %d, return w/o data", 
                        __FUNCTION__, 
                        playback_ip->playback_state == B_PlaybackIpState_eBuffering ? "pre-charging is going on" : "transitioning to trick mode state", 
                        playback_ip->playback_state));
            playback_ip->printedOnce = true;
        }
        BKNI_ReleaseMutex(playback_ip->lock);
        BKNI_Sleep(80);
        return BFILE_ERROR_NO_DATA;
    }
    else 
        playback_ip->printedOnce = false;
    if (!playback_ip->firstDataReadCallbackDone && playback_ip->psi.liveChannel) {
        BKNI_Sleep(2000);
        playback_ip->firstDataReadCallbackDone = true;
    }
    origLength = length;
    byteRangeStart = file->offset;
    byteRangeEnd = byteRangeStart+length-1;
    /* coverity[lock] */
    return http_read_data_from_data_cache(playback_ip, file, byteRangeStart, byteRangeEnd, buf, length);
}

static off_t
B_PlaybackIp_HttpNetDataSeek(bfile_io_read_t self, off_t offset, int whence)
{
    struct bfile_io_read_net *file = (struct bfile_io_read_net *) self;
    if (!file || !file->playback_ip) {
        return -1;
    }

    if (file->playback_ip->contentLength && offset > file->playback_ip->contentLength) {
        BDBG_WRN(("%s: seeking beyond EOF (content length %lld, offset %lld, whence %d), returning error", __FUNCTION__, file->playback_ip->contentLength, offset, whence));
        return -1;
    }

    if (whence != 0) {
        BDBG_MSG(("%s: whence is not 0: playback_ip %p, file %p, offset %lld, whence %d\n", __FUNCTION__, file->playback_ip, file, offset, whence));
    }
#ifdef THIS_MAYNOT_BE_NEEDED
    if (file->playback_ip->playback_state == B_PlaybackIpState_eEnteringTrickMode) {
        BDBG_WRN(("%s: during trick modes transition, temporarily fail the seek call \n", __FUNCTION__));
        return -1;
    }
#endif
    BDBG_MSG(("%s: playback_ip %p, file %p, offset %lld, whence %d, fd %d, reOpenSocket %d\n", __FUNCTION__, file->playback_ip, file, offset, whence, file->playback_ip->socketState.fd, file->playback_ip->reOpenSocket));
    file->offset = offset;
    if (file->playback_ip->playback_state != B_PlaybackIpState_eBuffering)
        /* only update lastSeekOffset if we are not buffering */
        file->playback_ip->lastSeekOffset = offset;
    return offset;
}

static int
B_PlaybackIp_HttpNetDataBounds(bfile_io_read_t self, off_t *first, off_t *last)
{
    struct bfile_io_read_net *file = (struct bfile_io_read_net *) self;

    if (!file || !file->playback_ip) {
        return -1;
    }
    /* if content length is not known (either server didn't send it or is sending content using chunk encoding */
    /* then returning -1 here puts players in loop forever mode until server closes the connection or we channel changes */
    if (file->playback_ip->contentLength == 0 || file->playback_ip->serverClosed) {
        BDBG_MSG(("%s: returning -1, content length %lld, server closed %d\n", 
                    __FUNCTION__, file->playback_ip->contentLength, file->playback_ip->serverClosed));
        return -1;
    }
    *first = 0;
    *last = file->playback_ip->contentLength;
    BDBG_MSG(("%s: playback_ip %p, file %p, last %lld, size %lld\n", __FUNCTION__, 
                file->playback_ip, file, *last, file->playback_ip->contentLength));
    return 0;
}

static int
B_PlaybackIp_HttpNetIndexBounds(bfile_io_read_t self, off_t *first, off_t *last)
{
    struct bfile_io_read_net *file = (struct bfile_io_read_net *) self;

    if (!file || !file->playback_ip) {
        return -1;
    }
    /* if content length is not known (either server didn't send it or is sending content using chunk encoding */
    /* then returning -1 here puts players in loop forever mode until server closes the connection or we channel change */
    if (file->playback_ip->contentLength == 0 /* || file->playback_ip->serverClosed */ ) {
        BDBG_MSG(("%s: returning -1, content length %lld, server closed %d", 
                    __FUNCTION__, file->playback_ip->contentLength, file->playback_ip->serverClosed));
        return -1;
    }
    *first = 0;
    *last = file->playback_ip->contentLength;
    BDBG_MSG(("%s: playback_ip %p, file %p, last %d\n", __FUNCTION__, file->playback_ip, file, *last));
    return 0;
}

static off_t
B_PlaybackIp_HttpNetIndexSeek(bfile_io_read_t self, off_t offset, int whence)
{
    struct bfile_io_read_net *file = (struct bfile_io_read_net *) self;
    B_PlaybackIpHandle playback_ip;

    if (!file || !file->playback_ip) {
        return -1;
    }
    playback_ip = file->playback_ip;
    BDBG_MSG(("%s: playback_ip %p, file %p, offset %lld, whence %d\n", __FUNCTION__, file->playback_ip, file, offset, whence));
    if (playback_ip->contentLength && offset > file->playback_ip->contentLength) {
        BDBG_WRN(("%s: seeking beyond EOF (content length %lld, offset %lld, whence %d), returning error", __FUNCTION__, file->playback_ip->contentLength, offset, whence));
        return -1;
    }

    if (!playback_ip->indexAtEnd && !playback_ip->setupSettings.u.http.disableRangeHeader) {
        /* to determine if index is at the end, we use following logic: */
        /* if content length is known (in most cases unless server is using HTTP chunk xfer encoding to send data), */
        /* then if requested offset is after 3/4th the content length, then index is at the end. */
        /* if content length is not known, then we check is requested offset is after 10MB. This gives enough data to determine media info */
        /* of the MPEG2/AVC TS type files, but still works for ASF & MP4 where index can be at the end. */
        if ((playback_ip->contentLength && offset > ((3*file->playback_ip->contentLength)/4)) || 
           (!playback_ip->contentLength && offset > (10*1024*1024))) {  /* ContentLength is not known in chunk encoding case */
            BDBG_MSG(("%s: index is towards end, resetting index cache: offset %lld, content length %lld\n", __FUNCTION__, offset, file->playback_ip->contentLength));
            if (!playback_ip->dataCache[0].inUse) {
                /* data cache is not yet setup (it can be setup if pre-buffering is already done, true for ASF files) */
                /* Copy currently read data from index to data cache as it is really the movie data & not index data */

                if (playback_ip->indexCacheDepth > playback_ip->dataCache[0].size) {
                    BDBG_MSG(("%s: index cache is too big to be copied into data cache\n", __FUNCTION__, playback_ip->indexCacheDepth, playback_ip->dataCache[0].size));
                    goto out;
                }
                memcpy(playback_ip->dataCache[0].cache, playback_ip->indexCache, playback_ip->indexCacheDepth);
                BDBG_MSG(("%s: copied %d bytes into data cache\n", __FUNCTION__, playback_ip->indexCacheDepth));
                /* increment data cache write index */
                playback_ip->dataCache[0].writeIndex = playback_ip->indexCacheDepth;
                /* indicate addition of this new chunk by moving forward the end offset by a chunk size */
                playback_ip->dataCache[0].endOffset = playback_ip->indexCacheEndOffset;
                /* increase data cache depth by 1 chunk size */
                playback_ip->dataCache[0].depth = playback_ip->indexCacheDepth;
                playback_ip->dataCache[0].readIndex = 0;
                playback_ip->dataCache[0].startOffset = playback_ip->indexCacheStartOffset;
                playback_ip->dataCache[0].inUse = true;
                BDBG_MSG(("%s: data cache start off %lld, end %lld, depth %u, wr idx %u\n",
                        __FUNCTION__, playback_ip->dataCache[0].startOffset, playback_ip->dataCache[0].endOffset, playback_ip->dataCache[0].depth, playback_ip->dataCache[0].writeIndex));
            }
            /* tell Read callback to open a separate connection to server for downloading the index (MOOV object) as it is towards the end of the file */
            playback_ip->indexAtEnd = true;
            playback_ip->indexCacheInit = 0;
        }
    }

out:
    file->offset = offset;
    playback_ip->indexSeekCount++;
    return offset;
}

static ssize_t
B_PlaybackIp_HttpNetIndexRead(bfile_io_read_t self, void *buf, size_t length)
{
    int rc = -1;
    B_PlaybackIpHandle playback_ip;
    off_t byteRangeStart;
    off_t byteRangeEnd;
    int cacheMissCount = 0;
    struct bfile_io_read_net *file = (struct bfile_io_read_net *) self;
    ssize_t bytesRead = 0;
    ssize_t bytesToRead = 0;
    off_t rangeEnd;
    int spaceAvailableAtEnd, spaceLeftInCache;
    static FILE * fclear = NULL;
    static int indexFileNameSuffix = 0;
    char recordFileName[32];

    if (!file || !file->playback_ip) {
        return -1;
    }
    playback_ip = file->playback_ip;
    file->fd = playback_ip->socketState.fd;
    if (playback_ip->playback_state == B_PlaybackIpState_eStopping || (playback_ip->playback_state == B_PlaybackIpState_eStopped)) {
        BDBG_MSG(("%s: returning error (-1) due to state %d change (stop event)", __FUNCTION__, playback_ip->playback_state));
        rc = -1;
        goto error;
    }
    if (playback_ip->enableRecording && fclear == NULL) {
        memset(recordFileName, 0, sizeof(recordFileName));
        snprintf(recordFileName, sizeof(recordFileName)-1, "./videos/http_index_rec%d.ts", indexFileNameSuffix++);
        fclear = fopen(recordFileName, "w+b");
    }
    byteRangeStart = file->offset;
    byteRangeEnd = byteRangeStart+length-1;
    if (playback_ip->contentLength && byteRangeEnd >= playback_ip->contentLength) {
        if (byteRangeStart >= playback_ip->contentLength) {
            BDBG_MSG(("%s: asking to read beyond EOF, returning -1", __FUNCTION__));
            return -1;
        }
        byteRangeEnd = playback_ip->contentLength -1;
        BDBG_MSG(("%s: trimmed byteRangeEnd from %lld to %lld\n", __FUNCTION__, byteRangeStart+length,  byteRangeEnd));
        length = byteRangeEnd - byteRangeStart +1;
    }
    BDBG_MSG(("%s: socketFd %d, read %d bytes of index data at range start %lld, end %lld", __FUNCTION__, playback_ip->socketState.fd, length, byteRangeStart, byteRangeEnd));

    if (playback_ip->playback_state == B_PlaybackIpState_eSessionSetupInProgress && 
        playback_ip->setupSettings.u.http.psiParsingTimeLimit) {
        if (!playback_ip->mediaProbeStartTimeNoted) {
            BDBG_MSG(("%s: parsingTimeLimit %d", __FUNCTION__, playback_ip->setupSettings.u.http.psiParsingTimeLimit));
            B_Time_Get(&playback_ip->mediaProbeStartTime);
            playback_ip->mediaProbeStartTimeNoted = true;
        }
        else {
            B_Time curTime;
            int mediaProbeTime;
            B_Time_Get(&curTime);
            mediaProbeTime = B_Time_Diff(&curTime, &playback_ip->mediaProbeStartTime);
            if (mediaProbeTime >= playback_ip->setupSettings.u.http.psiParsingTimeLimit) {
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: media probe time %d exceeded user specified limit %d", __FUNCTION__, mediaProbeTime, playback_ip->setupSettings.u.http.psiParsingTimeLimit));
#endif
                rc = -1;
                goto error;
            }
        }
    }

    if (!playback_ip->indexCacheInit) {
        /* initialize index cache */
        /* lower the default select timeout to 1 sec */
        if (playback_ip->networkTimeout == HTTP_SELECT_TIMEOUT) {
            BDBG_MSG(("%s: lowering default select timeout from %d to 1 sec\n", __FUNCTION__, playback_ip->networkTimeout));
            playback_ip->networkTimeout = 1;
        }
        if (playback_ip->indexAtEnd) {
            /* 2nd seek operation determines if index is at the end. */ 
            /* In that case, we need to setup a separate connection for getting the index data */ 
            /* if contentLength is not known, set rangeEnd to empty */
            file->socketError = false;
            playback_ip->serverClosed = false;
            if (playback_ip->contentLength)
                rangeEnd = playback_ip->contentLength -1;
            else
                rangeEnd = 0;
            rc = B_PlaybackIp_HttpNetRangeReq(playback_ip, playback_ip->indexCache, HTTP_INDEX_CACHE_CHUNK_SIZE, byteRangeStart, rangeEnd, playback_ip->socketState.fd, &playback_ip->socketState.fd);
            if (rc < 0) {
                BDBG_ERR(("%s: ERROR: Couldn't setup socket for reading when index data is towards end, bytes read %d, length %u\n", 
                            __FUNCTION__, rc, length));
                file->socketError = true;
                playback_ip->serverClosed = false;
                /* set flag to indicate data flow to re-open the socket */
                playback_ip->reOpenSocket = true;
                goto error;
            }
            if (rc != HTTP_INDEX_CACHE_CHUNK_SIZE) {
                BDBG_MSG(("%s: couldn't read (%d) all the requested bytes (%d) from socket due to n/w error or EOF, server closed %d\n", 
                            __FUNCTION__, rc, HTTP_INDEX_CACHE_CHUNK_SIZE, playback_ip->serverClosed));
            }
            playback_ip->indexCacheDepth = rc;
            playback_ip->indexCacheWriteIndex = playback_ip->indexCacheDepth;
            playback_ip->indexCacheStartOffset = byteRangeStart;
            playback_ip->indexCacheEndOffset = byteRangeStart + rc - 1;
            playback_ip->indexCacheReadIndex = 0;
            /* set flag to indicate data flow to re-open the socket */
            playback_ip->reOpenSocket = true;
            file->fd = playback_ip->socketState.fd;
            BDBG_MSG(("%s: Index at end, opened separate connx for index, start off %lld, end %lld, depth %u, wr idx %u\n",
                        __FUNCTION__, playback_ip->indexCacheStartOffset, 
                        playback_ip->indexCacheEndOffset, playback_ip->indexCacheDepth, playback_ip->indexCacheWriteIndex));
            playback_ip->indexCacheInit = 1;
        } else if (playback_ip->initial_data_len) {
            /* some index data got read part of reading the HTTP response, copy that data into icache & initialize cache state variables */
            memcpy(playback_ip->indexCache, playback_ip->temp_buf, playback_ip->initial_data_len);
            playback_ip->indexCacheDepth = playback_ip->initial_data_len;
            playback_ip->indexCacheEndOffset = playback_ip->indexCacheDepth -1;
            playback_ip->indexCacheWriteIndex = playback_ip->indexCacheDepth;
            bytesRead = playback_ip->initial_data_len;
            BDBG_MSG(("%s: Copied initial payload of %d bytes into the index cache\n", __FUNCTION__, playback_ip->initial_data_len));
            playback_ip->initial_data_len = 0;
            playback_ip->indexCacheStartOffset = 0;
            playback_ip->indexCacheReadIndex = 0;
        }
        else {
            /* Initialize indexCache state variables. No index data was read part of initial HTTP response data */
            playback_ip->indexCacheDepth = 0; 
            playback_ip->indexCacheEndOffset = 0; 
            playback_ip->indexCacheWriteIndex = playback_ip->indexCacheDepth;
            playback_ip->initial_data_len = 0;
            playback_ip->indexCacheStartOffset = 0;
            playback_ip->indexCacheReadIndex = 0;
            BDBG_MSG(("%s: index cache is initialized with no initial payload data", __FUNCTION__, playback_ip->initial_data_len));
        }
        playback_ip->indexCacheInit = 1;
    }
    /* else case: index cache is already initialized */
    /* check if data cache is setup */
    else if (playback_ip->dataCache[0].inUse) {
        ssize_t totalOutLength;
        /* both index and data cache are setup */

        /* if requested range is completely in the index cache, then return the requested data */
        if (byteRangeStart >= playback_ip->indexCacheStartOffset && byteRangeEnd <= playback_ip->indexCacheEndOffset) {
            return http_read_data_from_index_cache(playback_ip, byteRangeStart, byteRangeEnd, buf, length);
        }
        /* requested range is not completely in the index cache, */

        if (playback_ip->psi.psiValid) {
            /* we have finished the media probe (psiValid is true), so all index related data must */
            /* be in the index cache. However, since we didn't find it in the index cache, either */
            /* the index data is interleved with AV data (true for MKV formats) or */
            /* scattered at different places. In either case, we go thru the data cache for this requested range */
            totalOutLength = 0;
            while ((size_t)totalOutLength < length) {
                if (playback_ip->playback_state == B_PlaybackIpState_eStopping || (playback_ip->playback_state == B_PlaybackIpState_eStopped)) {
                    BDBG_MSG(("%s: breaking out of read loop due to state %d change (stop event)", __FUNCTION__, playback_ip->playback_state));
                    rc = -1;
                    goto error;
                }
                if ((rc = http_read_data_from_data_cache(playback_ip, file, byteRangeStart, byteRangeEnd, buf, length)) <= 0) {
                    BDBG_WRN(("%s: rc (%d)<= 0, failed to read index data via the data cache", __FUNCTION__, rc));
                    totalOutLength = rc;
                    break;
                }
                totalOutLength += rc;
                BDBG_MSG(("%s: read %d bytes from data_cache, total read so far %d, remaining to read %d", __FUNCTION__, rc, totalOutLength, length));
            }
            return totalOutLength;
        }
        else {
            BDBG_MSG(("%s: requested range is neither in index or data cache, since probing is still going on, go thru index cache", __FUNCTION__));
        }
    }

    /* index cache is already setup and data cache is not yet setup, that means we are still in the media probe phase, continue building up index cache */
    if (byteRangeStart < playback_ip->indexCacheStartOffset || (byteRangeStart > (playback_ip->indexCacheEndOffset + playback_ip->indexCacheSize)) || playback_ip->reOpenIndexSocket ) {
        /* requested byte range start is not cacheable, so modify the servers sending range */
        if (playback_ip->setupSettings.u.http.disableRangeHeader) {
            BDBG_ERR(("%s: ERROR: Can't seek when Range Header is not supported\n", __FUNCTION__));
            return -1;
        }
        file->socketError = false;
        playback_ip->serverClosed = false;
        rc = B_PlaybackIp_HttpNetRangeReq(playback_ip, playback_ip->indexCache, HTTP_INDEX_CACHE_CHUNK_SIZE, byteRangeStart, 0, playback_ip->socketState.fd, &playback_ip->socketState.fd);
        if (rc < 0) {
            BDBG_ERR(("%s: ERROR: Couldn't setup socket for reading when index data is towards end, bytes read %d, length %u\n", __FUNCTION__, rc, length));
            file->socketError = true;
            playback_ip->serverClosed = false;
            playback_ip->reOpenSocket = true;
            goto error;
        }
        if (rc != HTTP_INDEX_CACHE_CHUNK_SIZE) {
            BDBG_MSG(("%s: couldn't read (%d) all the requested bytes (%d) from socket due to n/w error or EOF, server closed %d\n", 
                        __FUNCTION__, rc, HTTP_INDEX_CACHE_CHUNK_SIZE, playback_ip->serverClosed));
            playback_ip->serverClosed = false;
            playback_ip->reOpenSocket = true;
        }
        playback_ip->reOpenIndexSocket = false;
        playback_ip->indexCacheDepth = rc;
        playback_ip->indexCacheWriteIndex = rc;
        playback_ip->indexCacheStartOffset = byteRangeStart;
        playback_ip->indexCacheEndOffset = byteRangeStart + rc - 1;
        playback_ip->indexCacheReadIndex = 0;
        /* set flag to indicate data flow to re-open the socket */
        playback_ip->reOpenSocket = true;
        file->fd = playback_ip->socketState.fd;
        BDBG_MSG(("%s: Index range not cacheable, re-requested new range , start off %lld, end %lld, depth %u, wr idx %u\n",
                    __FUNCTION__, playback_ip->indexCacheStartOffset, 
                    playback_ip->indexCacheEndOffset, playback_ip->indexCacheDepth, playback_ip->indexCacheWriteIndex));
    }

    while (byteRangeEnd > playback_ip->indexCacheEndOffset && file->socketError == false) {
        /* requested range is not in the index cache, so try to read more data from server */
        BDBG_MSG(("%s: iCache miss count %d, range end %lld, cache end %lld\n", __FUNCTION__, cacheMissCount, byteRangeEnd, playback_ip->indexCacheEndOffset));
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || (playback_ip->playback_state == B_PlaybackIpState_eStopped)) {
            BDBG_MSG(("%s: breaking out of read loop due to state %d change (stop event)", __FUNCTION__, playback_ip->playback_state));
            rc = -1;
            goto error;
        }

        /* since we always try to read fixed chunk size data into icache, see if some data was already read part of the initial HTTP resp processing */
        if (bytesRead) {
            bytesToRead = HTTP_INDEX_CACHE_CHUNK_SIZE - bytesRead;
        }
        else {
            bytesToRead = HTTP_INDEX_CACHE_CHUNK_SIZE;
        }
        
        /* adjust bytesToRead: read lower of: 1) index chunk size, 2) space available at end of cache (size-wrIndex), 3) actual space left in the cache (size-depth) */
        spaceAvailableAtEnd = playback_ip->indexCacheSize - playback_ip->indexCacheWriteIndex;
        spaceLeftInCache = playback_ip->indexCacheSize - playback_ip->indexCacheDepth;
        if (spaceAvailableAtEnd < bytesToRead || (spaceLeftInCache && spaceLeftInCache < bytesToRead)) {
            bytesToRead = spaceAvailableAtEnd > spaceLeftInCache ? spaceLeftInCache : spaceAvailableAtEnd;
            BDBG_MSG(("%s: reached towards end of index cache or cache getting full, so reading (%d) less than chunk size (%d), cache size %d, cache wr idx %d, depth %d, spaceAvailAtEnd %d, spaceLeftInCache %d",
                    __FUNCTION__, bytesToRead, HTTP_INDEX_CACHE_CHUNK_SIZE, playback_ip->indexCacheSize, playback_ip->indexCacheWriteIndex,
                    playback_ip->indexCacheDepth, spaceAvailableAtEnd, spaceLeftInCache));
        }
        /* if index cache is full, we retire the oldest entry from the cache */
        if (playback_ip->indexCacheDepth == playback_ip->indexCacheSize && !file->socketError) {
            playback_ip->indexCacheDepth -= HTTP_INDEX_CACHE_CHUNK_SIZE;
            playback_ip->indexCacheStartOffset += HTTP_INDEX_CACHE_CHUNK_SIZE;
            playback_ip->indexCacheReadIndex += HTTP_INDEX_CACHE_CHUNK_SIZE;
            playback_ip->indexCacheReadIndex %= playback_ip->indexCacheSize;
            BDBG_MSG(("%s: icache wrap: depth %d, size %d, rd idx %d, offset start %lld, end %lld\n", 
                        __FUNCTION__, playback_ip->indexCacheDepth, playback_ip->indexCacheSize, playback_ip->indexCacheReadIndex,
                        playback_ip->indexCacheStartOffset, playback_ip->indexCacheEndOffset));

        }
        if (playback_ip->indexCacheDepth > playback_ip->indexCacheSize) {
            BDBG_MSG(("%s: icache pointers bug: depth %d, size %d, rd idx %d, offset start %lld, end %lld\n", 
                        __FUNCTION__, playback_ip->indexCacheDepth, playback_ip->indexCacheSize, playback_ip->indexCacheReadIndex,
                        playback_ip->indexCacheStartOffset, playback_ip->indexCacheEndOffset));
            BDBG_ASSERT(NULL);
            goto error;
        }

        rc = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, file->fd, playback_ip->indexCache + playback_ip->indexCacheWriteIndex, bytesToRead, playback_ip->networkTimeout);
        if (rc <= 0 ) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_ERR(("%s: read failed, rc %d, Network timeout (%d) or EOF (%d)\n", __FUNCTION__, rc, playback_ip->selectTimeout, playback_ip->serverClosed));
#endif
            if (playback_ip->selectTimeout && !playback_ip->serverClosed) {
                /* this is a hack just to appease the broken DLNA CTT server as it doesn't close the connection after it done sending data */
                if (playback_ip->setupSettings.u.http.disableRangeHeader) {
                    BDBG_ERR(("%s: ERROR: Can't seek when Range Header is not supported\n", __FUNCTION__));
                    return -1;
                }
                BDBG_MSG(("select timeout, so retry read\n"));
                continue;
            }
            file->socketError = true;
            playback_ip->serverClosed = false;
            /* set flag to indicate data flow to re-open the socket */
            playback_ip->reOpenSocket = true;
            goto error;
        }
        if (rc != bytesToRead) {
            BDBG_MSG(("%s: couldn't read (%d) all the requested bytes (%d) from socket due to n/w timeout (%d) or EOF (%d)\n", __FUNCTION__, rc, HTTP_INDEX_CACHE_CHUNK_SIZE, playback_ip->selectTimeout, playback_ip->serverClosed));
            bytesRead += rc;
            if (!playback_ip->selectTimeout) {
                /* only consider this as an error if select hasn't timed out */
                file->socketError = true;
                /* set flag to indicate data flow to re-open the socket */
                playback_ip->reOpenSocket = true;
                playback_ip->serverClosed = false;
            }
        }
        else {
            bytesRead = 0;
        }
        if (fclear) {
            fwrite(playback_ip->indexCache + playback_ip->indexCacheWriteIndex, 1, rc , fclear); 
            fflush(fclear);
        }
        playback_ip->indexCacheDepth += rc;
        if (playback_ip->indexCacheEndOffset)
            playback_ip->indexCacheEndOffset += rc;
        else
            playback_ip->indexCacheEndOffset = rc-1;
        playback_ip->indexCacheWriteIndex += rc;
        playback_ip->indexCacheWriteIndex %= playback_ip->indexCacheSize;
        BDBG_MSG(("%s: read %d bytes into index cache of depth %d, size %d bytes, next wr index %d\n", 
                    __FUNCTION__, rc, playback_ip->indexCacheDepth, playback_ip->indexCacheSize, playback_ip->indexCacheWriteIndex));
        cacheMissCount++;
    }

    if (file->socketError == true) {
        /* server has closed connection, check if the requested range is with-in cache */
        if (byteRangeStart > playback_ip->indexCacheEndOffset) {
            BDBG_MSG(("%s: requested range is not in index: range start %llu, end %llu, cache depth %d, end off %lld, length %u\n", 
                    __FUNCTION__, byteRangeStart, byteRangeEnd, playback_ip->indexCacheDepth, playback_ip->indexCacheEndOffset, length));
            rc = 0;
            goto error;
        }
        /* so we know atleast the byteRangeStart is in the cache, trim the byteRangeEnd if it is outside the Cache */
        if (byteRangeEnd > playback_ip->indexCacheEndOffset) {
            byteRangeEnd = playback_ip->indexCacheEndOffset;
            length = byteRangeEnd - byteRangeStart + 1;
            BDBG_MSG(("%s: Server has already closed connection, so resetting byteRangeEnd to %lld, length to %u\n", __FUNCTION__, byteRangeEnd, length));
        }
    }

    if (byteRangeEnd <= playback_ip->indexCacheEndOffset) {
        return http_read_data_from_index_cache(playback_ip, byteRangeStart, byteRangeEnd, buf, length);
    }
    else {
        BDBG_WRN(("%s: SW bug: requested range is not in index: range start %llu, end %llu, cache depth %d, end off %lld, length %u\n", 
                    __FUNCTION__, byteRangeStart, byteRangeEnd, playback_ip->indexCacheDepth, playback_ip->indexCacheEndOffset, length));
        rc = -1;
    }

error:
    return rc;
}

/* allocate memory for caches */
static int 
_http_av_cache_alloc(B_PlaybackIpAvCache *avCache, int size)
{
    avCache->cache = (char *)BKNI_Malloc(size);
    if (avCache->cache == NULL) {
        BDBG_ERR(("%s: Failed to allocate data cache of %d bytes\n", __FUNCTION__, size));
        return -1;
    }
    BDBG_MSG(("%s: Allocated %d byte size data cache, buf %p", __FUNCTION__, size, avCache->cache));
    avCache->size = size;
    avCache->depth = 0;
    return 0;
}

void
http_av_cache_dealloc(B_PlaybackIpHandle playback_ip)
{
    int i;
    for (i=0; i<HTTP_MAX_DATA_CACHES; i++) {
        if (playback_ip->dataCache[i].cache) {
            BKNI_Free(playback_ip->dataCache[i].cache);
            playback_ip->dataCache[i].cache = NULL;
        }
    }
    if (playback_ip->indexCache) {
        BKNI_Free(playback_ip->indexCache);
        playback_ip->indexCache = NULL;
    }
    BDBG_MSG(("%s: Freed Data & Index Caches\n", __FUNCTION__));
}

B_PlaybackIpError 
http_av_cache_alloc(B_PlaybackIpHandle playback_ip)
{
    int i;
    int cacheSize;
    /* allocate data cache */
    if (playback_ip->openSettings.u.http.networkBufferSize > HTTP_DATA_CACHE_SIZE) {
        /* increase the user requested cache size to the next multiple of data chunk size */
        cacheSize = playback_ip->openSettings.u.http.networkBufferSize + (HTTP_DATA_CACHE_CHUNK_SIZE - (playback_ip->openSettings.u.http.networkBufferSize%HTTP_DATA_CACHE_CHUNK_SIZE));
    }
    else 
        cacheSize = HTTP_DATA_CACHE_SIZE;
    BDBG_MSG(("asked data cache size %d, new size %d\n", playback_ip->openSettings.u.http.networkBufferSize, cacheSize));
    for (i=0; i<HTTP_MAX_DATA_CACHES; i++) {
        if (_http_av_cache_alloc(&playback_ip->dataCache[i], cacheSize) < 0) {
            BDBG_ERR(("%s: Failed to create data cache # %d\n", __FUNCTION__, i));
            goto error;
        }
        playback_ip->dataCache[i].inUse = false; 
    }

    /* allocate index cache */
    if (playback_ip->openSettings.u.http.networkBufferSize > HTTP_INDEX_CACHE_SIZE) {
        /* increase the user requested cache size to the next multiple of index chunk size */
        cacheSize = playback_ip->openSettings.u.http.networkBufferSize + (HTTP_INDEX_CACHE_CHUNK_SIZE - (playback_ip->openSettings.u.http.networkBufferSize%HTTP_INDEX_CACHE_CHUNK_SIZE));
    }
    else 
        cacheSize = HTTP_INDEX_CACHE_SIZE;
    BDBG_MSG(("asked index cache size %d, new size %d\n", playback_ip->openSettings.u.http.networkBufferSize, cacheSize));
    playback_ip->indexCacheSize = cacheSize;
    playback_ip->indexCache = (char *)BKNI_Malloc(cacheSize);
    if (playback_ip->indexCache == NULL) {
        BDBG_ERR(("%s: Failed to allocate index cache of %d bytes\n", __FUNCTION__, playback_ip->indexCacheSize));
        goto error;
    }
    BDBG_MSG(("%s: Allocated %d byte size index cache", __FUNCTION__, playback_ip->indexCacheSize));
    playback_ip->indexCacheDepth = 0;

    return B_ERROR_SUCCESS;
error:
    http_av_cache_dealloc(playback_ip);
    return B_ERROR_OUT_OF_MEMORY;
}

void 
B_PlaybackIp_HttpSessionClose(B_PlaybackIpHandle playback_ip)
{
    int i;
    BDBG_MSG(("%s: ", __FUNCTION__));
    if (playback_ip->file) {
        BKNI_Free(playback_ip->file);
        playback_ip->file = NULL;
    }
    if (playback_ip->fileBuffered) {
        bfile_buffered_detach(playback_ip->fileBuffered);
        bfile_buffered_destroy(playback_ip->fileBuffered);
        playback_ip->fileBuffered = NULL;
    }
    if (playback_ip->factory) {
        batom_factory_destroy(playback_ip->factory);
        playback_ip->factory = NULL;
    }
    if (playback_ip->securityHandle) {
        playback_ip->netIo.close(playback_ip->securityHandle);
        playback_ip->securityHandle = NULL;
    }
    for (i=0; i<HTTP_MAX_DATA_CACHES; i++) {
        playback_ip->dataCache[i].inUse = false;
    }

    http_av_cache_dealloc(playback_ip);

    if (playback_ip->responseMessage) {
        BKNI_Free(playback_ip->responseMessage);
        playback_ip->responseMessage = NULL;
    }

    playback_ip->netIo.read = NULL;
    playback_ip->netIo.write = NULL;
    playback_ip->netIo.close = NULL;
    playback_ip->netIo.suspend = NULL;
    playback_ip->netIo.shutdown = NULL;
    playback_ip->openSettings.security.securityProtocol = B_PlaybackIpSecurityProtocol_None;

    playback_ip->indexCacheInit = 0;
    playback_ip->indexSeekCount = 0;
    playback_ip->indexAtEnd = false;
    playback_ip->contentLength = 0;
    playback_ip->preChargeBuffer = false;
    playback_ip->lastUsedCacheIndex = 0;
    playback_ip->cacheIndexUsingSocket = 0;
    playback_ip->serverClosed = false;
    playback_ip->socketClosed = false;
    playback_ip->reOpenSocket = false;
    playback_ip->tempBytesRead = 0;
    playback_ip->lastSeekOffset = 0;
    /* coverity[missing_lock] */
    playback_ip->cacheDepthFudgeFactor = 0;
    playback_ip->statusCode = 0;
    playback_ip->firstDataReadCallbackDone = false;
    playback_ip->waveHeaderInserted = false;
    playback_ip->indexCacheEndOffset = 0;

    if (playback_ip->stream)
        bmedia_probe_stream_free(playback_ip->probe, playback_ip->stream);
    if (playback_ip->probe)
        bmedia_probe_destroy(playback_ip->probe);

    if (playback_ip->httpSessionSetupThread) {
        /* destroy thread that was created during SessionSetup */
        /* it is not destroyed during sessionSetup completion as app can re-invoke sessionSetup in the callback itself */
        B_Thread_Destroy(playback_ip->httpSessionSetupThread);
        playback_ip->httpSessionSetupThread = NULL;
        BDBG_MSG(("%s: destroying temporary thread created during HTTP session setup", __FUNCTION__));
    }
    if (playback_ip->openSettings.u.http.additionalHeaders) {
        free(playback_ip->openSettings.u.http.additionalHeaders);
        playback_ip->openSettings.u.http.additionalHeaders = NULL;
    }
    if (playback_ip->openSettings.u.http.userAgent) {
        free(playback_ip->openSettings.u.http.userAgent);
        playback_ip->openSettings.u.http.userAgent = NULL;
    }
    if (playback_ip->cookieFoundViaHttpRedirect)
        BKNI_Free(playback_ip->cookieFoundViaHttpRedirect);
#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    if (playback_ip->hlsSessionEnabled)
        B_PlaybackIp_HlsSessionDestroy(playback_ip);
#endif
#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
    if (playback_ip->mpegDashSessionEnabled)
        B_PlaybackIp_MpegDashSessionDestroy(playback_ip);
#endif
    BDBG_MSG(("%s: Done", __FUNCTION__));
}

void B_PlaybackIp_HttpNetClose(
    NEXUS_FilePlayHandle file
    )
{
    struct bfile_in_net *filePlay = (struct bfile_in_net *)file;

    BDBG_MSG(("%s: \n", __FUNCTION__));
    B_PlaybackIp_HttpSessionClose(filePlay->data.playback_ip);
    return;
}

static const struct bfile_io_read net_io_data_read = { 
    B_PlaybackIp_HttpNetDataRead,
    B_PlaybackIp_HttpNetDataSeek,
    B_PlaybackIp_HttpNetDataBounds,
    BIO_DEFAULT_PRIORITY
};

static const struct bfile_io_read net_io_index_read = { 
    B_PlaybackIp_HttpNetIndexRead,
    B_PlaybackIp_HttpNetIndexSeek,
    B_PlaybackIp_HttpNetIndexBounds,
    BIO_DEFAULT_PRIORITY
};

void 
B_PlaybackIp_HttpDestroyFilePlayHandle(
    B_PlaybackIpHandle playback_ip
    )
{
    if (playback_ip->file) {
        BKNI_Free(playback_ip->file);
        playback_ip->file = NULL;
    }
    if (playback_ip->fileBuffered) {
        bfile_buffered_detach(playback_ip->fileBuffered);
        bfile_buffered_destroy(playback_ip->fileBuffered);
        playback_ip->fileBuffered = NULL;
    }
    if (playback_ip->factory) {
        batom_factory_destroy(playback_ip->factory);
        playback_ip->factory = NULL;
    }

    http_av_cache_dealloc(playback_ip);
}

NEXUS_FilePlayHandle 
B_PlaybackIp_HttpCreateFilePlayHandle(
    B_PlaybackIpHandle playback_ip,
    bfile_io_read_t *bufferedFdIndex
    )
{
    struct bfile_in_net *filePlay;
    bfile_buffered_cfg file_buffered_cfg;

    filePlay = BKNI_Malloc(sizeof(*filePlay));
    if (!filePlay) {
        BDBG_ERR(("%s: memory allocation failure\n", __FUNCTION__));
        return NULL;
    }
    BKNI_Memset(filePlay, 0, sizeof(*filePlay));
    BDBG_MSG(("%s: playback_ip %p, filePlay %p, data %p, index %p, size of file struct %d\n", 
                __FUNCTION__, playback_ip, filePlay, &filePlay->data, &filePlay->index, sizeof(*filePlay)));

    filePlay->data.fd = playback_ip->socketState.fd;
    filePlay->data.playback_ip = playback_ip;
    filePlay->data.socketError = false;
    filePlay->data.self = net_io_data_read;
    filePlay->self.file.data = &(filePlay->data.self);

    playback_ip->file = filePlay;
    filePlay->index.fd = playback_ip->socketState.fd;
    filePlay->index.playback_ip = playback_ip;
    filePlay->index.socketError = false;
    filePlay->index.self = net_io_index_read;
    filePlay->self.file.index = &(filePlay->index.self);
    filePlay->self.file.close = B_PlaybackIp_HttpNetClose;
    /* create buffered cache for index so that this gets cached inside the player */
    playback_ip->factory = batom_factory_create(bkni_alloc, 256);
    BDBG_ASSERT(playback_ip->factory);

    bfile_buffered_default_cfg(&file_buffered_cfg);
    file_buffered_cfg.nsegs = 128;
    file_buffered_cfg.buf_len = file_buffered_cfg.nsegs * (4096*4);
    playback_ip->fileBuffered = bfile_buffered_create(playback_ip->factory, &file_buffered_cfg);
    BDBG_ASSERT(playback_ip->fileBuffered);

    *bufferedFdIndex = bfile_buffered_attach(playback_ip->fileBuffered, &(filePlay->index.self));
    BDBG_ASSERT(*bufferedFdIndex);
    filePlay->self.file.index = *bufferedFdIndex;

    if (http_av_cache_alloc(playback_ip) != B_ERROR_SUCCESS)
        goto error;
    return &filePlay->self;

error:
    http_av_cache_dealloc(playback_ip);
    if (filePlay) {
        BKNI_Free(filePlay);
    }
    if (playback_ip->fileBuffered) {
        bfile_buffered_detach(playback_ip->fileBuffered);
        bfile_buffered_destroy(playback_ip->fileBuffered);
        playback_ip->fileBuffered = NULL;
    }
    if (playback_ip->factory){
        batom_factory_destroy(playback_ip->factory);
        playback_ip->factory = NULL;
    }
    return (NULL);
}

void
B_PlaybackIp_HttpParseRespHeaderForPsi(
    B_PlaybackIpHandle playback_ip, 
    NEXUS_TransportType http_content_type,
    char *http_hdr,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    char *p_psi;
    char *end_str;
	BSTD_UNUSED(playback_ip);

    memset(psi, 0, sizeof(B_PlaybackIpPsiInfo));
    p_psi = B_PlaybackIp_UtilsStristr(http_hdr, "FrameRateInTrickMode.dlna.org:");
    if (p_psi) {
        p_psi += strlen("FrameRateInTrickMode.dlna.org:");
        end_str = strstr(p_psi, "\r\n");
        if (end_str) {
            char *tmp1 = NULL, *tmp2 = NULL;
            end_str[0] = '\0';
            if ( (tmp1 = B_PlaybackIp_UtilsStristr(p_psi, "rate=")) != NULL || (tmp2 = B_PlaybackIp_UtilsStristr(p_psi, "rate =")) != NULL) {
                if (tmp1) p_psi = tmp1 + strlen("rate=");
                else if (tmp2) p_psi = tmp2 + strlen("rate =");
                psi->frameRateInTrickMode = strtol(p_psi, (char **)NULL, 10);
            }
            end_str[0] = '\r';
            BDBG_MSG(("FrameRateInTrickMode.dlna.org: rate= %lu\n",psi->frameRateInTrickMode));
        }
    }

    p_psi = B_PlaybackIp_UtilsStristr(http_hdr, "DLNA.ORG_PS=");
    if (p_psi) {
        char playSpeed[256] = {0,};
        char *p_end, *p_start;
        int ps_count = 0;

        p_psi += strlen("DLNA.ORG_PS=");
        if(strlen(p_psi) > 255) {
            BDBG_MSG(("playSpeeds are too many! Truncate!"));
            *(p_psi + 255) = 0;
        }
        end_str = strstr(p_psi, "\r\n");
        if (end_str) {
            end_str[0] = '\0';
            strncpy(playSpeed, p_psi, 255);
            end_str[0] = '\r';
            BDBG_MSG(("playSpeed : %s\n",playSpeed));
        }

        /* parse and store play speed set */
        p_start = playSpeed;
        while((p_end = strstr(playSpeed, ",")) != NULL) {
            *p_end = 0;
            BDBG_MSG(("PS[%d] = %s", ps_count, p_start));
            psi->playSpeed[ps_count++] = strtol(p_start, (char **)NULL, 10);
            *p_end = ' ';
            p_start = p_end; /* next */
            if(ps_count >= IP_PVR_PLAYSPEED_MAX_COUNT-1)
                break;
        }
        if(*p_start != '\0') {
            psi->playSpeed[ps_count++] = strtol(p_start, (char **)NULL, 10);
            BDBG_MSG(("PS[%d] = %d", ps_count-1, psi->playSpeed[ps_count-1]));
        }
        psi->numPlaySpeedEntries = ps_count;
        BDBG_MSG(("Server trickmodes supported: # of speeds %d", psi->numPlaySpeedEntries));
    }

    if (http_content_type == NEXUS_TransportType_eTs) {
        psi->mpegType = NEXUS_TransportType_eTs;
        BDBG_MSG(("HTTP Content Type: MPEG2 TS\n"));
        psi->psiValid = false;

        p_psi = strstr(http_hdr,"BCM-LiveChannel: ");
        if (p_psi)
        {
            p_psi += strlen("BCM-LiveChannel: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                int tmp;
                end_str[0] = '\0';
                tmp = strtol(p_psi, (char **)NULL, 10);
                if (tmp > 0)
                    psi->liveChannel = true;
                else
                    psi->liveChannel = false;
                end_str[0] = '\r';
                BDBG_MSG(("BCM-LiveChannel: %d",psi->liveChannel));
            }
        }
        else
            psi->liveChannel = false;

        p_psi = strstr(http_hdr,"Video-Pid: ");
        if (p_psi) {
            p_psi += strlen("Video-Pid: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->videoPid = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("video-PID: %d\n",psi->videoPid));
            }
        }

        p_psi = strstr(http_hdr,"Video-Type: ");
        if (p_psi)
        {
            p_psi += strlen("Video-Type: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                int videoCodec;
                end_str[0] = '\0';
                videoCodec = (NEXUS_VideoCodec)strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                if (strstr(http_hdr,"SDK/1.0"))
                    /* older streamer "httpstreamer" (SDK/1.0) uses Settop API types, so we need to convert it to Nexus types */
                    psi->videoCodec = B_PlaybackIp_UtilsVideoCodec2Nexus(videoCodec);
                else
                    /* new streamer "ip_streamer" SDK/2.0 uses Nexus types, so we dont any conversion */
                    psi->videoCodec = videoCodec;
                BDBG_MSG(("video-Type: %d\n",psi->videoCodec));
                psi->psiValid = true;
            }
        }

        p_psi = strstr(http_hdr,"Extra-Video-Pid: ");
        if (p_psi) {
            p_psi += strlen("Extra-Video-Pid: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->extraVideoPid = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("extra-video-PID: %d\n",psi->extraVideoPid));
            }
        }

        p_psi = strstr(http_hdr,"Extra-Video-Type: ");
        if (p_psi)
        {
            p_psi += strlen("Extra-Video-Type: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                int videoCodec;
                end_str[0] = '\0';
                videoCodec = (NEXUS_VideoCodec)strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                if (strstr(http_hdr,"SDK/1.0"))
                    /* older streamer "httpstreamer" (SDK/1.0) uses Settop API types, so we need to convert it to Nexus types */
                    psi->extraVideoCodec = B_PlaybackIp_UtilsVideoCodec2Nexus(videoCodec);
                else
                    /* new streamer "ip_streamer" SDK/2.0 uses Nexus types, so we dont any conversion */
                    psi->extraVideoCodec = videoCodec;
                BDBG_MSG(("extra-video-Type: %d\n",psi->extraVideoCodec));
            }
        }

        p_psi = strstr(http_hdr,"Video-Width: ");
        if (p_psi) {
            p_psi += strlen("Video-Width: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->videoWidth = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("video-width: %d\n",psi->videoWidth));
            }
        }

        p_psi = strstr(http_hdr,"Video-Height: ");
        if (p_psi) {
            p_psi += strlen("Video-Height: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->videoHeight = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("video-height: %d\n",psi->videoHeight));
            }
        }

        p_psi = strstr(http_hdr,"Pcr-Pid: ");
        if (p_psi)
        {
            p_psi += strlen("Pcr-Pid: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->pcrPid= strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("pcr-PID: %d\n",psi->pcrPid));
            }
        }

        p_psi = strstr(http_hdr,"Audio-Pid: ");
        if (p_psi)
        {
            p_psi += strlen("Audio-Pid: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->audioPid = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("audio-PID: %d\n",psi->audioPid));
            }
        }

        p_psi = strstr(http_hdr,"Audio-Type: ");
        if (p_psi)
        {
            p_psi += strlen("Audio-Type: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                int audioCodec;
                end_str[0] = '\0';
                audioCodec = (NEXUS_AudioCodec)strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                if (strstr(http_hdr,"SDK/1.0"))
                    /* older streamer "httpstreamer" (SDK/1.0) uses Settop API types, so we need to convert it to Nexus types */
                    psi->audioCodec = B_PlaybackIp_UtilsAudioCodec2Nexus(audioCodec);
                else
                    /* new streamer "ip_streamer" SDK/2.0 uses Nexus types, so we dont any conversion */
                    psi->audioCodec = audioCodec;
                BDBG_MSG(("audio-Type: %d\n",psi->audioCodec));
                psi->psiValid = true;
            }
        }

        p_psi = strstr(http_hdr,"TTS: ");
        if (p_psi)
        {
            p_psi += strlen("TTS: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->transportTimeStampEnabled = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("TTS: %d\n",psi->transportTimeStampEnabled));
            }
        }
        else
            psi->transportTimeStampEnabled = false;

        p_psi = strstr(http_hdr,"MediaDuration: ");
        if (p_psi)
        {
            p_psi += strlen("MediaDuration: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->duration = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("media-Duration[sec]: %d\n",psi->duration));
                psi->duration *= 1000; /* in msec */
            }
        }
        p_psi = strstr(http_hdr,"First-PTS: ");
        if (p_psi)
        {
            p_psi += strlen("First-PTS: ");
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->firstPts = strtoul(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("first-PTS: %lu\n",psi->firstPts));
            }
        }

        p_psi = strstr(http_hdr,"BCM-Min-Iframe-Speed: ");
        if (p_psi)
        {
            p_psi += strlen("BCM-Min-Iframe-Speed: ");
            psi->httpMinIFrameSpeed = 8;
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->httpMinIFrameSpeed = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("BCM-Min-Iframe-Speed: %lu\n",psi->httpMinIFrameSpeed));
            }
        }

        p_psi = strstr(http_hdr,"BCM-Frame-Repeat: ");
        if (p_psi)
        {
            p_psi += strlen("BCM-Frame-Repeat: ");
            psi->httpFrameRepeat=1;
            end_str = strstr(p_psi, "\r\n");
            if (end_str) {
                end_str[0] = '\0';
                psi->httpFrameRepeat = strtol(p_psi, (char **)NULL, 10);
                end_str[0] = '\r';
                BDBG_MSG(("BCM-Frame-Repeat: %lu\n",psi->httpFrameRepeat));
            }
        }
    }
}

#include "bmp4_util.h"
#define B_MP4_REF_MOV_DESC   BMP4_TYPE('r','m','d','a')
#define B_MP4_REF_DATA_REF   BMP4_TYPE('r','d','r','f')
#define B_MP4_REF_DATA_RATE   BMP4_TYPE('r','m','d','r')
typedef struct QuickTime_Rate_Atom {
    size_t size;
    unsigned type;
    unsigned flags;
    unsigned rate;
} QuickTime_Rate_Atom;

void 
parse_quicktime_ref_data_rate(char *buffer, unsigned *rate)
{
    QuickTime_Rate_Atom *dataRateAtom;

    dataRateAtom = (QuickTime_Rate_Atom *)buffer;
    BDBG_MSG(("%s: size %u, type 0x%x, flags %d, rate %u",
                __FUNCTION__, ntohl(dataRateAtom->size), ntohl(dataRateAtom->type), ntohl(dataRateAtom->flags), ntohl(dataRateAtom->rate)));
    *rate = ntohl(dataRateAtom->rate);
}

typedef struct QuickTime_Ref_Atom {
    size_t size;
    unsigned type;
    unsigned flags;
    unsigned refType;
    unsigned refSize;
    char url[];
} QuickTime_Ref_Atom;

void parse_quicktime_ref_data_ref(char *buffer, char **url)
{
    QuickTime_Ref_Atom *dataRefAtom;
    size_t size;

    dataRefAtom = (QuickTime_Ref_Atom *)buffer;
    size = ntohl(dataRefAtom->refSize);
    BDBG_MSG(("%s: size %u, type 0x%x, flags %d, refType 0x%x, url size %d, url %s", 
                __FUNCTION__, ntohl(dataRefAtom->size), ntohl(dataRefAtom->type), ntohl(dataRefAtom->flags), ntohl(dataRefAtom->refType), size, dataRefAtom->url));
    *url = dataRefAtom->url;
}

void 
parse_quicktime_ref_mov_desc(B_PlaybackIpHandle playback_ip, char *buffer, size_t length, char **url, unsigned *bitRate)
{
    batom_t atom;
    batom_cursor cursor;
    bmp4_box box;
    size_t boxHeaderSize;
    size_t bytesProcessed = 0;
    char *rmda;

    while (bytesProcessed < length) {
        rmda = buffer + bytesProcessed;
        atom = batom_from_range(playback_ip->factory, rmda, length-bytesProcessed, NULL, NULL);
        batom_cursor_from_atom(&cursor, atom);
        boxHeaderSize = bmp4_parse_box(&cursor, &box);
        BDBG_MSG(("%s: hdr size %u, type 0x%x, size %lld", __FUNCTION__, boxHeaderSize, box.type, box.size));
        switch (box.type) {
            case B_MP4_REF_DATA_REF:
                parse_quicktime_ref_data_ref(rmda, url);
                bytesProcessed += box.size;
                BDBG_MSG(("url %s", *url));
                break;
            case B_MP4_REF_DATA_RATE:
                parse_quicktime_ref_data_rate(rmda, bitRate);
                BDBG_MSG(("bitrate %d", *bitRate));
                bytesProcessed += box.size;
                break;
            default:
                bytesProcessed += box.size;
        }
        batom_release(atom);
    }
}

char *
parse_quicktime_redirects(B_PlaybackIpHandle playback_ip, char *buffer, size_t length)
{
    batom_t atom;
    batom_cursor cursor;
    bmp4_box box;
    size_t boxHeaderSize;
    size_t bytesProcessed = 0;
    char *url, *newUrl = NULL;
    unsigned bitRate, maxBitRate = 0;
    char *rmda;

    /* buffer points to the start of the 1st rmda header */
    while (bytesProcessed < length) {
        rmda = buffer + bytesProcessed;
        atom = batom_from_range(playback_ip->factory, rmda, length-bytesProcessed, NULL, NULL);
        batom_cursor_from_atom(&cursor, atom);
        boxHeaderSize = bmp4_parse_box(&cursor, &box);
        BDBG_MSG(("%s: hdr size %u, type 0x%x, size %lld", __FUNCTION__, boxHeaderSize, box.type, box.size));
        if (box.type != B_MP4_REF_MOV_DESC) {
            BDBG_WRN(("%s: Error: Invalid Reference Movie Atom as it doesn't contain Reference Movie Descriptor Atom"));
            return NULL;
        }
        parse_quicktime_ref_mov_desc(playback_ip, rmda+boxHeaderSize, box.size-boxHeaderSize, &url, &bitRate);
        BDBG_MSG(("Done parsing one RMDA atom, bitrate %d, url %s", bitRate, url));
        if (bitRate > maxBitRate) {
            maxBitRate = bitRate;
            newUrl = url;
            BDBG_MSG(("%s: bitRate %d, url %s", __FUNCTION__, maxBitRate, newUrl));
        }
        bytesProcessed += box.size;
        BDBG_MSG(("proc %d, len %d, size %d", bytesProcessed, length, box.size));
        batom_release(atom);
    }
    BDBG_MSG(("returning url %s", newUrl));
    return newUrl;
}

/* parses AV info from the HTTP response and sets the param structure for decoder configuration */
void 
http_parse_av_info(
    void *data
    )
{
    int rc = -1;
    B_PlaybackIpHandle playback_ip = (B_PlaybackIpHandle)data;
    B_PlaybackIpPsiInfoHandle psi = &playback_ip->psi; 
#ifdef B_HAS_HTTP_MP4_SUPPORT
    const bmedia_probe_track *track;
    bmedia_probe_config probe_config;
    char *stream_info = NULL;
#endif
    bfile_io_read_t bufferedFdIndex = playback_ip->bufferedFdIndex;
    B_PlaybackIpHttpSessionSetupSettings *setupSettings;
    char *http_hdr; 
    NEXUS_TransportType http_content_type;
    bool foundAudio = false, foundVideo = false;
    bool sessionTypeIsDetermined;

    setupSettings = &playback_ip->setupSettings.u.http;
    http_hdr = playback_ip->responseMessage;
    
    sessionTypeIsDetermined = false;

#define STREAM_INFO_SIZE 512
    if ((stream_info = BKNI_Malloc(STREAM_INFO_SIZE)) == NULL) {
        BDBG_ERR(("%s: memory allocation failure", __FUNCTION__));
        return;
    }

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    BDBG_MSG(("%s : %d : Calling B_PlaybackIp_HlsSessionSetup() ", __FUNCTION__, __LINE__   ));
    if (B_PlaybackIp_HlsSessionSetup(playback_ip, http_hdr) < 0) {
        BDBG_ERR(("%s: Error during HLS Session Probe & Setup", __FUNCTION__));
        goto error;
    }
    if (playback_ip->hlsSessionEnabled ) sessionTypeIsDetermined = true;
#endif

#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
    if (!sessionTypeIsDetermined ) {
        BDBG_MSG(("%s : %d : Calling B_PlaybackIp_MpegDashSessionSetup() ", __FUNCTION__, __LINE__   ));
        if (B_PlaybackIp_MpegDashSessionSetup(playback_ip, http_hdr) < 0) {
            BDBG_ERR(("%s: Error during MPEG-DASH Session Probe & Setup", __FUNCTION__));
            goto error;
        }
        if (playback_ip->mpegDashSessionEnabled ) sessionTypeIsDetermined = true;
    }
#endif

    http_content_type = playback_ip->http_content_type;

#ifdef B_HAS_HTTP_MP4_SUPPORT
redoProbe:
    /* Try to find PSI info using Media Probe Interface */
    playback_ip->probe = bmedia_probe_create();
    if (!playback_ip->probe) {
        BDBG_ERR(("%s: failed to create the probe object\n", __FUNCTION__));
        goto error;
    }
    bmedia_probe_default_cfg(&probe_config);

    probe_config.probe_es = false;
    probe_config.probe_payload = true;
    probe_config.probe_all_formats = false;
    if (setupSettings->enablePayloadScanning) {
        probe_config.probe_duration = true;
    }
    else {
        probe_config.probe_duration = false;
    }
    if (http_content_type == NEXUS_TransportType_eUnknown) {
        BDBG_MSG(("%s: HTTP Response header didn't provide any well known content type, using the hint %d", __FUNCTION__, setupSettings->contentTypeHint));
        if (setupSettings->contentTypeHint != NEXUS_TransportType_eUnknown) {
            http_content_type = setupSettings->contentTypeHint;
        }
    }
    switch (http_content_type) {
    case NEXUS_TransportType_eAsf:
        probe_config.file_name = "xxx.wmv";
        break;
    case NEXUS_TransportType_eTs:
        probe_config.file_name = "xxx.mpg";
        break;
    case NEXUS_TransportType_eMp4:
        probe_config.file_name = "xxx.mp4";
        break;
    case NEXUS_TransportType_eEs:
        probe_config.probe_es = true;
        probe_config.file_name = playback_ip->openSettings.socketOpenSettings.url;
        break;
    case NEXUS_TransportType_eFlv:
        probe_config.probe_es = false;
        probe_config.file_name = "xxx.flv";
        probe_config.probe_all_formats = true;
        break;
    default:
        probe_config.file_name = playback_ip->openSettings.socketOpenSettings.url;
        probe_config.probe_all_formats = true;
        probe_config.probe_es = true;
        break;
    }

    if (http_content_type != NEXUS_TransportType_eUnknown &&
        http_content_type != NEXUS_TransportType_eFlv)
        probe_config.type = B_PlaybackIp_UtilsNexus2Mpegtype(http_content_type);
    else 
        probe_config.type = bstream_mpeg_type_unknown;
#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    if (playback_ip->hlsSessionEnabled) {
        /* For HLS protocol, container format is restricted to TS and bitrate is already specified in the playlist file */
        /* so we dont need to carry out the detailed media probe */
        probe_config.probe_es = true;
        probe_config.probe_all_formats = true;
        probe_config.probe_payload = false;
        probe_config.file_name = "xxx.mpg";
        probe_config.type = B_PlaybackIp_UtilsNexus2Mpegtype(NEXUS_TransportType_eTs);
    }
#endif
#ifdef BDBG_DEBUG_BUILD
    if (playback_ip->ipVerboseLog)
        BDBG_WRN(("%s: Begin Probing Media for (content type %d) obtaining the PSI info (it may take some time ...)", __FUNCTION__, http_content_type));
#endif
    if (playback_ip->fileBuffered) {
        probe_config.buffer = bfile_buffered_get_buffer(playback_ip->fileBuffered);
    }
    playback_ip->stream = bmedia_probe_parse(playback_ip->probe, bufferedFdIndex, &probe_config);

    if (playback_ip->stream) {
        /* probe succeeded in finding a stream */
        bmedia_stream_to_string(playback_ip->stream, stream_info, STREAM_INFO_SIZE);
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_WRN(("media stream info: %s", stream_info));
#endif
        psi->mpegType = B_PlaybackIp_UtilsMpegtype2ToNexus(playback_ip->stream->type);

#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
        if (playback_ip->mpegDashSessionEnabled) {
            if (playback_ip->mpegDashSessionState && playback_ip->mpegDashSessionState->currentPlaylistFile && playback_ip->mpegDashSessionState->currentPlaylistFile->bounded) {
                psi->duration = playback_ip->mpegDashSessionState->currentPlaylistFile->totalDuration;
            }
            else {
                /* unbounded case, i.e. live stream, so set duration to large value */
                /* media browser needs the duration to know when to stop the playback */
                psi->duration = 9999999;
            }
            psi->mpegDashSessionEnabled = true;
        }
#endif
#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
        if (playback_ip->hlsSessionEnabled) {
            if (playback_ip->hlsSessionState && playback_ip->hlsSessionState->currentPlaylistFile && playback_ip->hlsSessionState->currentPlaylistFile->bounded) {
                psi->duration = playback_ip->hlsSessionState->currentPlaylistFile->totalDuration;
            }
            else {
                /* unbounded case, i.e. live stream, so set duration to large value */
                /* media browser needs the duration to know when to stop the playback */
                psi->duration = 9999999;
            }
            psi->hlsSessionEnabled = true;
        }
        else 
#endif
        if (playback_ip->stream->duration)
            psi->duration = playback_ip->stream->duration;
        psi->avgBitRate = playback_ip->stream->max_bitrate;
        if (psi->avgBitRate)
            psi->maxBufferDuration = (playback_ip->dataCache[0].size*8 / psi->avgBitRate)*1000;  /* in msec */
        else {
            BDBG_MSG(("%s: Warning: Media Probe couldn't determine the avg bitrate of stream!!!", __FUNCTION__));
            psi->avgBitRate = 0;
        }
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_WRN(("Media Details: container type %d, index %d, avg bitrate %d, duration %d, cache buffer in msec %lu, # of programs %d, # of tracks (streams) %d", 
                    psi->mpegType, playback_ip->stream->index, psi->avgBitRate, psi->duration, psi->maxBufferDuration, playback_ip->stream->nprograms, playback_ip->stream->ntracks));
#endif

        if ((setupSettings->dontUseIndex && playback_ip->stream->type != bstream_mpeg_type_mp4)
                /* App doesn't want to use the index info for all but MP4 files as some streams falsely indicate that it is present, but its really not */
                || playback_ip->stream->index == bmedia_probe_index_unknown /* true for VOBs & TS formats */
                || playback_ip->stream->index == bmedia_probe_index_missing    /* true for some WMA/WMV files w/o index */
           ) {
            struct bfile_in_net *filePlay;
            BDBG_MSG(("%s: Not setting index descriptor because %s for this content format %d, (probe index search code %d)", 
                        __FUNCTION__, (setupSettings->dontUseIndex?"app set dontUseIndex flag":"no index info is available"), psi->mpegType, playback_ip->stream->index));
            if (playback_ip->fileBuffered) {
                bfile_buffered_detach(playback_ip->fileBuffered);
                bfile_buffered_destroy(playback_ip->fileBuffered);
                playback_ip->fileBuffered = NULL;
            }
            if (playback_ip->factory) {
                batom_factory_destroy(playback_ip->factory);
                playback_ip->factory = NULL;
            }
            filePlay = (struct bfile_in_net *)playback_ip->nexusFileHandle;
            memset(&filePlay->index, 0, sizeof(filePlay->index));
            filePlay->self.file.index = NULL;
        }
        
#define QUICK_TIME_REDIRECT_SUPPORT
#ifdef QUICK_TIME_REDIRECT_SUPPORT
        /* check for quick time redirect streams */
        if (playback_ip->stream->type == bstream_mpeg_type_mp4 && ((((bmp4_probe_stream*)playback_ip->stream)->rmra.size) != 0)) {
            char *url;
            BDBG_MSG(("%s: QuickTime redirect stream: rmra size %lld, offset %lld", __FUNCTION__, ((bmp4_probe_stream*)playback_ip->stream)->rmra.size, ((bmp4_probe_stream*)playback_ip->stream)->rmra.offset));
            url = parse_quicktime_redirects(playback_ip, playback_ip->indexCache+((bmp4_probe_stream*)playback_ip->stream)->rmra.offset, ((bmp4_probe_stream*)playback_ip->stream)->rmra.size);
            if (url == NULL) {
                BDBG_ERR(("%s: ERROR: QuickTime redirect stream: however failed to retrieve a valid URL", __FUNCTION__));
            }
            else {
                /* separate server name from the URL */
                if (http_parse_url(playback_ip->openSettings.socketOpenSettings.ipAddr, &playback_ip->openSettings.socketOpenSettings.port, &playback_ip->openSettings.socketOpenSettings.url, url) != 0) {
                    BDBG_ERR(("%s: ERROR: Failed to parse server name, port, and uri fro the redirected URL %s", __FUNCTION__, url));
                    goto error;
                }
                BDBG_MSG(("%s: QuickTime redirect stream: server %s, port %d, uri %s", 
                            __FUNCTION__, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, playback_ip->openSettings.socketOpenSettings.url));

                /* reset index cache */
                playback_ip->indexCacheDepth = 0; 
                playback_ip->indexCacheEndOffset = 0; 
                playback_ip->indexCacheWriteIndex = playback_ip->indexCacheDepth;
                playback_ip->initial_data_len = 0;
                playback_ip->indexCacheStartOffset = 0;
                playback_ip->indexCacheReadIndex = 0;

                /* destroy and recreate the file handles */
                B_PlaybackIp_HttpDestroyFilePlayHandle(playback_ip);
                playback_ip->nexusFileHandle = B_PlaybackIp_HttpCreateFilePlayHandle(playback_ip, &playback_ip->bufferedFdIndex);
                if (playback_ip->nexusFileHandle == NULL) {
                    BDBG_ERR(("%s: Failed to Create the Playback Handles\n", __FUNCTION__));
                    goto error;
                }
                BDBG_MSG(("%s: re-created Nexus file IO handle %p", __FUNCTION__, playback_ip->nexusFileHandle));

                /* reconnect to server and send new HTTP request using new URL */
                rc = B_PlaybackIp_HttpNetRangeReq(playback_ip, playback_ip->indexCache, HTTP_INDEX_CACHE_CHUNK_SIZE, 0, 0, playback_ip->socketState.fd, &playback_ip->socketState.fd);
                if (rc < 0) {
                    BDBG_ERR(("%s: ERROR: Socket setup or HTTP request/response failed for QuickTime redirected URL", __FUNCTION__));
                    goto error;
                }
                if (rc > 0) {
                    /* setup the index cache w/ the initial data */
                    playback_ip->indexCacheInit = 1;
                    playback_ip->indexCacheDepth = rc;
                    playback_ip->indexCacheWriteIndex = playback_ip->indexCacheDepth;
                    playback_ip->indexCacheStartOffset = 0;
                    playback_ip->indexCacheEndOffset = rc - 1;
                    playback_ip->indexCacheReadIndex = 0;
                }
                BDBG_MSG(("%s: Connected to the server using the new URL, now redo the probe", __FUNCTION__));

                /* redo the probe */
                bmedia_probe_stream_free(playback_ip->probe, playback_ip->stream);
                bmedia_probe_destroy(playback_ip->probe);
                playback_ip->contentLength = 0;
                goto redoProbe;
            }
        }
        else 
#endif
            if (playback_ip->stream->type == bstream_mpeg_type_ts && ((((bmpeg2ts_probe_stream*)playback_ip->stream)->pkt_len) == 192)) {
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: TTS Stream\n", __FUNCTION__));
#endif
            psi->transportTimeStampEnabled = true;
        }
        else
            psi->transportTimeStampEnabled = false;
        rc = 0;
        psi->psiValid = true;
        for(track=BLST_SQ_FIRST(&playback_ip->stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
            if (track->type==bmedia_track_type_video && (track->info.video.codec == bvideo_codec_h264_svc || track->info.video.codec == bvideo_codec_h264_mvc)) {
                psi->extraVideoPid = track->number;
                psi->extraVideoCodec = B_PlaybackIp_UtilsVideoCodec2Nexus(track->info.video.codec);
                BDBG_MSG(("extra video track %u codec:%u\n", track->number, psi->extraVideoCodec));
                continue;
            }
            else if (track->type==bmedia_track_type_video && !foundVideo) {
                BDBG_MSG(("video track %u codec:%u\n", track->number, track->info.video.codec));
                psi->videoPid = track->number;
                psi->videoCodec = B_PlaybackIp_UtilsVideoCodec2Nexus(track->info.video.codec);
                psi->videoHeight = track->info.video.height;
                psi->videoWidth = track->info.video.width;
                foundVideo = true;
            }
            else if (track->type==bmedia_track_type_pcr) {
                BDBG_MSG(("pcr pid %u\n", track->number));
                psi->pcrPid = track->number;
            }
            else if(track->type==bmedia_track_type_audio && !foundAudio) {
                BDBG_MSG(("audio track %u codec:%u\n", track->number, track->info.audio.codec));
                psi->audioPid = track->number;
                psi->audioCodec = B_PlaybackIp_UtilsAudioCodec2Nexus(track->info.audio.codec);
                foundAudio = true;
            }
        }

        if (!psi->videoPid && !psi->audioPid) {
            BDBG_ERR(("%s: Video (%d) or Audio (%d) PIDs are not found during Media Probe\n", __FUNCTION__, psi->videoPid, psi->audioPid));
            goto error;
        }

        rc = 0;
        goto out;
    }
    else {
        /* probe didn't find the PSI info either, return error */
        BDBG_ERR(("%s: media probe didn't find the PSI info, return error", __FUNCTION__));
        goto error;
    }
#endif /* B_HAS_HTTP_MP4_SUPPORT */

error:
    BDBG_ERR(("%s: ERROR: Couldn't obtain PSI info from HTTP response or Media Probe\n", __FUNCTION__));
    psi->numPlaySpeedEntries = 1;
    psi->mpegType = NEXUS_TransportType_eUnknown;
    psi->psiValid = false;

out:
    if (playback_ip->playback_halt_event)
        BKNI_SetEvent(playback_ip->playback_halt_event);
    playback_ip->apiCompleted = true;
    playback_ip->apiInProgress = false;
    BDBG_MSG(("eventCallback %p, state %d", playback_ip->openSettings.eventCallback, playback_ip->playback_state));
    if (playback_ip->openSettings.eventCallback /*&& playback_ip->playback_state == B_PlaybackIpState_eSessionSetupInProgress*/)
        playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, B_PlaybackIpEvent_eSessionSetupDone);
    if (stream_info)
        BKNI_Free(stream_info);
    return;
}

/* 
   Function does following:
   -sdf
   -sdf
*/
B_PlaybackIpError 
B_PlaybackIp_HttpSessionOpen(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionOpenSettings *openSettings,
    B_PlaybackIpSessionOpenStatus *openStatus /* [out] */
    )
{
    int fd = 0;
    int rc = -1;
    char *http_hdr = NULL;
    char *http_payload = NULL;
    http_url_type_t http_url_type;
    char *server = NULL;
    int port;
    int loopCnt = 0;
    double seek_range = 0.;
    int speed = 1;
    off_t byteRangeStart, byteRangeEnd;
    B_PlaybackIpError errorCode = B_ERROR_PROTO;
	struct addrinfo hints;
	struct addrinfo *addrInfo = NULL;
    char portString[16];
	socklen_t addrLen;

    B_PlaybackIpSocketState *socketState;

    if (!playback_ip || !openSettings || !openStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, openSettings %p, openStatus %p\n", __FUNCTION__, playback_ip, openSettings, openStatus));
        errorCode = B_ERROR_INVALID_PARAMETER;
        return errorCode;
    }
    playback_ip->openSettings.u.http.additionalHeaders = NULL;
    if (openSettings->u.http.additionalHeaders) {
        if (!strstr(openSettings->u.http.additionalHeaders, "\r\n")) {
            BDBG_ERR(("%s: additional HTTP header is NOT properly terminated (missing \\r\\n), header is %s\n", __FUNCTION__, openSettings->u.http.additionalHeaders));
            errorCode = B_ERROR_INVALID_PARAMETER;
            goto error;
        }
        /* cache a copy of this field */
        if ((playback_ip->openSettings.u.http.additionalHeaders = strdup(openSettings->u.http.additionalHeaders)) == NULL) {
            BDBG_ERR(("%s: Failed to duplicate additional headers string due to out of memory condition", __FUNCTION__));
            goto error;
        }
    }
    playback_ip->openSettings.u.http.userAgent = NULL;
    if (openSettings->u.http.userAgent) {
        /* note: app is not required to terminate userAgent w/ \r\n as as accept it as a string only and add these terminators ourselves */
        /* cache a copy of this field */
        if ((playback_ip->openSettings.u.http.userAgent = strdup(openSettings->u.http.userAgent)) == NULL) {
            BDBG_ERR(("%s: Failed to duplicate userAgent field string due to out of memory condition", __FUNCTION__));
            goto error;
        }
    }
    /* cache a copy of this field as we may have to re-send request to server at a later point */
    BDBG_MSG(("%s: ip %s, port %d, proto %d, n/w buf sz %d, copyResp %d, url %s, addHdr %s, userAgent %s", __FUNCTION__, 
                openSettings->socketOpenSettings.ipAddr,
                openSettings->socketOpenSettings.port,
                openSettings->socketOpenSettings.protocol,
                openSettings->u.http.networkBufferSize,
                openSettings->u.http.copyResponseHeaders,
                openSettings->socketOpenSettings.url,
                openSettings->u.http.additionalHeaders,
                openSettings->u.http.userAgent
                ));

    socketState = &openStatus->socketState;

    playback_ip->responseMessage = (char *) BKNI_Malloc(TMP_BUF_SIZE+1);
    server = (char *)BKNI_Malloc(TMP_STRING_SIZE+1);
    if (!playback_ip->responseMessage || !server) {
        BDBG_ERR(("%s: ERROR: failed to allocate memory\n", __FUNCTION__));
        errorCode = B_ERROR_OUT_OF_MEMORY;
        goto error;
    }
    server[TMP_STRING_SIZE] = '\0';
    playback_ip->responseMessage[TMP_BUF_SIZE] = '\0';

    strncpy(server, openSettings->socketOpenSettings.ipAddr, TMP_STRING_SIZE);
    port = openSettings->socketOpenSettings.port;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    memset(portString, 0, sizeof(portString));  /* getaddrinfo() requires port # in the string form */
    snprintf(portString, sizeof(portString)-1, "%d", port);
    if (getaddrinfo(server, portString, &hints, &addrInfo) != 0) {
        BDBG_ERR(("%s: ERROR: getaddrinfo failed for server:port: %s:%d, errno %d", __FUNCTION__, server, port, errno));
        perror("getaddrinfo");
        errorCode = B_ERROR_SOCKET_ERROR;
        goto error;
    }
    socketState->remote_addr.sin_family = addrInfo->ai_family;
    socketState->remote_addr.sin_port = htons(openSettings->socketOpenSettings.port);
    socketState->remote_addr.sin_addr.s_addr = ((struct sockaddr_in *)(addrInfo->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(addrInfo);

    /* re-initialize the playback_ip state as this structure is not freed during each Socket Open/Close sequence */
    reset_playback_ip_state(playback_ip);
    playback_ip->networkTimeout = HTTP_SELECT_TIMEOUT;
    for (;;)
    {
        rc = -1;
        /* setup the socket connection to the server */
        if ((errorCode = B_PlaybackIp_UtilsTcpSocketConnect(&playback_ip->playback_state, server, port, true, &socketState->fd)) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to Setup Socket Connection to Server: %s:%d\n", __FUNCTION__, server, port));
            goto error;
        }
        playback_ip->socketState.fd = fd = socketState->fd;
        /* we cache the fd in the data cache as well. This is used when we are caching data */
        /* Do security session setup here as Get request to server may be encrypted */
        rc = B_PlaybackIp_SecuritySessionOpen(playback_ip, &playback_ip->openSettings, fd, &playback_ip->securityHandle);
        if (rc < 0) {
            BDBG_ERR(("%s: ERROR: failed to setup the session, rc = %d\n", __FUNCTION__, rc));
            errorCode = B_ERROR_PROTO;
            goto error;
        }

        /* prepare initial Get request */
        byteRangeStart = playback_ip->byteRangeOffset; /* default is 0; set in security session open */
        if (playback_ip->setupSettings.u.http.disableRangeHeader)
            byteRangeStart = 0; /* this ensures that we dont include Range Header in the outgoing requests */
        byteRangeEnd = 0;
        if (http_build_get_req(&playback_ip->requestMessage, playback_ip, speed, seek_range, 0., byteRangeStart, byteRangeEnd) < 0) {
            BDBG_ERR(("%s: ERROR: failed to build HTTP Get request to Server", __FUNCTION__));
            errorCode = B_ERROR_PROTO;
            goto error;
        }

        /* now send GET request */
        if ( (rc = playback_ip->netIo.write(playback_ip->securityHandle, &playback_ip->playback_state, socketState->fd, playback_ip->requestMessage, strlen(playback_ip->requestMessage))) < 0 ) {
            BDBG_ERR(("%s: ERROR: failed to send HTTP Get request to Server: %s:%d\n", __FUNCTION__, server, port));
            errorCode = B_ERROR_PROTO;
            goto error;
        }
        BDBG_MSG(("%s: Sent HTTP Get Request (fd %d)", __FUNCTION__, playback_ip->socketState.fd));
        BKNI_Free(playback_ip->requestMessage);
        playback_ip->requestMessage = NULL;

        /* now read the http response */
        if ( (rc = http_read_response(playback_ip, playback_ip->securityHandle, fd, &playback_ip->responseMessage, TMP_BUF_SIZE, &http_hdr, &http_payload, &playback_ip->initial_data_len)) < 0 ) {
            BDBG_ERR(("%s: ERROR: failed to receive valid HTTP response\n", __FUNCTION__));
            errorCode = B_ERROR_PROTO;
            goto error;
        }

        http_url_type = http_get_url_type(http_hdr, playback_ip->openSettings.socketOpenSettings.url);
        BDBG_MSG(("http url type %d", http_url_type));
        if (http_url_type == HTTP_URL_IS_ASX) {
            /* the response for a URL (which was a link to a content) would contain ASX Elements */
            /* Parse ASX header, it will either contains link to another ASX file or to the actual URL */
            if (http_parse_asx_payload(server, &playback_ip->openSettings.socketOpenSettings.url, http_payload) != 0) {
                BDBG_ERR(("Incorrect ASX Payload or parsing error\n'"));
                errorCode = B_ERROR_PROTO;
                goto error;
            }
            /* previous function gets the new URL & server information and we send another GET request to this server */
            close(fd);
            playback_ip->netIo.close(playback_ip->securityHandle);
            strncpy(playback_ip->openSettings.socketOpenSettings.ipAddr, server, sizeof(playback_ip->openSettings.socketOpenSettings.ipAddr) -1);
            playback_ip->openSettings.socketOpenSettings.ipAddr[sizeof(playback_ip->openSettings.socketOpenSettings.ipAddr)-1] = '\0';
        }
        else if (http_url_type == HTTP_URL_IS_REDIRECT) {
            if (!playback_ip->openSettings.socketOpenSettings.useProxy) {
                /* parse HTTP redirect and extract the new URI & server:port info */
                /* free up the previous cookie */
                if (playback_ip->cookieFoundViaHttpRedirect)
                    BKNI_Free(playback_ip->cookieFoundViaHttpRedirect);
                playback_ip->cookieFoundViaHttpRedirect = NULL;
                if (http_parse_redirect(server, &playback_ip->openSettings.socketOpenSettings.port, &playback_ip->openSettings.socketOpenSettings.protocol, &playback_ip->openSettings.socketOpenSettings.url, &playback_ip->cookieFoundViaHttpRedirect, http_hdr) != 0) {
                    BDBG_ERR(("Incorrect HTTP Redirect response or parsing error'"));
                    errorCode = B_ERROR_PROTO;
                    goto error;
                }
                /* now copy over the server IP address from the redicted URL for future byte range requests */
                strncpy(playback_ip->openSettings.socketOpenSettings.ipAddr, server, sizeof(playback_ip->openSettings.socketOpenSettings.ipAddr) -1);
                playback_ip->openSettings.socketOpenSettings.ipAddr[sizeof(playback_ip->openSettings.socketOpenSettings.ipAddr)-1] = '\0';
                port = playback_ip->openSettings.socketOpenSettings.port;
            }
            else {
                /* app is asking us to use proxy server, so all subsequent HTTP Get requests need to go thru this proxy server */
                /* We get the absolute URL from the Location Header in the HTTP Redirect response and use that as the URL in the Get Header */
                if (http_redirect_get_full_location_header(&playback_ip->openSettings.socketOpenSettings.url, http_hdr) != 0) {
                    BDBG_ERR(("Incorrect HTTP Redirect response or parsing error'"));
                    errorCode = B_ERROR_PROTO;
                    goto error;
                }
            }
            /* previous function gets the new URL & server information and we send another GET request to this server */
            close(fd);
            playback_ip->netIo.close(playback_ip->securityHandle);
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: HTTP redirect case, caching the redirected URL: http://%s:%d%s", __FUNCTION__, playback_ip->openSettings.socketOpenSettings.ipAddr, playback_ip->openSettings.socketOpenSettings.port, playback_ip->openSettings.socketOpenSettings.url));
#endif
        }
        else {
            /* actual content URL, get the content attributes from HTTP response header */
            BDBG_MSG(("GOT ACTUAL CONTENT\n"));

            /* copy any payload bytes (which is actual AV content) into a temp buffer */
            /* playback start copies this to playpump as well */
            if (playback_ip->initial_data_len != 0) {
                BDBG_MSG(("copying initial data (part of HTTP response) of len %d to temp buffer\n", playback_ip->initial_data_len));
                if (playback_ip->initial_data_len > TMP_BUF_SIZE) {
                    BDBG_ERR(("%s: ERROR: initial data read %d is greater than the temp buffer size %d, need to increase its size", __FUNCTION__, playback_ip->initial_data_len, TMP_BUF_SIZE));
                    errorCode = B_ERROR_PROTO;
                    goto error;
                }
                memcpy(playback_ip->temp_buf, http_payload, playback_ip->initial_data_len);
            }
                
            *http_payload = '\0';   /* for string parsing of HTTP header, payload is already copied into temp_buf */
            if (openSettings->u.http.copyResponseHeaders) {
                /* app needs a copy of the response headers for further app specific header parsing */
                openStatus->u.http.responseHeaders = strdup(http_hdr);
                if (openStatus->u.http.responseHeaders == NULL) {
                    BDBG_ERR(("%s: ERROR: failed to duplicate HTTP Response header string\n", __FUNCTION__));
                    errorCode = B_ERROR_OUT_OF_MEMORY;
                    goto error;
                }
            }
            /* TODO: copy local address in the socketState */

            /* we have the content now, so break out this loop */
            playback_ip->socketState.fd = fd = socketState->fd;
            break;
        }

        if (loopCnt++ >5) {
            BDBG_ERR(("ERROR: Can't resolve a URL Link in 5 attempts\n"));
            errorCode = B_ERROR_PROTO;
            goto error;
        }
    }
    /* success */
	addrLen = sizeof(socketState->local_addr);
	if (getsockname(socketState->fd, (struct sockaddr *)&socketState->local_addr, (socklen_t *)&addrLen) != 0) {
		BDBG_ERR(("ERROR: Failed to obtain connection socket address, errno: %d \n", errno));
		perror("getsockname");
		goto error;
	}

    BDBG_MSG(("%s: successfully received the HTTP Get Response (fd %d, local ip:port = %s:%d)", 
                __FUNCTION__, socketState->fd,
                inet_ntoa(socketState->local_addr.sin_addr),
                ntohs(socketState->local_addr.sin_port)));
    rc = 0;
    playback_ip->playback_state = B_PlaybackIpState_eOpened;

error:
    if (playback_ip->statusCode)
        openStatus->u.http.statusCode = playback_ip->statusCode;
    if (server) BKNI_Free(server);
    if (rc <0) {
        if (playback_ip->cookieFoundViaHttpRedirect)
            BKNI_Free(playback_ip->cookieFoundViaHttpRedirect);
        if (fd) 
            close(fd);
        if (playback_ip->responseMessage) {
            BKNI_Free(playback_ip->responseMessage);
            playback_ip->responseMessage = NULL;
        }
        if (playback_ip->requestMessage) {
            BKNI_Free(playback_ip->requestMessage);
            playback_ip->requestMessage = NULL;
        }
        if (playback_ip->openSettings.u.http.additionalHeaders) {
            free(playback_ip->openSettings.u.http.additionalHeaders);
            playback_ip->openSettings.u.http.additionalHeaders = NULL;
        }
        if (playback_ip->openSettings.u.http.userAgent) {
            free(playback_ip->openSettings.u.http.userAgent);
            playback_ip->openSettings.u.http.userAgent = NULL;
        }
        return errorCode;
    }
    else {
        return B_ERROR_SUCCESS;
    }
}

static int 
readAppHeader(
    B_PlaybackIpHandle playback_ip
    )
{
    int rc = -1;
    char *tmpPtr;
    do {
        tmpPtr = strstr(playback_ip->responseMessage, "BCM-App-Header-Inserted: length=");
        if (tmpPtr) {
            char *tmpPtr1;
            B_PlaybackIpAppHeader *appHeader;
            tmpPtr += strlen("BCM-App-Header-Inserted: length=");
            tmpPtr1 = strstr(tmpPtr, "\r\n");
            if (tmpPtr1) {
                appHeader = &playback_ip->appHeader;
                *tmpPtr1 = '\0';
                appHeader->length = strtol(tmpPtr, (char **)NULL, 10);
                *tmpPtr1 = '\r';
                BDBG_MSG(("%s: length %d", __FUNCTION__, appHeader->length));
                if (appHeader->length <= 0 || appHeader->length > sizeof(appHeader->data)) {
                    BDBG_ERR(("%s: read invalid appHeader length %d, should be > 0 & < %d", __FUNCTION__, appHeader->length, sizeof(appHeader->data)));
                    rc = -1;
                    break;
                }
                rc = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, playback_ip->socketState.fd, (char *)appHeader->data, appHeader->length, playback_ip->networkTimeout);
                if (rc <= 0) {
                    if (errno == EAGAIN) {
                        /* can happen due to DTCP/IP PCP header being present before the app header, that is why need to read another time */
                        rc = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, playback_ip->socketState.fd, (char *)appHeader->data, appHeader->length, playback_ip->networkTimeout);
                    }
                    if (rc <=0) {
                        BDBG_ERR(("%s: failed to read appHeader: length %d", __FUNCTION__, appHeader->length));
                        rc = -1;
                        break;
                    }
                }
                BDBG_MSG(("%s: appHeader data: %d, %d, %d, %d, %d, %d, %d, %d", __FUNCTION__, appHeader->data[0], appHeader->data[1], appHeader->data[2], appHeader->data[3], appHeader->data[4], appHeader->data[5], appHeader->data[6], appHeader->data[7]));
            }
        }
    } while(0);

    return rc;
}

/* 
   Function does following:
   -sdf
   -sdf
*/
B_PlaybackIpError 
B_PlaybackIp_HttpSessionSetup(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionSetupSettings *setupSettings,
    B_PlaybackIpSessionSetupStatus *setupStatus /* [out] */
    )
{
    int rc = -1;
    B_PlaybackIpError errorCode = B_ERROR_PROTO;
    char *http_hdr; 
    B_PlaybackIpPsiInfo *psi;

    if (!playback_ip || !setupSettings || !setupStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, setupSettings %p, setupStatus %p\n", __FUNCTION__, playback_ip, setupSettings, setupStatus));
        errorCode = B_ERROR_INVALID_PARAMETER;
        return errorCode;
    }

    /* if SessionSetup is in progress, return INCOMPLETE */
    if (playback_ip->apiInProgress)
        return B_ERROR_IN_PROGRESS;

    /* if SessionSetup is completed, return results to app */
    if (playback_ip->apiCompleted) {
        BDBG_MSG(("%s: previously started session setup operation completed, playback_ip %p", __FUNCTION__, playback_ip));
        /* Note: since this api was run in a separate thread, we defer thread cleanup until the Ip_Start */
        /* as this call to read up the session status may be invoked in the context of this thread via the callback */
        goto done;
    }

    /* Neither SessionSetup is in progress nor it is completed, so start setup */
    playback_ip->apiInProgress = true;
    playback_ip->apiCompleted = false;
    
    if (strstr(playback_ip->responseMessage, "BCM-App-Header-Inserted")) {
        if (readAppHeader(playback_ip) <= 0) {
            BDBG_ERR(("%s: Failed to read App specific header", __FUNCTION__));
            errorCode = B_ERROR_PROTO;
            goto error;
        }
        playback_ip->appHeader.valid = true;
    }

    if (!playback_ip->contentLength && setupSettings->u.http.contentLengthHint) {
        /* if server is streaming content using HTTP chunk xfer encoding, content length is not set */
        /* setting this value allows index information to be built and thus trickmodes will work on stream */
        playback_ip->contentLength = setupSettings->u.http.contentLengthHint;
        BDBG_MSG(("%s: Updated Content Length to user supplied hint %lld", __FUNCTION__, playback_ip->contentLength));
    }
    /* check if HTTP server provided any PSI info in the HTTP response header (Broadcom HTTP streamer does that) */
    /* in that case, we dont need to do explicit probe operation (which can take several seconds) and */
    /* SessionSetup is immediately completed. Also, Nexus Playpump must be used to feed IP streams, so we dont need to create Nexus file handle */
    http_hdr = playback_ip->responseMessage;
    playback_ip->http_content_type = http_get_payload_content_type(http_hdr);
    psi = &playback_ip->psi;
    B_PlaybackIp_HttpParseRespHeaderForPsi(playback_ip, playback_ip->http_content_type, http_hdr, psi);
    if ((psi->psiValid && psi->numPlaySpeedEntries > 1) || /* found valid PSI info & server supports the server side trick modes, thus we have already obtained media info in HTTP response and thus dont need to setup the index */
            playback_ip->openSettings.u.http.rvuCompliant || /* RVU: it uses trickmodes similar to server side trickmodes */
            (setupSettings->u.http.skipPsiParsing && playback_ip->openSettings.useNexusPlaypump) || /* we only allow app to tell us to skip PSI parsing if app is going to use Nexus Playpump to feed IP stream */
            psi->liveChannel /* we skip Media probe for Live Channels for two reasons: server provides all necessary info via HTTP response & we dont want to delay the live channels because of probing, saves 2-3 secs */
       ){
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_WRN(("%s: Skipping Media Probe: %s", 
                    __FUNCTION__, playback_ip->openSettings.u.http.rvuCompliant ? "RVU Session: App knows PSI info" :
                                ((psi->psiValid && psi->numPlaySpeedEntries > 1) ? "Server Side Trickmodes" :
                                 (psi->liveChannel ? "Live Channel" : "App knows PSI & is using Nexus Playpump"))));
#endif
        /* set flag to use Nexus Playpump as for any of these cases above, we have to use Nexus Playpump to feed IP streams. */
        /* we can't use Nexus Playback interface because all of these modes can do server side trickmodes which causes Nexus Playback to loose position info */
        /* as Nexus Playback doesn't know about the scaled position that server sends back. */
        playback_ip->useNexusPlaypump = true;
    }
    else {
        /* we didn't get PSI info from the server, it is a non-RVU non-live session where app doesn't want to skip the PSI discovery & not use Playpump*/
        /* so we may have to setup the Nexus File Handle for both data and index and optionally do PSI discovery. */

        if (psi->numPlaySpeedEntries > 1)
            /* this happens when server hasn't sent us the PSI info in the response. But since it supports playspeeds, we will need to use playpump for feeding data */
            /* as only it (& not nexus playback) gives us control for the server side trickmodes. We will let the code below use the media probe to find the PSI info and continue w/ playpump thread */
            playback_ip->useNexusPlaypump = true;

        /* so we create a Nexus File handle representing the network socket (both data & index handles are mapped to the network socket) */
        playback_ip->nexusFileHandle = B_PlaybackIp_HttpCreateFilePlayHandle(playback_ip, &playback_ip->bufferedFdIndex);
        if (playback_ip->nexusFileHandle == NULL) {
            BDBG_ERR(("%s: Failed to Create the Playback Handles\n", __FUNCTION__));
            errorCode = B_ERROR_OUT_OF_MEMORY;
            goto error;
        }
        BDBG_MSG(("%s: created Nexus file IO handle %p", __FUNCTION__, playback_ip->nexusFileHandle));

        /* check if we need to skip the probe, if so, we can't not setup the index descriptor as index info is built/discovered due to skipping PSI discovery */
        if (setupSettings->u.http.skipPsiParsing) {
            /* app told us to skip the probing as it already knows the media info */ 
            /* typically app does this for LPCM, RAD/EA encrypted MP3 streams */
            struct bfile_in_net *filePlay;
            BDBG_MSG(("%s: Skipping Media Probe as App asked to skip it", __FUNCTION__));

            /* turn off trick modes (by not providing the index file descriptor in file handle to the Playback */
            if (playback_ip->fileBuffered) {
                bfile_buffered_detach(playback_ip->fileBuffered);
                bfile_buffered_destroy(playback_ip->fileBuffered);
                playback_ip->fileBuffered = NULL;
            }
            if (playback_ip->factory) {
                batom_factory_destroy(playback_ip->factory);
                playback_ip->factory = NULL;
            }

            /* reset index handle */
            filePlay = (struct bfile_in_net *)playback_ip->nexusFileHandle;
            memset(&filePlay->index, 0, sizeof(filePlay->index));
            filePlay->self.file.index = NULL;

            if (setupSettings->u.http.avgBitRate) {
                psi->maxBufferDuration = (playback_ip->dataCache[0].size*8 / setupSettings->u.http.avgBitRate)*1000;  /* in msec */
                psi->avgBitRate = setupSettings->u.http.avgBitRate;
            }
        }
        else {
            /* App has asked for PSI info and it was not available via HTTP header response, so we will need to probe the media */
            /* This probe operation may require requesting data from server at different offsets and thus can take over seconds */
            /* So if app has requested *nonBlockingMode*, then carry out probe operation in a thread and return IN_PROGRESS to app */
            /* When this operation is completed, app is notified via the eventCallback and it can then re-issue SessionSetup to */
            /* read the results. */
            if (playback_ip->openSettings.nonBlockingMode) {
                /* do PSI parsing in a thread and return back to app */
                playback_ip->httpSessionSetupThread = B_Thread_Create("SessionSetupThread", (B_ThreadFunc)http_parse_av_info, (void *)playback_ip, NULL);
                if (NULL == playback_ip->httpSessionSetupThread) {
                    BDBG_ERR(("%s: Failed to create thread for media probe during Session Setup\n", __FUNCTION__));
                    errorCode = B_ERROR_UNKNOWN;
                    goto error;
                }
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: Non blocking media probe operation started: playback_ip %p\n", __FUNCTION__, playback_ip));
#endif
                errorCode = B_ERROR_IN_PROGRESS;
                goto error;
            }
            else {
                http_parse_av_info(playback_ip);
                /* success case, fall below to the done label */
            }
        }
    }

done:
    /* dont copy PSI info if App had asked us to skip PSI parsing */
    if (!setupSettings->u.http.skipPsiParsing) {
        if (!playback_ip->psi.psiValid) {
            BDBG_ERR(("%s: Failed to acquire PSI info via media probe\n", __FUNCTION__));
            errorCode = B_ERROR_UNKNOWN;
            goto error;
        }
        setupStatus->u.http.psi = playback_ip->psi;
        setupStatus->u.http.stream = (void *)playback_ip->stream;
    }
    setupStatus->u.http.file = playback_ip->nexusFileHandle;
    if (playback_ip->appHeader.valid) {
        setupStatus->u.http.appHeader = playback_ip->appHeader;
    }
    errorCode = B_ERROR_SUCCESS;

    /* success */
    rc = 0;

error:
    if (errorCode != B_ERROR_IN_PROGRESS) {
        playback_ip->apiInProgress = false;
        playback_ip->apiCompleted = false;
    }
    if (rc < 0) {
        return errorCode;
    }
    else {
        return B_ERROR_SUCCESS;
    }
}

/* HTTP processing thread: happens in the context of the Nexus IO threads */
/* Main purpose of this thread is to perform the pre-charging function when requested by app. */
/* App can request to do pre-charging either at the Ip_Start() time or during runtime. */
/* For runtime pre-charging, app uses the SetSettings function to indicate to this thread */
/* to start pre-charging. */
/* Note: once pre-charging is done, most content processing happens in the context of the Nexus IO threads */
void B_PlaybackIp_HttpPlaybackThread(
    void *data
    )
{
    B_PlaybackIpHandle playback_ip;
    int cacheIndex;
    off_t offset, adjustedOffset;
    ssize_t bytesRead = 0;
    int bytesToRead = 0;
    int spaceAvailableAtEnd;
    int rc = -1;
    bool sendEvent = false;
    char *internalError = NULL;

    playback_ip = (B_PlaybackIpHandle)data;
    if (playback_ip->settings.networkTimeout) {
        if (playback_ip->settings.networkTimeout > (HTTP_SELECT_TIMEOUT/10))
            playback_ip->networkTimeout = HTTP_SELECT_TIMEOUT/10;
        else
            playback_ip->networkTimeout = playback_ip->settings.networkTimeout;
    }
    else {
        playback_ip->networkTimeout = HTTP_SELECT_TIMEOUT/10;
    }
    BDBG_MSG(("%s: n/w timeout %d secs", __FUNCTION__, playback_ip->networkTimeout));

    /* main loop */
    /* TODO: extend this thread to do multiple functions:
       -process 2nd half of the SessionSetup command (where actual media happens) 
       -process request to do runtime buffering
    */
    while (true) {
        if  (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of main pre-charging loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        if (!playback_ip->preChargeBuffer && !playback_ip->sendNextTimeSeekReq) {
            /* app hasn't yet asked us to start pre-charging or turned off the flag, nor it wants us to send the time seek req for rvus, so wait */
            if (playback_ip->playback_state == B_PlaybackIpState_eBuffering) {
                playback_ip->playback_state = B_PlaybackIpState_ePlaying;
                BKNI_SetEvent(playback_ip->preChargeBufferEvent);
            }
            /* RVU TODO: check if 20msec isn't too low to impose extra overhead or not too high cause extra latency during trickplay */
            rc = BKNI_WaitForEvent(playback_ip->newJobEvent, 20);
            if (rc == BERR_TIMEOUT) {
                /* event timeout, thus no new task to do, go back and retry it */
                continue;
            } else if (rc!=0) {
                BDBG_ERR(("%s: got error while waiting for new task event, rc %d", __FUNCTION__, rc));
                break;
            }
            /* else we got a new task to do: so do the work below */
        }

        BKNI_AcquireMutex(playback_ip->lock);

        /* change playback_ip state to buffering state: this forces the read callbacks from the Nexus IO threads to return a timeout condition */
        /* this way we ensure that only one thread is working on the data cache */
        playback_ip->playback_state = B_PlaybackIpState_eBuffering;

        /* send event to let app thread know we have started buffering */
        BKNI_SetEvent(playback_ip->preChargeBufferEvent);

        /* wait until the nexus io threads has finished reading socket data in the read callbacks, otherwise we will have cache corruption */
        if (playback_ip->readCallBackInProgress) {
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: read callback via nexus io thread is still active, HTTP thread has to wait for its completion, sleep & retry\n", __FUNCTION__));
#endif
            BKNI_ReleaseMutex(playback_ip->lock);
            BKNI_Sleep(100);
            continue;
        }
            
        /* determine which data cache will be used for pre-charging: start with data cache 0 */
        if (!playback_ip->dataCache[0].inUse) {
            int initialDataLength;
            /* Since data cache is not yet setup, this must be the initial buffering request before playback has started. */
            /* Initialize data cache: index cache should already be setup by now (happens during media probe) */
            if (playback_ip->setupSettings.u.http.disableRangeHeader) {
                /* Since we can't send Range Request, we just start from the begining */
                initialDataLength = 0;
                playback_ip->dataCache[0].startOffset = 0;
                playback_ip->dataCache[0].endOffset = playback_ip->dataCache[0].startOffset - 1;
                BDBG_MSG(("%s: Can't seek when Range Header is not supported, so cache from begining!\n", __FUNCTION__));
            }
            /* However, for ASF, index data (if present) is at the end of stream. True index data is not acquired during probe until */
            /* playback starts. Thus, during initial buffering for ASF, we copy the content of index cache (mostly ASF headers) into the data cache*/
            else if (playback_ip->psi.mpegType == NEXUS_TransportType_eAsf) {
                /* Copy currently read data from index to data cache as it is really the movie hdr & data & not index data */
                initialDataLength = playback_ip->indexCacheDepth;
                memcpy(playback_ip->dataCache[0].cache, playback_ip->indexCache, initialDataLength);
                BDBG_MSG(("%s: copied %d bytes from index to data cache for ASF\n", __FUNCTION__, initialDataLength));

                playback_ip->dataCache[0].endOffset = playback_ip->indexCacheDepth -1;
                playback_ip->dataCache[0].startOffset = 0;
            }
            else {
                initialDataLength = 0;
                if (playback_ip->indexCacheEndOffset > 0)
                    playback_ip->dataCache[0].startOffset = playback_ip->indexCacheEndOffset + 1;
                else
                    playback_ip->dataCache[0].startOffset = 0;
                playback_ip->dataCache[0].endOffset = playback_ip->dataCache[0].startOffset - 1;
            }

            /* need to set this as this otherwise get status keeps returning 0 buffer depth due to lastSeekOffset not changing */
            playback_ip->lastSeekOffset = playback_ip->dataCache[0].startOffset;
            playback_ip->dataCache[0].readIndex = 0;
            playback_ip->dataCache[0].depth = initialDataLength;
            playback_ip->dataCache[0].writeIndex = initialDataLength;
            playback_ip->dataCache[0].inUse = true;
            playback_ip->cacheDepthFudgeFactor = 0;
            cacheIndex = 0;
            BDBG_MSG(("%s: Data Cache (# %d) setup during pre-charging: offset start %lld, end %lld, depth %d, index rd %u, wr %u\n",
                    __FUNCTION__, cacheIndex, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset, 
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, playback_ip->dataCache[cacheIndex].writeIndex));
        }
        else if ( (playback_ip->dataCache[playback_ip->lastUsedCacheIndex].endOffset+1) == playback_ip->contentLength) {
            /* we are asked to buffer, but we have already received the complete content from server */
            /* so this must be the rewind case where app is asking us to pre-charge before replaying */
            /* we reset the cache and start buffering from begining */

            /* select the cache that was used last by the io threads to read data */
            cacheIndex = playback_ip->lastUsedCacheIndex;
            playback_ip->dataCache[cacheIndex].readIndex = 0;
            playback_ip->dataCache[cacheIndex].depth = 0;
            playback_ip->dataCache[cacheIndex].writeIndex = 0;
            playback_ip->dataCache[cacheIndex].startOffset = 0;
            playback_ip->lastSeekOffset = playback_ip->dataCache[0].startOffset;
            playback_ip->dataCache[cacheIndex].endOffset = playback_ip->dataCache[cacheIndex].startOffset - 1;
            playback_ip->cacheDepthFudgeFactor = 0;
            /* set flag to indicate that we need to re-open the socket */
            playback_ip->reOpenSocket = true;
            playback_ip->serverClosed = false;
            BDBG_MSG(("%s: Data Cache (# %d) pre-charging during rewind: offset start %lld, end %lld, depth %d, index rd %u, wr %u\n",
                    __FUNCTION__, cacheIndex, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset, 
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, playback_ip->dataCache[cacheIndex].writeIndex));
        }
        else {
            /* This pre-charging request is after the data cache has been setup (either during playback or when index is at end for MP4) */
            /* We move the start index forward to the last used offset value, accordingly reduce the buffer depth, */
            /* and then start buffering after the point upto which we have already buffered */
            
            /* select the cache that was used last by the io threads to read data */
            cacheIndex = playback_ip->lastUsedCacheIndex;
            BDBG_MSG(("%s: before adjustments: Data Cache (# %d): lastSeekOffset %lld, offset start %lld, end %lld, depth %d, index rd %u, wr %u\n",
                    __FUNCTION__, cacheIndex, playback_ip->lastSeekOffset, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset, 
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, playback_ip->dataCache[cacheIndex].writeIndex));
            if (playback_ip->lastSeekOffset > playback_ip->dataCache[cacheIndex].startOffset && playback_ip->lastSeekOffset <= playback_ip->dataCache[cacheIndex].endOffset) {
                int remainder;
                /* last seek offset is in the cache */
                adjustedOffset = playback_ip->lastSeekOffset;
                /* round off the offset to the chunk size */
                remainder = (adjustedOffset%HTTP_DATA_CACHE_CHUNK_SIZE);
                adjustedOffset -= remainder;
                /* move offset back by n chunks: mainly because for MP4 when we resume, player might need some of the previous data */
                adjustedOffset -= HTTP_CACHE_DEPTH_FUDGE_FACTOR;
                /* we rememeber this fudge factor as we adjust max buffer size by this amount in the GetStatus call */
                playback_ip->cacheDepthFudgeFactor = HTTP_CACHE_DEPTH_FUDGE_FACTOR + remainder;
                if (adjustedOffset <= playback_ip->dataCache[cacheIndex].startOffset) {
                    BDBG_MSG(("%s: adjusted offset was going to go before cache start, resetting it to cacheStart\n", __FUNCTION__));
                    offset = 0;
                }
                else {
                    offset = adjustedOffset - playback_ip->dataCache[cacheIndex].startOffset;
                    BDBG_MSG(("%s: moving start offset by %d bytes\n", __FUNCTION__, offset));
                }
                /* startOffset is moved forward to the adjustedOffset */
                playback_ip->dataCache[cacheIndex].startOffset += offset;
                /* readIndex is moved by the offset amount, taking care of the wrapping */
                playback_ip->dataCache[cacheIndex].readIndex += offset;
                playback_ip->dataCache[cacheIndex].readIndex %= playback_ip->dataCache[cacheIndex].size;
                /* cache depth is accordingly adjusted to indicate additional space that has been created for buffering */
                playback_ip->dataCache[cacheIndex].depth = playback_ip->dataCache[cacheIndex].endOffset - playback_ip->dataCache[cacheIndex].startOffset + 1;
                if (playback_ip->dataCache[cacheIndex].depth >= playback_ip->dataCache[cacheIndex].size || playback_ip->dataCache[cacheIndex].depth <= 0) {
                    internalError = "CacheDepthNotCorrect";
                    goto error;
                }
                BDBG_MSG(("%s: Data Cache (# %d) Pruning during pre-charging: lastSeekOffset %lld, offset start %lld, end %lld, depth %d, index rd %u, wr %u\n",
                    __FUNCTION__, cacheIndex, playback_ip->lastSeekOffset, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset, 
                    playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, playback_ip->dataCache[cacheIndex].writeIndex));
            }
            else {
                int remainder;
                /* last seek offset is NOT in the cache, reset cache & reopen connex from last seek offset */
                playback_ip->dataCache[cacheIndex].readIndex = 0;
                playback_ip->dataCache[cacheIndex].depth = 0;
                playback_ip->dataCache[cacheIndex].writeIndex = 0;
                /* last seek offset is in the cache */
                adjustedOffset = playback_ip->lastSeekOffset;
                /* round off the offset to the chunk size */
                remainder = (adjustedOffset%HTTP_DATA_CACHE_CHUNK_SIZE);
                adjustedOffset -= remainder;
                /* move offset back by n chunks: mainly because for MP4 when we resume, player might need some of the previous data */
                adjustedOffset -= HTTP_CACHE_DEPTH_FUDGE_FACTOR;
                if (adjustedOffset < 0) {
                    adjustedOffset = 0;
                }
                else
                    /* we rememeber this fudge factor as we adjust max buffer size by this amount in the GetStatus call */
                    playback_ip->cacheDepthFudgeFactor = HTTP_CACHE_DEPTH_FUDGE_FACTOR + remainder;
                playback_ip->dataCache[cacheIndex].startOffset = adjustedOffset;
                playback_ip->dataCache[cacheIndex].endOffset = playback_ip->dataCache[cacheIndex].startOffset - 1;
                /* set flag to indicate that we need to re-open the socket */
                playback_ip->reOpenSocket = true;
                BDBG_MSG(("%s: Data Cache (# %d) pre-charging: seekOffset is outside current cache, restarting cache from it: offset lastSeek %lld, start %lld, end %lld, depth %d, index rd %u, wr %u\n",
                        __FUNCTION__, cacheIndex, playback_ip->lastSeekOffset, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset, 
                        playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].readIndex, playback_ip->dataCache[cacheIndex].writeIndex));
            }
        }

        /* we keep pre-charging the network buffer until either preChargeBuffer flag is turned off (via B_PlaybackIp_SetSetting()) or network buffer gets full. */
        while (true) {
            sendEvent = true;
            if  (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
                /* user changed the channel, so return */
                BDBG_MSG(("%s: breaking out of current pre-charging loop due to state (%d) change\n", __FUNCTION__, playback_ip->playback_state));
                break;
            }
            if (!playback_ip->preChargeBuffer) {
                /* app is now asking to stop preCharging, so break */
                BDBG_MSG(("%s: preChargeBuffer flag is now false, stop pre-charging (playback ip state %d) \n", __FUNCTION__, playback_ip->playback_state));
                break;
            }

            /* See if there is room in cache to read n/w data */ 
            if (playback_ip->dataCache[cacheIndex].depth >= playback_ip->dataCache[cacheIndex].size || playback_ip->serverClosed) {
                BDBG_MSG(("%s: Pre-Charge buffer is full (size %d, depth %d) or eof (%d), waiting for app to stop pre-charging, till then sleeping...\n", 
                            __FUNCTION__, playback_ip->dataCache[cacheIndex].size, playback_ip->dataCache[cacheIndex].depth, playback_ip->serverClosed));
                BKNI_Sleep(500);
                continue;
            }

            /* read lower of data chunk size or space available at end of data cache FIFO */
            spaceAvailableAtEnd = playback_ip->dataCache[cacheIndex].size - playback_ip->dataCache[cacheIndex].writeIndex;
            if (spaceAvailableAtEnd < HTTP_DATA_CACHE_CHUNK_SIZE) {
                BDBG_MSG(("%s: reached towards end of data cache, so reading (%d) less than chunk size (%d), cache size %d, cache wr idx %d\n",
                            __FUNCTION__, spaceAvailableAtEnd, HTTP_DATA_CACHE_CHUNK_SIZE, playback_ip->dataCache[cacheIndex].size, playback_ip->dataCache[cacheIndex].writeIndex));
                bytesToRead = spaceAvailableAtEnd;
            }
            else 
                bytesToRead = HTTP_DATA_CACHE_CHUNK_SIZE;

            if (playback_ip->reOpenSocket /* true if index was at the end (e.g. for ASF) and thus server had closed socket a/f sending the index part */
                    || playback_ip->cacheIndexUsingSocket != cacheIndex /* true if we are switching the data caches */
            ) {
                /* in both cases, we need to close the current socket & send a new request to server */
                /* both are done in NetRangeReq()) */
                BDBG_MSG(("%s: re-setup the connection for cache idx %d, prev cache idx %d, re-open socket %d\n", __FUNCTION__, cacheIndex, playback_ip->cacheIndexUsingSocket, playback_ip->reOpenSocket));
                playback_ip->reOpenSocket = false;
                playback_ip->cacheIndexUsingSocket = cacheIndex; 
                /* send new HTTP Range request on a new socket */
                rc = B_PlaybackIp_HttpNetRangeReq(playback_ip, playback_ip->dataCache[cacheIndex].cache + playback_ip->dataCache[cacheIndex].writeIndex, bytesToRead, playback_ip->dataCache[cacheIndex].endOffset+1, 0, playback_ip->socketState.fd, &playback_ip->socketState.fd);
            }
            else {
                /* read 1 chunk worth data from n/w */
                rc = playback_ip_read_socket(playback_ip, playback_ip->securityHandle,
                        playback_ip->socketState.fd,
                        playback_ip->dataCache[cacheIndex].cache + playback_ip->dataCache[cacheIndex].writeIndex, 
                        bytesToRead, playback_ip->networkTimeout);
           }
           if (rc <= 0) {
#ifdef BDBG_DEBUG_BUILD
               if (playback_ip->ipVerboseLog)
                   BDBG_ERR(("%s: Network Read Error, rc %d, playback ip state %d", __FUNCTION__, rc, playback_ip->playback_state));
#endif
                /* we wait until we recover from n/w error (e.g. select timeout due to temporary n/w loss) or app turns off the pre-charging */
                BKNI_Sleep(100);
                continue;
            }
            bytesRead += rc;

            /* increment data cache write index */
            playback_ip->dataCache[cacheIndex].writeIndex += rc;
            playback_ip->dataCache[cacheIndex].writeIndex %= playback_ip->dataCache[cacheIndex].size;
            /* indicate addition of this new chunk by moving forward the end offset by a chunk size */
            playback_ip->dataCache[cacheIndex].endOffset == 0 ? 
                playback_ip->dataCache[cacheIndex].endOffset = rc - 1 :
                (playback_ip->dataCache[cacheIndex].endOffset += rc);
            /* increase data cache depth by read amount */
            playback_ip->dataCache[cacheIndex].depth += rc;

            if (!playback_ip->dataCache[cacheIndex].inUse) {
                playback_ip->dataCache[cacheIndex].inUse = true;
            }
        }

        /* change playback_ip state to back to playing state */
        if (playback_ip->playback_state == B_PlaybackIpState_eBuffering)
            playback_ip->playback_state = B_PlaybackIpState_ePlaying;
        if (sendEvent) {
            BKNI_SetEvent(playback_ip->preChargeBufferEvent);
            sendEvent = false;
        }

        BDBG_MSG(("%s: pre-charged data cache[%d] read %d: cache start %lld, end %lld depth %d, wr idx %d",
            __FUNCTION__, cacheIndex, bytesRead, playback_ip->dataCache[cacheIndex].startOffset, playback_ip->dataCache[cacheIndex].endOffset,
            playback_ip->dataCache[cacheIndex].depth, playback_ip->dataCache[cacheIndex].writeIndex));
        bytesRead = 0;
        /* reset the fudge factor */
        playback_ip->cacheDepthFudgeFactor = 0;
        BKNI_ReleaseMutex(playback_ip->lock);
    }

error:
    if (internalError) {
        BDBG_ERR(("%s: sw bug: error hint: %s\n", __FUNCTION__, internalError));
        BKNI_SetEvent(playback_ip->preChargeBufferEvent);
    }
    BDBG_MSG(("%s: thread is exiting..., state %d", __FUNCTION__, playback_ip->playback_state));
    BKNI_SetEvent(playback_ip->playback_halt_event);
    playback_ip->speed = 0;
    BKNI_ReleaseMutex(playback_ip->lock);
    return;
}

bool 
B_PlaybackIp_UtilsEndOfStream(
    B_PlaybackIpHandle playback_ip
    )
{
    unsigned int currentPlayed;
    bool status = true;
    /* TODO: can't sleep here as user may be trying to stop/close during this time and thus that would be stuck */
    /* instead, check if we have reached the end and return true or false here */
    /* caller should check for stop/stopping state */

    if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped || playback_ip->psi.liveChannel) {
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_WRN(("%s: %s (playback ip state %d), so return immediately", __FUNCTION__, playback_ip->psi.liveChannel ? "live channel":"app has issued stop", playback_ip->playback_state));
#endif
        return true;
    }

    if (playback_ip->nexusHandles.videoDecoder) {
        NEXUS_VideoDecoderStatus status;
        if (NEXUS_VideoDecoder_GetStatus(playback_ip->nexusHandles.videoDecoder, &status) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to get vidoer status", __FUNCTION__));
            return true;
        }
        currentPlayed = status.numDisplayed;
    }
    else if (playback_ip->nexusHandles.primaryAudioDecoder || playback_ip->nexusHandles.secondaryAudioDecoder) {
        NEXUS_AudioDecoderStatus audioStatus;
        NEXUS_AudioDecoderHandle audioDecoder;

        audioDecoder = playback_ip->nexusHandles.primaryAudioDecoder ? playback_ip->nexusHandles.primaryAudioDecoder:playback_ip->nexusHandles.secondaryAudioDecoder;
        if (NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to get current audio decoder status", __FUNCTION__));
            return true;
        }
        currentPlayed = audioStatus.pts;
    }
#ifdef NEXUS_HAS_SIMPLE_DECODER
    else if (playback_ip->nexusHandles.simpleVideoDecoder) {
        NEXUS_VideoDecoderStatus status;
        if (NEXUS_SimpleVideoDecoder_GetStatus(playback_ip->nexusHandles.simpleVideoDecoder, &status) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to get vidoer status", __FUNCTION__));
            return true;
        }
        currentPlayed = status.numDisplayed;
    }
#endif
    else {
        /* no audio or video decoders, shouldn't happen, just return true */
        return true;
    }
    if (playback_ip->prevPlayed == 0) {
        /* 1st call to this function */
        playback_ip->prevPlayed = currentPlayed;
        status = false;
    }
    else {
        if (currentPlayed == 0 || currentPlayed == playback_ip->prevPlayed) {
            /* audio PTS or display count isn't changing, so we are done playing the stream */
            NEXUS_PlaybackPosition currentPosition;
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: %s isn't changing, so we are done playing the stream", __FUNCTION__, (playback_ip->nexusHandles.videoDecoder || playback_ip->nexusHandles.simpleVideoDecoder)? "Displaed Picture Count" : "Audio PTS"));
#endif
            /* reached EOF */
            status = true;
            playback_ip->serverClosed = true;
            if (B_PlaybackIp_HttpGetCurrentPlaybackPosition(playback_ip, &currentPosition) != B_ERROR_SUCCESS) {
                BDBG_WRN(("%s: Failed to determine the current playback position", __FUNCTION__));
            }
            else 
                BDBG_WRN(("%s: last play position %0.3f", __FUNCTION__, currentPosition/1000.));
        }
        else {
            /* still changing, still not reached end of stream */
            playback_ip->prevPlayed = currentPlayed;
            status = false;
        }
    }
    
    return status;
}

#define HTTP_PLAYPUMP_BUF_SIZE (16384)
void B_PlaybackIp_HttpPlaypumpThread(
    void *data
    )
{
    B_PlaybackIpHandle playback_ip;
    size_t totalBytesRecv;
    ssize_t rc = -1;
    FILE * fclear = NULL;
    static int fileNameSuffix = 0;
    char recordFileName[32];

    playback_ip = (B_PlaybackIpHandle)data;
    if (playback_ip->settings.networkTimeout) {
        if (playback_ip->settings.networkTimeout > (HTTP_SELECT_TIMEOUT/10))
            playback_ip->networkTimeout = HTTP_SELECT_TIMEOUT/10;
        else
            playback_ip->networkTimeout = playback_ip->settings.networkTimeout;
    }
    else {
        playback_ip->networkTimeout = HTTP_SELECT_TIMEOUT/10;
    }
    BDBG_MSG(("%s: n/w timeout %d secs", __FUNCTION__, playback_ip->networkTimeout));

    if (B_PlaybackIp_UtilsWaitForPlaypumpDecoderSetup(playback_ip))
        goto error;

    if (playback_ip->enableRecording) {
        memset(recordFileName, 0, sizeof(recordFileName));
        snprintf(recordFileName, sizeof(recordFileName)-1, "./videos/http_playpump_rec%d.ts", fileNameSuffix++);
        fclear = fopen(recordFileName, "w+b");
    }
    if (playback_ip->serverClosed) {
        /* this is to allow the case where app can restart the session by issuing stop & start again */
        if (http_do_server_trick_modes_socket_transition(playback_ip, 0, 0, 1) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: Failed to re-open socket connection", __FUNCTION__));
            goto error;
        }
        playback_ip->serverClosed = false;
    }
    if (playback_ip->settings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
        B_PlaybackIp_TtsThrottle_Params throttleParams;
        NEXUS_PlaypumpSettings playpumpSettings;

        playback_ip->ttsThrottle = B_PlaybackIp_TtsThrottle_Open();

        B_PlaybackIp_TtsThrottle_GetSettings(playback_ip->ttsThrottle, &throttleParams);
        BKNI_Memcpy(&throttleParams, &playback_ip->settings.ttsParams.throttleParams, sizeof(B_PlaybackIp_TtsThrottle_Params));
        throttleParams.playPump = playback_ip->nexusHandles.playpump;
        NEXUS_Playpump_GetSettings(playback_ip->nexusHandles.playpump, &playpumpSettings);
        BDBG_MSG(("Setting throttleParams.timebase: %d", playpumpSettings.timestamp.timebase));
        throttleParams.timebase = playpumpSettings.timestamp.timebase;
        B_PlaybackIp_TtsThrottle_SetSettings(playback_ip->ttsThrottle, &throttleParams);
        B_PlaybackIp_TtsThrottle_Start(playback_ip->ttsThrottle);
    }

    /* main loop */
    while (true) {
        BKNI_AcquireMutex(playback_ip->lock);
        if (playback_ip->playback_state == B_PlaybackIpState_eStopping || playback_ip->playback_state == B_PlaybackIpState_eStopped) {
            /* user changed the channel, so return */
            BDBG_MSG(("%s: breaking out of HTTP loop due to state (%d) change", __FUNCTION__, playback_ip->playback_state));
            break;
        }
        if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
            BKNI_ReleaseMutex(playback_ip->lock);
            BDBG_MSG(("%s: pausing feeder thread to allow trickmode transitions", __FUNCTION__));
            BKNI_Sleep(20); /* wait little bit before going back to the top, this allows trickmode thread to get the lock first, finish the trickmode transition and then release the lock */
            continue;
        }

        if (playback_ip->playback_state == B_PlaybackIpState_ePaused) {
            BKNI_ReleaseMutex(playback_ip->lock);
            BDBG_MSG(("%s: wait to come out of pause", __FUNCTION__));
            BKNI_Sleep(100);
            continue;
        }

        /* get an adequately sized buffer from the playpump */
        if (B_PlaybackIp_UtilsGetPlaypumpBuffer(playback_ip, HTTP_PLAYPUMP_BUF_SIZE) < 0) {
            BKNI_ReleaseMutex(playback_ip->lock);
#ifdef BDBG_DEBUG_BUILD
            if (playback_ip->ipVerboseLog)
                BDBG_WRN(("%s: Failed to get buffer from playpump, retry ", __FUNCTION__));
#endif
            BKNI_Sleep(100);
            continue;
        }

        /* read the requested range of data chunk from socket */
        rc = playback_ip_read_socket(playback_ip, playback_ip->securityHandle, playback_ip->socketState.fd, (char *)playback_ip->buffer, HTTP_PLAYPUMP_BUF_SIZE, playback_ip->networkTimeout);
        BDBG_MSG(("%s: Got playpump buffer to read more data into, now Read %d bytes from socket into this buffer", __FUNCTION__, rc));
        if (rc <= 0) {
            BDBG_MSG(("%s: rc %d, serverClosed %d, selectTimeout %d", __FUNCTION__, rc, playback_ip->serverClosed, playback_ip->selectTimeout));
            if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode || playback_ip->selectTimeout) {
                BKNI_ReleaseMutex(playback_ip->lock);
#ifdef BDBG_DEBUG_BUILD
                if (playback_ip->ipVerboseLog)
                    BDBG_WRN(("%s: %s", __FUNCTION__, playback_ip->selectTimeout? "no data coming from server, retry!": "pausing feeder thread to allow trickmode transitions"));
#endif
                BKNI_Sleep(50);
                continue;
            }
            else if (playback_ip->serverClosed && playback_ip->openSettings.u.http.rvuCompliant && playback_ip->playback_state == B_PlaybackIpState_eTrickMode) {
                /* server has closed (either via end of chunk header or socket close) & we are in trickmode state */
                /* set a flag to indicate to the HTTP thread to send next time seek request */
                /* fake these values as it will make this function to return TIMEOUT message to the player and make it retry the read */
                rc = 0;
                playback_ip->serverClosed = false;
                playback_ip->selectTimeout = true;
                playback_ip->sendNextTimeSeekReq = true;
                /* TODO: needs more work */
                BDBG_MSG(("%s: RVU trickmode case, finished reading one set of frames, send request for the next frame", __FUNCTION__));
                /* we are in the trick mode state where we need to send new time seek request */
                if ((rc = http_send_time_seek_request(playback_ip)) != B_ERROR_SUCCESS) {
                    if (playback_ip->playback_state == B_PlaybackIpState_eWaitingToEnterTrickMode) {
                        BKNI_ReleaseMutex(playback_ip->lock);
                        BDBG_MSG(("%s: pausing feeder thread to allow trickmode transitions", __FUNCTION__));
                        BKNI_Sleep(50);
                        continue;
                    }
                    BDBG_MSG(("%s: RVU Trick mode transition had some error, ip state %d \n", __FUNCTION__, playback_ip->playback_state));
                    goto error;
                }
                BKNI_ReleaseMutex(playback_ip->lock);
                BDBG_MSG(("%s: RVU trickmode: done sending next time seek request, continue reading & feeding", __FUNCTION__));
                continue;
            }
            else {
                BDBG_MSG(("%s: Network Read Error, wait to play out the stream", __FUNCTION__));
                /* wait for h/w pipeline to playout the buffered up data and then issue the error/eof callback */
                if (B_PlaybackIp_UtilsEndOfStream(playback_ip) == true)
                    break;
                else {
                    /* continue waiting to either playout the whole stream or re-reading from socket incase it made valid again by a seek or rewind trickplay */
                    BKNI_ReleaseMutex(playback_ip->lock);
#ifdef BDBG_DEBUG_BUILD
                    if (playback_ip->ipVerboseLog)
                        BDBG_WRN(("%s: Continue waiting to either playout the whole stream or re-reading from socket incase it becomes valid again by a seek or rewind trickplay", __FUNCTION__));
#endif
                    BKNI_Sleep(1000);
                    continue;
                }
            }
        }
        playback_ip->totalConsumed += rc;
        totalBytesRecv = rc;
        BDBG_MSG(("%s: Read from socket: asked %d bytes, returning %d bytes, total Read %lld, Consumed %lld", 
                    __FUNCTION__, totalBytesRecv, rc, playback_ip->totalRead, playback_ip->totalConsumed));
        
        /* now feed appropriate data it to the playpump */
        if (NEXUS_Playpump_WriteComplete(playback_ip->nexusHandles.playpump, 0, totalBytesRecv)) {
            BDBG_WRN(("%s: NEXUS_Playpump_WriteComplete failed, continuing...", __FUNCTION__));
            BKNI_ReleaseMutex(playback_ip->lock);
            continue;
        }
        /* write data to file */
        if (fclear) {
            fwrite(playback_ip->buffer, 1, totalBytesRecv, fclear); 
            fflush(fclear);
        }
        BKNI_ReleaseMutex(playback_ip->lock);
        BDBG_MSG(("%s: Fed %d bytes to Playpump\n", __FUNCTION__, totalBytesRecv));
    }

error:
    BKNI_ReleaseMutex(playback_ip->lock);
    /* TODO: look if tts stop/close needs to be called here or get called within ip stop, most likely should be done here */

    BDBG_MSG(("%s: thread is exiting..., event cb %p, state %d", __FUNCTION__, playback_ip->openSettings.eventCallback, playback_ip->playback_state));

    if (playback_ip->settings.ipMode == B_PlaybackIpClockRecoveryMode_ePushWithTtsNoSyncSlip) {
        B_PlaybackIp_TtsThrottle_Stop(playback_ip->ttsThrottle);
        B_PlaybackIp_TtsThrottle_Close(playback_ip->ttsThrottle);
    }

    if (playback_ip->openSettings.eventCallback && 
            playback_ip->playback_state != B_PlaybackIpState_eStopping && 
            playback_ip->playback_state != B_PlaybackIpState_eStopped) 
    {
        B_PlaybackIpEventIds eventId;
        if (playback_ip->serverClosed)
            eventId = B_PlaybackIpEvent_eServerEndofStreamReached;
        else
            eventId = B_PlaybackIpEvent_eErrorDuringStreamPlayback;
        playback_ip->openSettings.eventCallback(playback_ip->openSettings.appCtx, eventId);
    }
    if (fclear) {
        fclose(fclear);
    }
    if (playback_ip->playback_halt_event)
        BKNI_SetEvent(playback_ip->playback_halt_event);
    return;
}

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
void B_PlaybackIp_HlsPlaybackThread(void *data);
#endif

#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
void B_PlaybackIp_MpegDashPlaybackThread(void *data);
#endif

B_PlaybackIpError 
B_PlaybackIp_HttpSessionStart(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpSessionStartSettings *startSettings,
    B_PlaybackIpSessionStartStatus *startStatus /* [out] */
    )
{
    B_PlaybackIpError errorCode = B_ERROR_PROTO;
    B_ThreadSettings settingsThread;
    char *threadName;
    BERR_Code rc;
    B_ThreadFunc httpThreadFuncName;

    if (!playback_ip || !startSettings || !startStatus) {
        BDBG_ERR(("%s: invalid params, playback_ip %p, startSettings %p, startStatus %p\n", __FUNCTION__, playback_ip, startSettings, startStatus));
        errorCode = B_ERROR_INVALID_PARAMETER;
        return errorCode;
    }

    B_PlaybackIp_HttpSetDefaultTrickModeSettings(&playback_ip->ipTrickModeSettings);
    threadName = "PlaybackIpHttp";

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    if (playback_ip->hlsSessionEnabled) {
        httpThreadFuncName = B_PlaybackIp_HlsPlaybackThread;
    }
    else 
#endif
#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
    if (playback_ip->mpegDashSessionEnabled) {
        httpThreadFuncName = B_PlaybackIp_MpegDashPlaybackThread;
    }
    else 
#endif
    if (playback_ip->useNexusPlaypump) {
        /* Feed media stream to Nexus Playpump, done when Nexus Playback can't keep track of stream position (e.g. in server side trickmodes case) */
        httpThreadFuncName = B_PlaybackIp_HttpPlaypumpThread;
    }
    else {
        /* by default for HTTP playback, we use Nexus Playback to pull media stream from IP library */
        httpThreadFuncName = B_PlaybackIp_HttpPlaybackThread;
    }
    

    /* reset this event as it may have been set during the psi media probe */
    BKNI_ResetEvent(playback_ip->playback_halt_event);

    /* create thread to process incoming IP packets & feed them to PB hw */
    B_Thread_GetDefaultSettings(&settingsThread);
    playback_ip->playbackIpThread = B_Thread_Create(threadName, httpThreadFuncName, (void *)playback_ip, &settingsThread);
    if (NULL == playback_ip->playbackIpThread) {
        BDBG_ERR(("%s: Failed to create the %s thread for HTTP media transport protocol\n", __FUNCTION__, threadName));
        goto error;
    }


    if (
#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
        !playback_ip->hlsSessionEnabled && 
#endif
#ifdef B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
        !playback_ip->mpegDashSessionEnabled && 
#endif
        playback_ip->settings.ipMode == B_PlaybackIpClockRecoveryMode_ePull &&
        playback_ip->preChargeBuffer) {

        /* wait until pre-charging has started */
        rc = BKNI_WaitForEvent(playback_ip->preChargeBufferEvent, IP_HALT_TASK_TIMEOUT_MSEC);
        if (rc == BERR_TIMEOUT) {
            BDBG_WRN(("%s: timed out for pre-charging start event", __FUNCTION__));
            return B_ERROR_UNKNOWN;
        } else if (rc!=0) {
            BDBG_WRN(("%s: got error while trying to wait for pre-charging start event, rc %d", __FUNCTION__, rc));
            return B_ERROR_UNKNOWN;
        }
        BDBG_MSG(("%s: Enabled pre-charging of network buffer", __FUNCTION__));
    }
    errorCode = B_ERROR_SUCCESS;

error:
    return errorCode;
}

void 
B_PlaybackIp_HttpSessionStop(B_PlaybackIpHandle playback_ip)
{
    /* TODO: make this more deterministic by waiting on the playback_halt_event and making DataRead send this event when state is stopped */
    while (playback_ip->readCallBackInProgress) {
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_WRN(("read callback via nexus io thread is still active, %s() has to wait for its completion, for now sleeping for 100msec & retrying", __FUNCTION__));
#endif
        BKNI_Sleep(100);
        continue;
    }
}

#endif /* LINUX || VxWorks */
