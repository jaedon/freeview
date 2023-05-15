/*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : rootparams.h
 *
 *----------------------------------------------------------------------*/

#ifndef ROOT_PARAMS_H
#define ROOT_PARAMS_H

/*
* The CPE parameters are represented by a tree where each node in the
 * tree is an array of TRxObjNode structures. Each array item represents
 * either a paramater item or an object containing a pointer to the next
 * level of TRxObjNode items that make up  parameters that make up the object.
 * 
 * Each item that describes an object contains a pointer component, *objDetail,
 * that points to the array of parameter and object items that form the
 * object. 
 * The function pointer, getTRxParam, may be NULL or point to
 * a hardware dependent function that returns the value of the parameter in
 * a string variable. 
 * The function pointer, setTRxParam, is used to call a hardware dependent
 * function to set the parameter. If the function pointer is NULL the
 * parameter is not writeable. If the item is an object and the pointer is
 * NULL the rpc function AddObject/DeleteObject are not supported by that
 * item.
 *
 * The global variable thisTRxObjNode points to the current TRxObjNode entry
 * when the set/getTRxParam functions are called.
 * 
 * If the node contains a single TRxObjNode item with TRxType of tInstance
 * then this item represents all instances of the object. This function is
 * responsible for keeping track of all instances associated with this
 * object. The parameter handling framework expects the following functionality
 * of the getTRxParam function which will need to maintain state information
 * as the framework accesses the instances. The framework will always call
 * the getTRxParam to access an instance on its way to access its 
 * sub-objects/parameters. As the framework is traversing the parameter
 * tree it will call the getTRxParam function with one of the following
 * forms to validate and set the instance state before proceeding
 * to the next object/parameter level.
 * 
 * To determine the existance of a specific instance:
 *  The getTRxParam function is called as follows:
 *                     
 *  node->getTRXParam(char **ParamInstance)
 *  If *ParamInstance is not NULL then it points to the value to be found.
 *  Returns: TRx_OK if ParameterInstance found. The function should set
 *                  a global state variable for use by the next level
 *                  get/setTRxParam functions to the ParameterInstance.
 *           TRx_ERR if ParameterInstance not found
 * 
 *  To retrieve each of the instances in order:
 *  If *ParamInstance is NULL then it returns the first instance of the
 *  object.
 *  Returns: TRx_OK if any Instances exist. The *ParamInstance pointer points to the
 *                  name (instance number string) of the first instance.
 *                  The global instance state variable is set to the 
 *                  instance returned in the value string.
 *           TRx_ERR no instances of  this object exist.
 * 
 *  If *ParamInstance is (void *)(-1) then find the next instance relative
 * to the last instance returned.
 *  Returns: TRx_OK The *ParamInstance pointer points to the next instance. 
 *                  instance name. Repeated calls with the returned 
 *                  instance name from the previous call as the InstanceValue
 *                  will return all instances. The global instance state
 *                  variable is the instance returned in the value string.
 *          TRx_ERR no more instances.
 * See xxx for an example of how this is coded.
 */

#include "../inc/tr69cdefs.h"

/* InternetGatewayDevice. */
SVAR(InternetGatewayDevice);

SVAR(LANDeviceNumberOfEntries);
SVAR(WANDeviceNumberOfEntries);

/* InternetGatewayDevice.DeviceSummary */
SVAR(DeviceSummary);

/* InternetGatewayDevice.DeviceInfo. */
SVAR(DeviceInfo);

/* InternetGatewayDevice.ManagementServer. */
SVAR(ManagementServer);

/* InternetGatewayDevice.UserInterface */
SVAR(UserInterface);

/* InternetGatewayDevice.Layer3Forwarding. */
SVAR(Layer3Forwarding);

/* InternetGatewayDevice.Services. */
SVAR(Services);

/* InternetGatewayDevice.LANDevice. */
SVAR(LANDevice);

/* InternetGatewayDevice.WANDevice. */
SVAR(WANDevice);

/* InternetGatewayDevice.Time. */
SVAR(Time);

/* InternetGatewayDevice.IPPingDiagnostics. */
SVAR(IPPingDiagnostics);

#ifdef PROPRIETARY    
/* InternetGatewayDevice.X_BROADCOM_COM. */
SVAR(X_BROADCOM_COM);
#endif   /* PROPRIETARY	*/

#endif   /* ROOT_PARAMS_H */
