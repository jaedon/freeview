#include <stdlib.h>
#include <stdio.h>
#include "uapi.h"

void general_parameters(UAPI_configParamStruct_t* myThing)
{
    int j;
        UINT32 cpudata = 0;
        UINT8 uapienable = 0;
        UINT32 version = 0;
        UINT32 pollrate = 0;
        UINT8 pollstate = 0;
        UAPI_productInfo_t productInfo;

        if(UAPI_SUCCESS == UAPI_getParam(myThing, UapiEnbState, (void*)&uapienable))
            VK_fprintf(VK_STDERR, "UAPI ENABLE = %d\n", uapienable);
        if(UAPI_SUCCESS == UAPI_getParam(myThing, UapiVersionInfo, (void*)&version))
            VK_fprintf(VK_STDERR, "Version: %d.%d.%d\n",
                    ((version>>0)&0xFF),
                    ((version>>8)&0xFF),
                    ((version>>16)&0xFF)|((version>>16)&0xFF));

        if(UAPI_SUCCESS == UAPI_getParam(myThing,UapiProductInfo, (void*)&productInfo))
        {
            VK_fprintf(VK_STDERR, "UAPI Version: %d.%d.%d\n",
                    ((productInfo.swVersion>>0 )&0xFF),
                    ((productInfo.swVersion>>8 )&0xFF),
                    ((productInfo.swVersion>>16)&0xFF)|((productInfo.swVersion>>16)&0xFF));
                VK_fprintf(VK_STDERR, "UAPI Product Name: ");
                for(j=0;j<MAX_PRODUCT_NAME;j++)
                    VK_fprintf(VK_STDERR, "%c", productInfo.productName[j]);
                VK_fprintf(VK_STDERR, "\n");
        }
        if(UAPI_SUCCESS == UAPI_getParam(myThing, UapiPollingRate, (void*)&pollrate))
        {
                VK_fprintf(VK_STDERR, "UAPI Polling Rate: %02X%02X%02X%02X\n",
                    ((pollrate>>24)&0xFF),
                    ((pollrate>>16)&0xFF),
                    ((pollrate>>8)&0xFF),
                    ((pollrate)&0xFF));
        }
        if(UAPI_SUCCESS == UAPI_getParam(myThing, UapiAutoPollState, &pollstate))
                VK_fprintf(VK_STDERR, "UAPI Auto Poll State : %d\n", pollstate);

        if(UAPI_SUCCESS == UAPI_getParam(myThing, UapiCPUHwInfo, &cpudata))
        {
            VK_fprintf(VK_STDERR, "HW INFO- REV: %02X\n", (cpudata>>24)&0xFF);
            VK_fprintf(VK_STDERR, "HW INFO- IR:  %02X\n",((cpudata>>16)&0xFF));
            VK_fprintf(VK_STDERR, "HW INFO- RF:  %02X\n",((cpudata>>8)&0xFF));
            VK_fprintf(VK_STDERR, "HW INFO- CPU: %02dMHz\n",((cpudata)&0xFF)*4);
        }
}
void relative_motion_settings( UAPI_configParamStruct_t* myThing )
{
int i;
for(i=0;i<myThing->numParams;i++)
{
switch( myThing->param[i].attributeId )
{
case RelMotionFirstPhysicalToLogical:
    VK_fprintf(VK_STDERR, "First Physical to Logical : %d\n", myThing->param[i].value.UINT16_data);
break;
case RelMotionDecelPhysicalToLogical:
    VK_fprintf(VK_STDERR, "Decel Physical to Logical : %d\n", myThing->param[i].value.UINT16_data);
break;
case RelMotionEnable:
    VK_fprintf(VK_STDERR, "Relative Motion Enable State : %d\n", myThing->param[i].value.UINT8_data);
break;

case RelMotionAccelEnable:
    VK_fprintf(VK_STDERR, "Relative Motion Acceleration Enable State: %d\n", myThing->param[i].value.UINT8_data);
break;

case RelMotionDecelEnable:
    VK_fprintf(VK_STDERR, "Relative Motion Deceleration Enable State: %d\n", myThing->param[i].value.UINT8_data);
break;

case RelMotionFingerPresenceNotifyEnable:
    VK_fprintf(VK_STDERR, "Finger Present Notification Enable State: %d\n", myThing->param[i].value.UINT8_data);
break;

case RelMotionPhysicalToLogical:
    VK_fprintf(VK_STDERR, "Relative Motion Physical To Logical: %d\n", myThing->param[i].value.UINT16_data);
break;

case RelMotionTraverseTime:
    VK_fprintf(VK_STDERR, "Relative Motion Traverse Time (in ms): %d\n", myThing->param[i].value.UINT16_data);
break;

case RelMotionDecelCoefficient:
    VK_fprintf(VK_STDERR, "Relative Motion Deceleration Coefficient: %d\n", myThing->param[i].value.UINT16_data);
break;

case RelMotionDecelSampleTime:
    VK_fprintf(VK_STDERR, "Relative Motion Deceleration Sample Time (in ms): %d\n", myThing->param[i].value.UINT16_data);
break;

case RelMotionTxIntervalMs:
    VK_fprintf(VK_STDERR, "Relative Motion Transmit Interval Time (in ms): %d\n", myThing->param[i].value.UINT16_data);
break;
}
}
}

void absolute_position_settings(UAPI_configParamStruct_t* myThing)
{
    int i;
    for(i=0;i<myThing->numParams;i++)
    {
    switch(myThing->param[i].attributeId )
    {
        case AbsMotionEnable:
            VK_fprintf(VK_STDERR, "Absolute Motion Enable State: %d\n", myThing->param[i].value.UINT8_data);
        break;
        case AbsMotionTxIntervalMs:
            VK_fprintf(VK_STDERR, "Absolute Motion Transmit Interval Time (in ms): %d\n", myThing->param[i].value.UINT16_data);
        break;
    }
    }
}


void settings( void* data)
{
    int i;
    UAPI_remoteParams_t* inData = (UAPI_remoteParams_t*)data;
    UAPI_configParamStruct_t* myThing = (UAPI_configParamStruct_t*)inData->remoteParams;
    VK_fprintf(VK_STDERR, "param struct is for feature %02X\n", myThing->featureId);
    VK_fprintf(VK_STDERR, "param struct has %d parameters\n", myThing->numParams);
    switch(myThing->featureId)
    {
        case UAPIFeatureIDGeneralServices:
	    general_parameters(myThing);
	break;
	case UAPIFeatureIDRelativeMotion:
	    relative_motion_settings(myThing);
	break;
	case UAPIFeatureIDAbsolutePosition:
	    absolute_position_settings(myThing);
	break;
    }
    for(i=0;i<myThing->numParams;i++)
    {
        switch(myThing->featureId)
        {
            case UAPIFeatureIDBatteryStatus:
            switch(myThing->param[i].attributeId )
            {
                case BatteryStatusEnable:
                    VK_fprintf(VK_STDERR, "Battery Status Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;
            }
           break;
            case UAPIFeatureIDOrientation:
            switch(myThing->param[i].attributeId )
            {
                case OrientationEnable:
                    VK_fprintf(VK_STDERR, "Orientation Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;
            }
            break;
            case UAPIFeatureIDUserActivity:
            switch(myThing->param[i].attributeId )
            {
                case UserActivityEnable:
                    VK_fprintf(VK_STDERR, "User Activity Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;
            }
            break;
            case UAPIFeatureIDText:
            switch(myThing->param[i].attributeId )
            {
                case TextEntryEnable:
                    VK_fprintf(VK_STDERR, "Text Entry Enable State: %d\n", myThing->param[i].value.UINT8_data);
                    break;
                case SymShiftModeEnable:
                    VK_fprintf(VK_STDERR, "Text Entry Symbol Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;

                case CapsModeEnable:
                    VK_fprintf(VK_STDERR, "Text Entry CapsLock Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;

                case AltModeEnable:
                    VK_fprintf(VK_STDERR, "Text Entry Alt Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;
            }
            break;
            case UAPIFeatureIDUrcKey:
            switch(myThing->param[i].attributeId )
            {
                case URCKeyEnable:
                    VK_fprintf(VK_STDERR, "URC Key Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;
            }
            break;
            case UAPIFeatureIDRemoteAPI:
            switch(myThing->param[i].attributeId)
            {
                case RemoteAPIEnable:
                    VK_fprintf(VK_STDERR, "RemoteAPI Enable State: %d\n", myThing->param[i].value.UINT8_data);
                break;
            }
            break;
        }
    }
    VK_fprintf(VK_STDERR, "\n");
}


void relative_motion( void* data)
{
    UAPIFeatureData_t* rel_data = (UAPIFeatureData_t*)data;
    UAPI_RelativeMotion_t* relativeMotion_data = (UAPI_RelativeMotion_t*)rel_data->featureData;
    VK_fprintf(VK_STDERR, "button1 = %02X, button2 = %02X, button3 = %02X, fingerPresent = %02X\n",
             relativeMotion_data->buttonInfo.button1,
             relativeMotion_data->buttonInfo.button2,
             relativeMotion_data->buttonInfo.button3,
             relativeMotion_data->buttonInfo.fingerPresent);

    if( (relativeMotion_data->xDisplacement || relativeMotion_data->yDisplacement)
        && relativeMotion_data->verticalScroll == 0 && relativeMotion_data->horizontalPan == 0)
    {
        VK_fprintf(VK_STDERR, "xDisplacement = %d\n", relativeMotion_data->xDisplacement);
        VK_fprintf(VK_STDERR, "yDisplacement = %d\n", relativeMotion_data->yDisplacement);
    }
    else if( (relativeMotion_data->verticalScroll || relativeMotion_data->horizontalPan)
        && (relativeMotion_data->xDisplacement == 0 && relativeMotion_data->yDisplacement == 0))
    {
        VK_fprintf(VK_STDERR, "vertical scroll = %d\n", relativeMotion_data->verticalScroll);
        VK_fprintf(VK_STDERR, "horizontal pan  = %d\n", relativeMotion_data->horizontalPan);
    }
}

void absolute_motion( void* data)
{
    UAPIFeatureData_t* abs_data = (UAPIFeatureData_t*)data;
    UAPI_AbsoluteMotion_t* absoluteMotion_data = (UAPI_AbsoluteMotion_t*)abs_data->featureData;
    VK_fprintf(VK_STDERR, "button1 = %02X, button2 = %02X, button3 = %02X, fingerPresent = %02X\n",
            absoluteMotion_data->buttonInfo.button1,
            absoluteMotion_data->buttonInfo.button2,
            absoluteMotion_data->buttonInfo.button3,
            absoluteMotion_data->buttonInfo.fingerPresent);
    VK_fprintf(VK_STDERR, "xPosition = %d\n", absoluteMotion_data->xPosition);
    VK_fprintf(VK_STDERR, "yPosition = %d\n", absoluteMotion_data->yPosition);
}
void text(void* data)
{
    UAPIFeatureData_t* text_data = (UAPIFeatureData_t*)data;
    UAPI_TextEntry_t* textInput = (UAPI_TextEntry_t*)text_data->featureData;
    switch(textInput->charType)
    {
        case 4:
        case 1:
            VK_fprintf(VK_STDERR, "key [%c] was pressed\n", textInput->value.UTF8char[0]);
        break;
        case 2:
        case 3:
        break;
    }

}
void RawSensor(void* data)
{
     int i;
     UAPIFeatureData_t* uapi_data = (UAPIFeatureData_t*)data;
     UAPI_RawSensorData_t* rawSensor_data = (UAPI_RawSensorData_t*)uapi_data->featureData;
            VK_fprintf(VK_STDERR,"Sequence Number = %d\n", rawSensor_data->seqNum);
            VK_fprintf(VK_STDERR,"Button          = %d, %d\n", rawSensor_data->button.button_pressed, rawSensor_data->button.button_released);
            VK_fprintf(VK_STDERR,"length of data is %d\n", rawSensor_data->length);
            for(i=0;i<rawSensor_data->length;i++)
                VK_fprintf(VK_STDERR,"%02X ", rawSensor_data->sensorData[i]);
            VK_fprintf(VK_STDERR,"\n\r");

}
void battery(void* data)
{
    UAPIFeatureData_t* battery_data = (UAPIFeatureData_t*)data;
    UAPI_BatteryStatus_t* battery_status = (UAPI_BatteryStatus_t*)battery_data->featureData;
    VK_fprintf(VK_STDERR, "battery status = %x\n", battery_status->status);
    VK_fprintf(VK_STDERR, "battery value = %x\n", battery_status->voltage);
}

