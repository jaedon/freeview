#include "uapi.h"
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
typedef struct{
    int length;
    unsigned char response[1040];
}QS_response_t;
QS_response_t global_response;


int test_rctransport(unsigned char* data, int length, unsigned char* response);

unsigned char T0810[] = {
0x00, 0x1a, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0xdb, 0x01, 0x04, 0x00, 0x65, 0x00,
0x59, 0x02, 0x4b, 0x00, 0x00, 0x9c, 0x1c, 0x5c, 0x7c, 0xbc };

extern sem_t QS_semaphore;
void my_add_crc(unsigned char* buffer)
{
    char workstring[5];
    int i = 0;
    int length = 0;
    VK_sprintf(workstring, "%02X%02X", buffer[0], buffer[1]);
    length = strtol(workstring, NULL, 16);
    buffer[length+1] = 0x00;
    for(i=0;i<length+1;i++)
    {
        buffer[length+1]^=buffer[i];
    }
}


void download(unsigned char* codeset, int length)
{
    int mylength = 0;
    unsigned char response[1040];
    unsigned char global_message[1000];
    VK_memset(response, '\0', 1040);
    global_message[0] = (((length+2)>>8)&0xFF);
    global_message[1] = ((length+2)&0xFF);
    global_message[2] = 0x82;
    VK_memcpy(global_message+3, codeset, length);
    global_message[length+4] = 0x00;
    my_add_crc(global_message);
    mylength = test_rctransport(global_message, length+4, response);
    VK_fprintf(VK_STDERR, "length = %d\n", mylength);
    if(mylength > 0)
    {
    if(response[2] == 0x00)
        VK_fprintf(VK_STDERR, "download success\n");
    else
        VK_fprintf(VK_STDERR, "download status %02X\n", response[2]);
    }
    else
        VK_fprintf(VK_STDERR, "download failed\n");
}


int test_rctransport(unsigned char* data, int length, unsigned char* response)
{
    int error = 0;
    int counter = 0;
    int i;
    UAPIQSSendToRemote(data,length, UAPI_RF4CE_NIF, 0);

    for(i=0;i<length;i++)
        VK_fprintf(VK_STDERR, "%02X ", data[i]);
    VK_fprintf(VK_STDERR, "\n");
    while(1)
    {
        if( 0 == sem_trywait(&QS_semaphore))
        {
            error = 0;
            break;
        }
        error = errno;
        if(error != EAGAIN)
            VK_fprintf(VK_STDERR, "something really bad happened\n");
        sleep(1);
        counter++;
        if(counter == 50)
        {
            error = ETIMEDOUT;
            break;
        }
    }
    if(error == ETIMEDOUT)
    {
        VK_fprintf(VK_STDERR, "timed out error\n");
        response[0] = 0x00;
        response[1] = 0x02;
        response[2] = 0xB3;
        response[3] = 0xB1;
        return 4;
    }
    else
        VK_fprintf(VK_STDERR, "error = %d\n", error);
    if(error == 0)
    {
        if(global_response.length > 0)
        {
            VK_memcpy(response, global_response.response, global_response.length);
            return global_response.length;
        }
        else return 0;
    }
    return 0;
}

void test_code(void)
{
    int length = 0;
    unsigned char response[1040];
    unsigned char global_message[10];
    global_message[0] = 0x00;
    global_message[1] = 0x05;
    global_message[2] = 0x8F;
    global_message[3] = 0x01;
    global_message[4] = 0x03;
    global_message[5] = 0x2A;
    global_message[6] = 0x00;
    my_add_crc(global_message);
    length = test_rctransport(global_message, 7, response);
    if(length > 0)
    {
        int i;
        for(i=0;i<length;i++)
            VK_fprintf(VK_STDERR, "%02X ", response[i]);
        VK_fprintf(VK_STDERR, "\n");
    if(response[2] == 0x00)
        VK_fprintf(VK_STDERR, "test mode enabled success\n");
    }
}

void QuickSet( void* data )
{
    UAPIFeatureData_t* myData = (UAPIFeatureData_t*)data;
    UAPI_RemoteAPI_t* remoteAPI_data = (UAPI_RemoteAPI_t*)myData->featureData;
    global_response.length = 0;
    VK_memset(global_response.response, '\0', 1040);
    global_response.length = remoteAPI_data->recordLength;
    VK_memcpy(global_response.response, remoteAPI_data->remoteAPIdata, remoteAPI_data->recordLength);
    sem_post(&QS_semaphore);
}

void download_and_test(void)
{
    VK_fprintf(VK_STDERR, "******************************************* DOWNLOAD START ***************************************\n");
    VK_fprintf(VK_STDERR, "press ok/select\n");
    download(T0810,sizeof(T0810));
    test_code();
VK_fprintf(VK_STDERR, "******************************************* DOWNLOAD DONE ****************************************\n");

}

