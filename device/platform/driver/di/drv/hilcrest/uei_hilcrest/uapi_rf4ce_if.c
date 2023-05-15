/*===================================================================
 COPYRIGHT 2010 UNIVERSAL ELECTRONICS INC (UEI).
 These materials are provided under license by UEI.  UEI licenses
 this file to you under the Universal Remote Control API (UAPI)
 License accompanying the UEI libuapi Software (the "License").
 You may not use this file except in compliance with the License.
 You may obtain a copy of the License from UEI. Unless required by
 applicable law or agreed to in writing, all materials distributed
 under the License is distributed on an "AS IS" BASIS, WITHOUT
 WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions
 and limitations under the License.
===================================================================*/
/*********************************************************************************
* Filename:     uapi_rf4ce_if.c
* Description: This filees contains rf4ce calls that uapi uses to
			   communicate with the RF4CE hardware.
*********************************************************************************/
#include "uapi.h"
#include <string.h>
/******************************************************************************************************************
Function: UAPI_RF4CESend()
Description :
******************************************************************************************************************/
extern void RTI_SendDataReq( UINT8 dstIndex, UINT8 profile, UINT16 vendorId, UINT8 txOptions, UINT8 length, UINT8* data);
INT16 UAPI_RF4CESend(UAPIDataPkt *uapiDataPkt)
{
unsigned char data[255];
VK_memcpy(data, &uapiDataPkt->uapiHdr, sizeof(UAPIHdr_t));
VK_memcpy(data+sizeof(UAPIHdr_t), uapiDataPkt->data, uapiDataPkt->dataLength);
RTI_SendDataReq( uapiDataPkt->remoteID,
                 0x01,
                 0xF2FF,
                 0x64,
                 uapiDataPkt->dataLength+sizeof(UAPIHdr_t),
                 data);
	return UAPI_SUCCESS ;
}
