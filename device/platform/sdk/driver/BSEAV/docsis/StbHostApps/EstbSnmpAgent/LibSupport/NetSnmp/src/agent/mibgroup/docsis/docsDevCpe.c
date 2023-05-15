/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.brcm.scalar.conf $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "docsDevCpe.h"

/*
 * Define the OID pointer to the top of the mib tree that we're
 * registering underneath
 */
oid             docsDevCpe_module_oid[] = { 1, 3, 6, 1, 2, 1, 69, 1, 7 };
int             docsDevCpe_module_oid_len =
    sizeof(docsDevCpe_module_oid) / sizeof(oid);
int             docsDevCpe_module_count = 0;


/** Initializes the docsDevCpe module */
void
init_docsDevCpe(void)
{
    static oid      docsDevCpeEnroll_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 7, 1, 0 };
    static oid      docsDevCpeIpMax_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 7, 2, 0 };

    DEBUGMSGTL(("docsDevCpe", "Initializing\n"));

    netsnmp_register_instance(netsnmp_create_handler_registration
                              ("docsDevCpeEnroll",
                               do_docsDevCpeEnroll,
                               docsDevCpeEnroll_oid,
                               OID_LENGTH(docsDevCpeEnroll_oid),
                               HANDLER_CAN_RWRITE));
    netsnmp_register_instance(netsnmp_create_handler_registration
                              ("docsDevCpeIpMax",
                               do_docsDevCpeIpMax,
                               docsDevCpeIpMax_oid,
                               OID_LENGTH(docsDevCpeIpMax_oid),
                               HANDLER_CAN_RWRITE));
}

int
do_docsDevCpeEnroll(netsnmp_mib_handler *handler,
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

    u_char          docsDevCpeEnroll_var[STRINGMAX];
    size_t          docsDevCpeEnroll_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length, docsDevCpeEnroll_var,
                  &docsDevCpeEnroll_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     docsDevCpeEnroll_var,
                                     docsDevCpeEnroll_varLen);
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
do_docsDevCpeIpMax(netsnmp_mib_handler *handler,
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

    u_char          docsDevCpeIpMax_var[STRINGMAX];
    size_t          docsDevCpeIpMax_varLen;

    switch (reqinfo->mode) {

    case MODE_GET:
        retVal =
            v2Get(requests->requestvb->name,
                  requests->requestvb->name_length, docsDevCpeIpMax_var,
                  &docsDevCpeIpMax_varLen, reqinfo);
        if (retVal == SNMP_ERR_NOERROR)
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     docsDevCpeIpMax_var,
                                     docsDevCpeIpMax_varLen);
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