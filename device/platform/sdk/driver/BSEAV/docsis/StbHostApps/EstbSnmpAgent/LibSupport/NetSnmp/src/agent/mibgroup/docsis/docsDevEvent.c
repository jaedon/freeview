/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.brcm.scalar.conf $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "docsDevEvent.h"

/*
 * Define the OID pointer to the top of the mib tree that we're
 * registering underneath
 */
oid             docsDevEvent_module_oid[] = { 1, 3, 6, 1, 2, 1, 69, 1, 5 };
int             docsDevEvent_module_oid_len =
    sizeof(docsDevEvent_module_oid) / sizeof(oid);
int             docsDevEvent_module_count = 0;


/** Initializes the docsDevEvent module */
void
init_docsDevEvent(void)
{
    static oid      docsDevEvThrottleInhibited_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 5, 4, 0 };
    static oid      docsDevEvThrottleInterval_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 5, 6, 0 };
    static oid      docsDevEvControl_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 5, 1, 0 };
    static oid      docsDevEvThrottleThreshold_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 5, 5, 0 };
    static oid      docsDevEvSyslog_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 5, 2, 0 };
    static oid      docsDevEvThrottleAdminStatus_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 5, 3, 0 };

    DEBUGMSGTL(("docsDevEvent", "Initializing\n"));

    netsnmp_register_read_only_instance(netsnmp_create_handler_registration
                                        ("docsDevEvThrottleInhibited",
                                         get_docsDevEvThrottleInhibited,
                                         docsDevEvThrottleInhibited_oid,
                                         OID_LENGTH
                                         (docsDevEvThrottleInhibited_oid),
                                         HANDLER_CAN_RONLY));
    netsnmp_register_instance(netsnmp_create_handler_registration
                              ("docsDevEvThrottleInterval",
                               do_docsDevEvThrottleInterval,
                               docsDevEvThrottleInterval_oid,
                               OID_LENGTH(docsDevEvThrottleInterval_oid),
                               HANDLER_CAN_RWRITE));
    netsnmp_register_instance(netsnmp_create_handler_registration
                              ("docsDevEvControl", do_docsDevEvControl,
                               docsDevEvControl_oid,
                               OID_LENGTH(docsDevEvControl_oid),
                               HANDLER_CAN_RWRITE));
    netsnmp_register_instance(netsnmp_create_handler_registration
                              ("docsDevEvThrottleThreshold",
                               do_docsDevEvThrottleThreshold,
                               docsDevEvThrottleThreshold_oid,
                               OID_LENGTH(docsDevEvThrottleThreshold_oid),
                               HANDLER_CAN_RWRITE));
    netsnmp_register_instance(netsnmp_create_handler_registration
                              ("docsDevEvSyslog", do_docsDevEvSyslog,
                               docsDevEvSyslog_oid,
                               OID_LENGTH(docsDevEvSyslog_oid),
                               HANDLER_CAN_RWRITE));
    netsnmp_register_instance(netsnmp_create_handler_registration
                              ("docsDevEvThrottleAdminStatus",
                               do_docsDevEvThrottleAdminStatus,
                               docsDevEvThrottleAdminStatus_oid,
                               OID_LENGTH
                               (docsDevEvThrottleAdminStatus_oid),
                               HANDLER_CAN_RWRITE));
}

int
get_docsDevEvThrottleInhibited(netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{
    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    int             retVal = SNMP_ERR_NOERROR;

    u_char          docsDevEvThrottleInhibited_var[STRINGMAX];
    size_t          docsDevEvThrottleInhibited_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length,
                  docsDevEvThrottleInhibited_var,
                  &docsDevEvThrottleInhibited_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     docsDevEvThrottleInhibited_var,
                                     docsDevEvThrottleInhibited_varLen);
        else
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;


    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return retVal;
}

int
do_docsDevEvThrottleInterval(netsnmp_mib_handler *handler,
                             netsnmp_handler_registration *reginfo,
                             netsnmp_agent_request_info *reqinfo,
                             netsnmp_request_info *requests)
{
    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    int             retVal = SNMP_ERR_NOERROR;

    u_char          docsDevEvThrottleInterval_var[STRINGMAX];
    size_t          docsDevEvThrottleInterval_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length,
                  docsDevEvThrottleInterval_var,
                  &docsDevEvThrottleInterval_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     docsDevEvThrottleInterval_var,
                                     docsDevEvThrottleInterval_varLen);
        else
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

        /*
         * SET REQUEST
         *
         */
    case MODE_SET_RESERVE1:
        if (requests->requestvb->type != ASN_INTEGER) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_WRONGTYPE);
            retVal = SNMP_ERR_WRONGTYPE;
        } else {
            retVal = v2Test(requests->requestvb->name,
                            requests->requestvb->name_length,
                            requests->requestvb->val.string,
                            requests->requestvb->val_len,
                            requests->requestvb->type, reqinfo);
            if (retVal != SNMP_ERR_NOERROR)
                netsnmp_set_request_error(reqinfo, requests, retVal);
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        break;

    case MODE_SET_COMMIT:
        /*
         * really do the set! 
         */
        retVal = v2Set(requests->requestvb->name,
                       requests->requestvb->name_length,
                       requests->requestvb->val.string,
                       requests->requestvb->val_len,
                       requests->requestvb->type, reqinfo);
        if (retVal != SNMP_ERR_NOERROR)
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

    case MODE_SET_UNDO:
        /*
         * Can't undo V2 sets! 
         */
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return retVal;
}

int
do_docsDevEvControl(netsnmp_mib_handler *handler,
                    netsnmp_handler_registration *reginfo,
                    netsnmp_agent_request_info *reqinfo,
                    netsnmp_request_info *requests)
{
    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    int             retVal = SNMP_ERR_NOERROR;

    u_char          docsDevEvControl_var[STRINGMAX];
    size_t          docsDevEvControl_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length, docsDevEvControl_var,
                  &docsDevEvControl_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     docsDevEvControl_var,
                                     docsDevEvControl_varLen);
        else
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

        /*
         * SET REQUEST
         *
         */
    case MODE_SET_RESERVE1:
        if (requests->requestvb->type != ASN_INTEGER) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_WRONGTYPE);
            retVal = SNMP_ERR_WRONGTYPE;
        } else {
            retVal = v2Test(requests->requestvb->name,
                            requests->requestvb->name_length,
                            requests->requestvb->val.string,
                            requests->requestvb->val_len,
                            requests->requestvb->type, reqinfo);
            if (retVal != SNMP_ERR_NOERROR)
                netsnmp_set_request_error(reqinfo, requests, retVal);
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        break;

    case MODE_SET_COMMIT:
        /*
         * really do the set! 
         */
        retVal = v2Set(requests->requestvb->name,
                       requests->requestvb->name_length,
                       requests->requestvb->val.string,
                       requests->requestvb->val_len,
                       requests->requestvb->type, reqinfo);
        if (retVal != SNMP_ERR_NOERROR)
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

    case MODE_SET_UNDO:
        /*
         * Can't undo V2 sets! 
         */
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return retVal;
}

int
do_docsDevEvThrottleThreshold(netsnmp_mib_handler *handler,
                              netsnmp_handler_registration *reginfo,
                              netsnmp_agent_request_info *reqinfo,
                              netsnmp_request_info *requests)
{
    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    int             retVal = SNMP_ERR_NOERROR;

    u_char          docsDevEvThrottleThreshold_var[STRINGMAX];
    size_t          docsDevEvThrottleThreshold_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length,
                  docsDevEvThrottleThreshold_var,
                  &docsDevEvThrottleThreshold_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_UNSIGNED,
                                     docsDevEvThrottleThreshold_var,
                                     docsDevEvThrottleThreshold_varLen);
        else
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

        /*
         * SET REQUEST
         *
         */
    case MODE_SET_RESERVE1:
        if (requests->requestvb->type != ASN_UNSIGNED) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_WRONGTYPE);
            retVal = SNMP_ERR_WRONGTYPE;
        } else {
            retVal = v2Test(requests->requestvb->name,
                            requests->requestvb->name_length,
                            requests->requestvb->val.string,
                            requests->requestvb->val_len,
                            requests->requestvb->type, reqinfo);
            if (retVal != SNMP_ERR_NOERROR)
                netsnmp_set_request_error(reqinfo, requests, retVal);
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        break;

    case MODE_SET_COMMIT:
        /*
         * really do the set! 
         */
        retVal = v2Set(requests->requestvb->name,
                       requests->requestvb->name_length,
                       requests->requestvb->val.string,
                       requests->requestvb->val_len,
                       requests->requestvb->type, reqinfo);
        if (retVal != SNMP_ERR_NOERROR)
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

    case MODE_SET_UNDO:
        /*
         * Can't undo V2 sets! 
         */
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return retVal;
}

int
do_docsDevEvSyslog(netsnmp_mib_handler *handler,
                   netsnmp_handler_registration *reginfo,
                   netsnmp_agent_request_info *reqinfo,
                   netsnmp_request_info *requests)
{
    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    int             retVal = SNMP_ERR_NOERROR;

    u_char          docsDevEvSyslog_var[STRINGMAX];
    size_t          docsDevEvSyslog_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length, docsDevEvSyslog_var,
                  &docsDevEvSyslog_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_IPADDRESS,
                                     docsDevEvSyslog_var,
                                     docsDevEvSyslog_varLen);
        else
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

        /*
         * SET REQUEST
         *
         */
    case MODE_SET_RESERVE1:
        if (requests->requestvb->type != ASN_IPADDRESS) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_WRONGTYPE);
            retVal = SNMP_ERR_WRONGTYPE;
        } else {
            retVal = v2Test(requests->requestvb->name,
                            requests->requestvb->name_length,
                            requests->requestvb->val.string,
                            requests->requestvb->val_len,
                            requests->requestvb->type, reqinfo);
            if (retVal != SNMP_ERR_NOERROR)
                netsnmp_set_request_error(reqinfo, requests, retVal);
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        break;

    case MODE_SET_COMMIT:
        /*
         * really do the set! 
         */
        retVal = v2Set(requests->requestvb->name,
                       requests->requestvb->name_length,
                       requests->requestvb->val.string,
                       requests->requestvb->val_len,
                       requests->requestvb->type, reqinfo);
        if (retVal != SNMP_ERR_NOERROR)
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

    case MODE_SET_UNDO:
        /*
         * Can't undo V2 sets! 
         */
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return retVal;
}

int
do_docsDevEvThrottleAdminStatus(netsnmp_mib_handler *handler,
                                netsnmp_handler_registration *reginfo,
                                netsnmp_agent_request_info *reqinfo,
                                netsnmp_request_info *requests)
{
    /*
     * We are never called for a GETNEXT if it's registered as a
     * "instance", as it's "magically" handled for us.  
     */

    /*
     * a instance handler also only hands us one request at a time, so
     * we don't need to loop over a list of requests; we'll only get one. 
     */

    int             retVal = SNMP_ERR_NOERROR;

    u_char          docsDevEvThrottleAdminStatus_var[STRINGMAX];
    size_t          docsDevEvThrottleAdminStatus_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length,
                  docsDevEvThrottleAdminStatus_var,
                  &docsDevEvThrottleAdminStatus_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     docsDevEvThrottleAdminStatus_var,
                                     docsDevEvThrottleAdminStatus_varLen);
        else
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

        /*
         * SET REQUEST
         *
         */
    case MODE_SET_RESERVE1:
        if (requests->requestvb->type != ASN_INTEGER) {
            netsnmp_set_request_error(reqinfo, requests,
                                      SNMP_ERR_WRONGTYPE);
            retVal = SNMP_ERR_WRONGTYPE;
        } else {
            retVal = v2Test(requests->requestvb->name,
                            requests->requestvb->name_length,
                            requests->requestvb->val.string,
                            requests->requestvb->val_len,
                            requests->requestvb->type, reqinfo);
            if (retVal != SNMP_ERR_NOERROR)
                netsnmp_set_request_error(reqinfo, requests, retVal);
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        break;

    case MODE_SET_COMMIT:
        /*
         * really do the set! 
         */
        retVal = v2Set(requests->requestvb->name,
                       requests->requestvb->name_length,
                       requests->requestvb->val.string,
                       requests->requestvb->val_len,
                       requests->requestvb->type, reqinfo);
        if (retVal != SNMP_ERR_NOERROR)
            netsnmp_set_request_error(reqinfo, requests, retVal);
        break;

    case MODE_SET_UNDO:
        /*
         * Can't undo V2 sets! 
         */
        break;

    default:
        /*
         * we should never get here, so this is a really bad error 
         */
        return SNMP_ERR_GENERR;
    }

    return retVal;
}