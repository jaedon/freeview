#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/select.h>
#include <unistd.h>
#include <pthread.h>
#include "callbacks.h"
#include "uapi.h"
#include <semaphore.h>


#define PORT	"/dev/ttyACM0"

#define REMOTE_0 0
int rem_id = 0x00;

// RTI API Status Values
// Application framework layer primitive status field values
#define RTI_SUCCESS                                      0
#define RTI_ERROR_INVALID_INDEX                          0xF9
#define RTI_ERROR_INVALID_PARAMETER                      0xE8
#define RTI_ERROR_UNSUPPORTED_ATTRIBUTE                  0xF4
#define RTI_ERROR_NO_ORG_CAPACITY                        0xB0
#define RTI_ERROR_NO_REC_CAPACITY                        0xB1
#define RTI_ERROR_NO_PAIRING_INDEX                       0xB2
#define RTI_ERROR_NO_RESPONSE                            0xB3
#define RTI_ERROR_NOT_PERMITTED                          0xB4
#define RTI_ERROR_FRAME_COUNTER_EXPIRED                  0xB6
#define RTI_ERROR_DISCOVERY_ERROR                        0xB7
#define RTI_ERROR_DISCOVERY_TIMEOUT                      0xB8
#define RTI_ERROR_SECURITY_TIMEOUT                       0xB9
#define RTI_ERROR_SECURITY_FAILURE                       0xBA
#define RTI_ERROR_NO_SECURITY_KEY                        0xBD
#define RTI_ERROR_OUT_OF_MEMORY                          0xBE
#define RTI_ERROR_OSAL_NO_TIMER_AVAIL                    0x08
#define RTI_ERROR_OSAL_NV_OPER_FAILED                    0x0A
#define RTI_ERROR_OSAL_NV_ITEM_UNINIT                    0x09
#define RTI_ERROR_OSAL_NV_BAD_ITEM_LEN                   0x0C
#define RTI_ERROR_MAC_TRANSACTION_EXPIRED                0xF0
#define RTI_ERROR_MAC_TRANSACTION_OVERFLOW               0xF1
#define RTI_ERROR_MAC_NO_RESOURCES                       0x1A
#define RTI_ERROR_MAC_UNSUPPORTED                        0x18
#define RTI_ERROR_MAC_BAD_STATE                          0x19
#define RTI_ERROR_MAC_CHANNEL_ACCESS_FAILURE             0xE1
#define RTI_ERROR_MAC_NO_ACK                             0xE9
#define RTI_ERROR_MAC_BEACON_LOST                        0xE0
#define RTI_ERROR_MAC_PAN_ID_CONFLICT                    0xEE
#define RTI_ERROR_MAC_SCAN_IN_PROGRESS                   0xFC
#define RTI_ERROR_UNKNOWN_STATUS_RETURNED                0x20
#define RTI_ERROR_FAILED_TO_DISCOVER                     0x21
#define RTI_ERROR_FAILED_TO_PAIR                         0x22
#define RTI_ERROR_ALLOW_PAIRING_TIMEOUT                  0x23
#define RTI_ERROR_PAIR_COMPLETE                          0x24
#define RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT                0xFF


typedef unsigned short uint16_t;
typedef unsigned char uint8_t ;


#define RTI_CP_ITEM_STARTUP_CTRL       0xA0
#define RTI_CP_ITEM_NODE_CAPABILITIES  0xA1
#define RTI_CP_ITEM_APPL_CAPABILITIES  0xA3
#define RTI_CP_ITEM_APPL_DEV_TYPE_LIST 0xA4
#define NP_MAX_BUF_LEN                  128
#define NP_MAX_RESP_SIZE                255

sem_t QS_semaphore;
static sem_t rf_sync_sem;
static sem_t rf_async_sem;
uint8_t RTI_ReadItem( uint8_t itemId, uint8_t len, uint8_t *response );
uint8_t RTI_WriteItem( uint8_t itemId, uint8_t len, uint8_t * value);
void RTI_PairReq();
void RTI_InitReq();

static pthread_mutex_t sg_usb_mutex = PTHREAD_MUTEX_INITIALIZER;
int g_usb_dongle = 0;

typedef struct
{
    uint8_t length;
    uint8_t message[NP_MAX_RESP_SIZE];
}RF4CE_Message_t;


static RF4CE_Message_t SyncMessage;
static RF4CE_Message_t AsyncMessage;
void UAPIProcessReceivedData( unsigned char* data, int length, unsigned short deviceID ,unsigned char remoteID) //
{
    UAPIDataPkt* uapiDataInd;
    uapiDataInd = VK_malloc(sizeof(UAPIDataPkt)+length-sizeof(UAPIHdr_t));
    uapiDataInd->remoteID   = remoteID;
    uapiDataInd->deviceID   = deviceID;
    uapiDataInd->dataLength = length-sizeof(UAPIHdr_t);
    uapiDataInd->data = (uint8_t*)uapiDataInd+sizeof(UAPIDataPkt);
    VK_memcpy( &uapiDataInd->uapiHdr, data, sizeof(UAPIHdr_t));
    VK_memcpy( uapiDataInd->data, data+sizeof(UAPIHdr_t), uapiDataInd->dataLength);
    UAPIDeMultiplexer(uapiDataInd);//entry point
    VK_free(uapiDataInd);
}

static void add_crc( uint8_t* buffer, uint8_t length)
{
    int i;
    if(length <= 1)
        return;
    buffer[length-1] = 0x00;
    for(i=1;i<length-1;i++)
        buffer[length-1]^=buffer[i];
}

void RTI_PairReq()
{
    uint8_t pair_command[] = { 0xFE, 0x00, 0x4A, 0x06, 0x4C };
    pthread_mutex_lock(&sg_usb_mutex);
    write(g_usb_dongle, pair_command, sizeof(pair_command));
    VK_fprintf(VK_STDERR, "pairing?\n");
    sem_wait(&rf_async_sem);
    switch(AsyncMessage.message[3])
    {
        case RTI_SUCCESS:
                VK_fprintf(VK_STDERR, "dstIndex = %02X, devType = %02X\n", AsyncMessage.message[4], AsyncMessage.message[5]);
        break;
        case RTI_ERROR_NOT_PERMITTED:
            VK_fprintf(VK_STDERR, "PAIRING NOT PERMITTED\n");
        break;
        case  RTI_ERROR_OUT_OF_MEMORY:
            VK_fprintf(VK_STDERR, "PAIRING out of memory\n");
        break;
        case  RTI_ERROR_MAC_TRANSACTION_EXPIRED:
            VK_fprintf(VK_STDERR, "PAIRING MAC transaction expired\n");
        break;
        case  RTI_ERROR_MAC_TRANSACTION_OVERFLOW:
            VK_fprintf(VK_STDERR, "PAIRING MAC transaction overflow\n");
        break;
        case  RTI_ERROR_MAC_NO_RESOURCES:
            VK_fprintf(VK_STDERR, "PAIRING MAC no resources\n");
        break;
        case  RTI_ERROR_MAC_UNSUPPORTED:
            VK_fprintf(VK_STDERR, "PAIRING MAC unsupported\n");
        break;
        case  RTI_ERROR_MAC_BAD_STATE:
            VK_fprintf(VK_STDERR, "PAIRING MAC bad state\n");
        break;
        case  RTI_ERROR_MAC_CHANNEL_ACCESS_FAILURE:
            VK_fprintf(VK_STDERR, "PAIRING MAC channel access failure\n");
        break;
        case  RTI_ERROR_MAC_NO_ACK:
            VK_fprintf(VK_STDERR, "PAIRING MAC no ack\n");
        break;
    }
    pthread_mutex_unlock(&sg_usb_mutex);
}

uint8_t RTI_ReadItem( uint8_t itemId, uint8_t len, uint8_t *response )
{
    uint8_t read_command[7] = {0xFE, 0x02, 0x2A, 0x01, 0x00, 0x00, 0x00};
    read_command[4] = itemId;
    read_command[5] = len;
    add_crc(read_command, 7);
    pthread_mutex_lock(&sg_usb_mutex);
    write(g_usb_dongle, read_command, sizeof(read_command));
    sem_wait(&rf_sync_sem);
    VK_memset(response, '\0', NP_MAX_RESP_SIZE);
    VK_memcpy(response, SyncMessage.message, SyncMessage.length);
    pthread_mutex_unlock(&sg_usb_mutex);
    return SyncMessage.length;
}

uint8_t RTI_WriteItem( uint8_t itemId, uint8_t len, uint8_t* value)
{
    if( (len>0) && (value != NULL) )
    {
        uint8_t write_command[NP_MAX_RESP_SIZE] = { 0xFE, 0x00, 0x2A, 0x02 };
        write_command[1] = 0x02+len;
        write_command[4] = itemId;
        write_command[5] = len;
        VK_memcpy(write_command+6, value, len);
        add_crc(write_command, write_command[1]+5);
        pthread_mutex_lock(&sg_usb_mutex);
        write(g_usb_dongle, write_command, write_command[1]+5);
        sem_wait(&rf_sync_sem);
        pthread_mutex_unlock(&sg_usb_mutex);
        return SyncMessage.message[3];
    }
    return 0x05;
}
int init_usb_serial(int device_handle)
{
        struct termios options;                         //serial port settings
        device_handle = open(PORT, O_RDWR );
        if (device_handle < 0)
        {
                VK_fprintf(VK_STDERR, "can't open %s, are you sure you have permission to use it?\n", PORT);
                return -1;
        }
        options.c_cflag = B115200 | CS8| CREAD | CLOCAL;
        options.c_iflag = IGNPAR;
        options.c_oflag = 0;
        options.c_lflag = 0;
        options.c_cc[VMIN]=1;
        options.c_cc[VTIME]=0;
        tcflush(device_handle, TCIFLUSH);
        tcsetattr(device_handle,TCSANOW,&options);
        return device_handle;
}
#define SOF_STATE  0
#define LEN_STATE  1
#define CMD0_STATE 2
#define CMD1_STATE 3
#define DATA_STATE 4
#define FCS_STATE  5
#define SOF 0xFE
#define HEADER_LEN 3

static unsigned char calcFCS(unsigned char *pMsg, unsigned char len)
{
  unsigned char result = 0;
  while (len--)
  {
    result ^= *pMsg++;
  }
  return result;
}
void RTI_SendDataReq( uint8_t dstIndex,
                      uint8_t profileId,
                      uint16_t vendorId,
                      uint8_t txOptions,
                      uint8_t len,
                      uint8_t* data)
{
    uint8_t out[NP_MAX_RESP_SIZE] = { 0xFE, 0x00, 0x4A, 0x05 };
    out[0] = 0xFE;
    out[1] = len+6;
    out[2] = 0x4A;
    out[3] = 0x05;
    out[4] = dstIndex;
    out[5] = profileId;
    out[6] = (vendorId>>8)&0xFF;
    out[7] = vendorId&0xFF;
    out[8] = txOptions;
    out[9] = len;
    VK_memcpy(out+10, data, len);
    out[len+10] = calcFCS(out+1, len+9);
    write(g_usb_dongle, out, len+11);
}
#define RTI_INIT_CNF         0x01
#define RTI_SEND_DATA_CNF    0x03
#define RTI_ALLOW_PAIR_CNF   0x04
#define RTI_RECEIVE_DATA_IND 0x05
#define RTI_STANDBY_CNF      0x06
static void ProcessAsynchronousRequest(unsigned char* message, int length)
{
    int i;
    switch( message[2] )
    {
        case RTI_INIT_CNF:
            VK_fprintf(VK_STDERR, "RemoTI Initialized\n");
            AsyncMessage.length = length;
            VK_memcpy(AsyncMessage.message, message, length);
            sem_post(&rf_async_sem);
        break;
        case RTI_ALLOW_PAIR_CNF:
            VK_fprintf(VK_STDERR, "RemoTI Pair Request Confirm\n");
            AsyncMessage.length = length;
            VK_memcpy(AsyncMessage.message, message, length);
            sem_post(&rf_async_sem);
        break;
        case RTI_RECEIVE_DATA_IND:
        if( (message[8]&0x04) >> 2)
        {
            VK_fprintf(VK_STDERR, "received: ");
            for(i=0;i<length;i++)
                VK_fprintf(VK_STDERR, "%02X ", message[i]);
		rem_id = message[3];
            VK_fprintf(VK_STDERR, "\n");
            UAPIProcessReceivedData(message+10, message[9], UAPI_RF4CE_NIF, message[3]);
        }
        else
        {
            for(i=0;i<length;i++)
                VK_fprintf(VK_STDERR, "%02X ", message[i]);
            VK_fprintf(VK_STDERR, "\n");
        }
        break;
        case RTI_SEND_DATA_CNF:
            VK_fprintf(VK_STDERR, "send data success = %d\n", message[3]);
        break;
        case RTI_STANDBY_CNF:
            VK_fprintf(VK_STDERR, "standby success = %d\n", message[3]);
            sem_post(&rf_async_sem);
        break;
    }
}

static void ProcessSynchronousRequest(unsigned char* SyncRequest, int length)
{
    VK_fprintf(VK_STDERR, "Sync Request\n");
}

static void ProcessSynchronousResponse(unsigned char* SyncResponse, int length)
{
    SyncMessage.length = length;
    VK_memcpy(SyncMessage.message, SyncResponse, length);
    sem_post(&rf_sync_sem);
}

#define SREQ 0x01
#define AREQ 0x02
#define SRSP 0x03

static void ProcessMessage(unsigned char* message, int length)
{
    switch( (message[1]&0xE0) >> 5)
    {
        case SREQ:
            ProcessSynchronousRequest(message, length);
        break;
        case AREQ:
            ProcessAsynchronousRequest(message, length);
        break;
        case SRSP:
            ProcessSynchronousResponse(message, length);
        break;
    }
}

static void* ReceiveMessage()
{
	unsigned char byte_index = 0;
	unsigned char message[136];
	int current_state = SOF_STATE;
	uint8_t data[1];
	char bytes_left = 0;
	while(1)
	{
		if( 1 == read(g_usb_dongle, data, 1))
		{
			switch( current_state )
			{
				case SOF_STATE:
				byte_index = 0;
				if(SOF == data[0]) //found start of frame byte, next byte should be length
					current_state = LEN_STATE;
				break;
				case LEN_STATE:
					if( ((char)data[0]>=0) && ((char)data[0]<=123))//validate that length is valid
					{
						message[byte_index++]=bytes_left=data[0];
						current_state = CMD0_STATE;
					}
					else
						current_state = SOF_STATE; //reset back to looking for start byte
				break;
				case CMD0_STATE:
                    if( ((((data[0]&0xE0)>>5)>0) && (((data[0]&0xE0)>>5)<=3)) && //validate it's either SREQ, AREQ, SRSP
					    ((data[0]&0x1F) == 0x0A))   //validate it's a RemoTI Application interface message
                    {
                        message[byte_index++] = data[0];
						current_state = CMD1_STATE;//get ready for the RemoTI API command ID.
                    }
					else
						current_state = SOF_STATE; //reset back to looking for start byte
				break;
				case CMD1_STATE:
					message[byte_index++] = data[0]; //grab RemoTI API command ID
					current_state = DATA_STATE; //prepare for incoming data
				break;
				case DATA_STATE:
					bytes_left--; //count down number of bytes of data expected
					message[byte_index++] = data[0];
					if(0 == bytes_left)
						current_state = FCS_STATE; //data is complete, checksum message to validate
				break;
				case FCS_STATE:
					if( calcFCS(message,HEADER_LEN+message[0]) == data[0]) //validate that checksum matches
						ProcessMessage(message,byte_index);//good message, please process
					current_state = SOF_STATE;
				break;
                default:
                    current_state = SOF_STATE;
                break;
			}
		}
	}
    return 0;
}


void RTI_InitReq()
{
    unsigned char init[] = {0xFE, 0x00, 0x4A, 0x03, 0x49 };
    pthread_mutex_lock( &sg_usb_mutex);
    if( sizeof(init) != write(g_usb_dongle, init, sizeof(init)))
        VK_fprintf(VK_STDERR, "could not initialize RemoTI\n");
    sem_wait(&rf_async_sem);
    VK_fprintf(VK_STDERR, "init done\n");
    pthread_mutex_unlock( &sg_usb_mutex);
}
static void print_buffer(uint8_t *buffer, int size)
{
    int i;
    for(i=0;i<size;i++)
        VK_fprintf(VK_STDERR, "%02X ", buffer[i]);
    VK_fprintf(VK_STDERR, "\n");
}
static void RTI_StandbyReq(uint8_t mode)
{
    uint8_t standby[6] = { 0xFE, 0x01, 0x4A, 0x07, 0x00, 0x00 };
    standby[4] = mode;
    add_crc(standby, 6);
    pthread_mutex_lock( &sg_usb_mutex);
    if( sizeof(standby) != write(g_usb_dongle, standby, sizeof(standby)))
        VK_fprintf(VK_STDERR, "could not disable power save\n");
    sem_wait(&rf_async_sem);
    VK_fprintf(VK_STDERR, "standby %d\n", mode);
    pthread_mutex_unlock(&sg_usb_mutex);
}
static pthread_t receive_thread;
int init_rf4ce()
{
    unsigned char response[NP_MAX_RESP_SIZE];
    uint8_t wakeup[1] = {0xFF};
    uint8_t value;
    int i;
    int ret = 0;
    g_usb_dongle = init_usb_serial(g_usb_dongle);
    if(g_usb_dongle <= 0)
        return -1;

    for(i=0;i<100;i++)
        write(g_usb_dongle, wakeup, sizeof(wakeup));
    sem_init(&QS_semaphore, 0, 0);
    sem_init(&rf_async_sem, 0, 0);
    sem_init(&rf_sync_sem, 0, 0);
    pthread_create(&receive_thread, NULL, &ReceiveMessage, NULL);

    ret = 0;
    value = 1;
    ret = RTI_WriteItem( RTI_CP_ITEM_STARTUP_CTRL, 1, &value);
    if(ret != RTI_SUCCESS)
        VK_fprintf(VK_STDERR, "write failed\n");

    ret = 0;
    ret = RTI_ReadItem(RTI_CP_ITEM_STARTUP_CTRL, 1, response);
    if(ret)
        print_buffer(response, ret);
    ret = 0;
    ret = RTI_ReadItem(RTI_CP_ITEM_NODE_CAPABILITIES, 1, response);
    if(ret)
        print_buffer(response, ret);
    ret = 0;
    ret = RTI_ReadItem(RTI_CP_ITEM_APPL_CAPABILITIES, 1, response);
    if(ret)
        print_buffer(response, ret);
    ret = 0;
    ret = RTI_ReadItem(RTI_CP_ITEM_APPL_DEV_TYPE_LIST, 1, response);
    if(ret)
        print_buffer(response, ret);
    unsigned char devtypes[2] = { 0x02, 0xFE };
    value = 0x14;
    ret = RTI_WriteItem( RTI_CP_ITEM_APPL_CAPABILITIES, 1, &value);
    if(ret != RTI_SUCCESS)
        VK_fprintf(VK_STDERR, "Write failed\n");
    ret = RTI_WriteItem( RTI_CP_ITEM_APPL_DEV_TYPE_LIST, 2, devtypes);
    if(ret != RTI_SUCCESS)
        VK_fprintf(VK_STDERR, "write failed\n");
    ret = 0;
    ret = RTI_ReadItem( RTI_CP_ITEM_APPL_CAPABILITIES, 1, response);
    if(ret)
        print_buffer(response, ret);
    ret = 0;
    ret = RTI_ReadItem(RTI_CP_ITEM_APPL_DEV_TYPE_LIST, 2, response);
    if(ret)
        print_buffer(response, ret);
    RTI_InitReq();
    RTI_StandbyReq(0);
    return 0;
}

void init_pair()
{
    RTI_PairReq();
}

void enable_relative_motion(int value)
{
    unsigned char UINT8_value = value;
    UAPI_configParamStruct_t* relativeMotionParams = NULL;
    relativeMotionParams = UAPI_createParamStruct(relativeMotionParams, 1, UAPIFeature_RelativeMotion);
    UAPI_setParam(relativeMotionParams, 0, RelMotionEnable, (void*)&UINT8_value, sizeof(UINT8_value));
    UAPIConfigureFeature((void*)relativeMotionParams, UAPI_RF4CE_NIF, 0);
    VK_fprintf(VK_STDERR, "press ok/select on remote\n");
}

void enable_raw_sensor_data(int value)
{
  unsigned char UINT8_value = value;
  UAPI_configParamStruct_t* rawSensorDataParams = NULL;
  rawSensorDataParams = UAPI_createParamStruct(rawSensorDataParams, 1, UAPIFeatureIDRawSensorData);
  UAPI_setParam(rawSensorDataParams, 0, RawSensorDataEnable, (void*)&UINT8_value, sizeof(UINT8_value));
  UAPIConfigureFeature((void*) rawSensorDataParams, UAPI_RF4CE_NIF, rem_id);
  VK_fprintf(VK_STDERR, "press ok/select on remote\n");
  sleep(5);
}


void RF4CE_Handler( int choice )
{
    switch ( choice ) {
    case 1:
        enable_relative_motion(1);
        break;
    case 2:
        enable_relative_motion(0);
        break;
    case 3:
        VK_fprintf(VK_STDERR, "initiate pairing on remote\n");
        RTI_PairReq();
        break;
    case 4:
        exit(0);
    case 5:
        //Remote 0
        UAPIBatteryStatusRequest(UAPI_RF4CE_NIF, REMOTE_0);
    break;
    case 6:
        UAPIGetRemoteFeatures(UAPI_RF4CE_NIF, REMOTE_0);
    break;
    case 7:
        UAPIGetInfo(UAPIFeature_Core, UAPI_RF4CE_NIF, REMOTE_0);
    break;
    case 8:
        download_and_test();
    break;
    case 9:
        UAPIGetInfo(UAPIFeature_BatteryStatus, UAPI_RF4CE_NIF, REMOTE_0);
    break;

    case 10:
		enable_raw_sensor_data(1);
    break;

	case 11:
		enable_raw_sensor_data(0);
    break;
    default:
        return;
    }
}
extern void QuickSet(void*);
int main()
{
    int choice = 0;
    /* initialize UAPI */
    UAPI_Init();
    UAPIRegisterCallback( UAPI_RELATIVEMOTION_NAME, relative_motion);
    UAPIRegisterCallback( UAPI_ABSOLUTEPOSITION_NAME, absolute_motion);
    UAPIRegisterCallback( UAPI_TEXTENTRY_NAME, text);
    UAPIRegisterCallback( UAPI_BATTERYSTATUS_NAME, battery);
    UAPIRegisterCallback( UAPI_CONFIGURATION_NAME, settings);
    UAPIRegisterCallback( UAPI_REMOTEAPI_NAME, QuickSet);
    /* initialize RF4CE */
    init_rf4ce();

    while(1)
    {
        VK_fprintf(VK_STDERR, "1. Enable Relative Motion\n");
        VK_fprintf(VK_STDERR, "2. Disable Relative Motion\n");
        VK_fprintf(VK_STDERR, "3. Initiate Pairing\n");
        VK_fprintf(VK_STDERR, "4. Exit\n");
        VK_fprintf(VK_STDERR, "5. Get Battery Status\n");
        VK_fprintf(VK_STDERR, "6. Get Feature List\n");
        VK_fprintf(VK_STDERR, "7. Get Remote Information\n");
        VK_fprintf(VK_STDERR, "8. Download T0810 and Test T0810\n");
        VK_fprintf(VK_STDERR, "9. Get Battery Parameters\n");
	 VK_fprintf(VK_STDERR, "10. Enable Raw sensor\n");
	 VK_fprintf(VK_STDERR, "11. Disable Raw sensor\n");
        VK_fprintf(VK_STDERR, "\nYour Option: ");
        VK_scanf("%d", &choice);
        RF4CE_Handler(choice);
        choice = 0;
    }
}
