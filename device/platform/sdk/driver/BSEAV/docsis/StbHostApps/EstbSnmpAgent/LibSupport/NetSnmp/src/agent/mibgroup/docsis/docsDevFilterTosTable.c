/*
 * Note: this file originally auto-generated by mib2c using
 *        mib2c.brcm.iterate.conf 2003/6/16 turcotte 
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "docsDevFilterTosTable.h"

/** Initialize the docsDevFilterTosTable table by defining its contents and how it's structured */
void
initialize_table_docsDevFilterTosTable(void)
{
    static oid      docsDevFilterTosTable_oid[] =
        { 1, 3, 6, 1, 2, 1, 69, 1, 6, 6 };
    netsnmp_handler_registration *my_handler;

    /*
     * create the table structure itself 
     */

    /*
     * if your table is read only, it's easiest to change the
     * HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY 
     */
    my_handler =
        netsnmp_create_handler_registration("docsDevFilterTosTable",
                                            docsDevFilterTosTable_handler,
                                            docsDevFilterTosTable_oid,
                                            OID_LENGTH
                                            (docsDevFilterTosTable_oid),
                                            HANDLER_CAN_RWRITE);

    if (my_handler) {
        if (netsnmp_register_handler(my_handler)) {
            DEBUGMSGTL(("initialize_table_docsDevFilterTosTable",
                        "Handler registration failed!\n"));
            return;
        }
    } else {
        DEBUGMSGTL(("initialize_table_docsDevFilterTosTable",
                    "Handler create failed!\n"));
        return;
    }
    DEBUGMSGTL(("initialize_table_docsDevFilterTosTable",
                "Handler created and registered.\n"));



}

/** Initializes the docsDevFilterTosTable module */
void
init_docsDevFilterTosTable(void)
{

    /*
     * here we initialize all the tables we're planning on supporting 
     */
    initialize_table_docsDevFilterTosTable();
}

/** handles requests for the docsDevFilterTosTable table */
int
docsDevFilterTosTable_handler(netsnmp_mib_handler *handler,
                              netsnmp_handler_registration *reginfo,
                              netsnmp_agent_request_info *reqinfo,
                              netsnmp_request_info *requests)
{

    netsnmp_request_info *request;
    netsnmp_variable_list *var;
    char           *tmpVal[MAX_OID_LEN * sizeof(oid)];
    size_t          tmpValLen;
    int             tmpAsnType, tmpOidLen;
    oid             tmpOid[MAX_OID_LEN];
    int             retVal = SNMP_ERR_NOERROR;

    for (request = requests; request; request = request->next) {
        var = request->requestvb;
        if (request->processed != 0)
            continue;

        /*
         * perform anything here that you need to do before each
         * request is processed. 
         */

        switch (reqinfo->mode) {
        case MODE_GET:
            retVal = v2Get(var->name, var->name_length, tmpVal,
                           &tmpValLen, reqinfo);
            if (retVal == SNMP_ERR_NOERROR) {
                switch (var->name[reginfo->rootoid_len + 1]) {
                case COLUMN_DOCSDEVFILTERTOSSTATUS:
                    snmp_set_var_typed_value(var, ASN_INTEGER,
                                             (u_char *) tmpVal, tmpValLen);
                    break;

                case COLUMN_DOCSDEVFILTERTOSANDMASK:
                    snmp_set_var_typed_value(var, ASN_OCTET_STR,
                                             (u_char *) tmpVal, tmpValLen);
                    break;

                case COLUMN_DOCSDEVFILTERTOSORMASK:
                    snmp_set_var_typed_value(var, ASN_OCTET_STR,
                                             (u_char *) tmpVal, tmpValLen);
                    break;

                default:
                    /*
                     * We shouldn't get here 
                     */
                    snmp_log(LOG_ERR,
                             "problem encountered in docsDevFilterTosTable_handler: unknown column\n");
                }
            } else {
                netsnmp_set_request_error(reqinfo, requests, retVal);
            }
            break;
        case MODE_GETNEXT:
            DEBUGMSGTL(("docsDevFilterTosTable_handler", "GETNEXT\n"));
            /*
             * Check to see if var is in this subtree. If not, just
             * get the first object by using rootoid.1.firstColumn 
             */
            if ((netsnmp_oid_is_subtree
                 (reginfo->rootoid, reginfo->rootoid_len, var->name,
                  var->name_length) == 0)
                && (var->name_length >= reginfo->rootoid_len + 2)) {
                DEBUGMSGTL(("docsDevFilterTosTable_handler",
                            "In Subtree - using var:\n"));
                tmpOidLen = var->name_length;
                memcpy((char *) tmpOid, (char *) var->name,
                       tmpOidLen * sizeof(oid));
                retVal =
                    v2GetNext(tmpOid, &tmpOidLen, reginfo->rootoid_len + 2,
                              tmpVal, &tmpValLen, reqinfo);
                if ((retVal == SNMP_ERR_NOSUCHNAME)
                    && (tmpOid[reginfo->rootoid_len + 1] < 4)) {
                    /*
                     * more columns to try 
                     */
                    tmpOidLen = reginfo->rootoid_len + 2;
                    tmpOid[tmpOidLen - 1] = tmpOid[tmpOidLen - 1] + 1;
                    retVal = v2GetNext(tmpOid, &tmpOidLen, tmpOidLen,
                                       tmpVal, &tmpValLen, reqinfo);
                }
            } else {            /* not in subtree */
                tmpOidLen = reginfo->rootoid_len;
                memcpy((char *) tmpOid, (char *) reginfo->rootoid,
                       tmpOidLen * sizeof(oid));
                tmpOid[tmpOidLen] = 1;
                tmpOidLen++;
                tmpOid[tmpOidLen] = 2;
                tmpOidLen++;
                retVal = v2GetNext(tmpOid, &tmpOidLen, tmpOidLen,
                                   tmpVal, &tmpValLen, reqinfo);
            }

            if (retVal == SNMP_ERR_NOERROR) {
                snmp_set_var_objid(var, tmpOid, tmpOidLen);
                switch (var->name[reginfo->rootoid_len + 1]) {
                case COLUMN_DOCSDEVFILTERTOSSTATUS:
                    snmp_set_var_typed_value(var, ASN_INTEGER,
                                             (u_char *) tmpVal, tmpValLen);
                    break;

                case COLUMN_DOCSDEVFILTERTOSANDMASK:
                    snmp_set_var_typed_value(var, ASN_OCTET_STR,
                                             (u_char *) tmpVal, tmpValLen);
                    break;

                case COLUMN_DOCSDEVFILTERTOSORMASK:
                    snmp_set_var_typed_value(var, ASN_OCTET_STR,
                                             (u_char *) tmpVal, tmpValLen);
                    break;


                default:
                    /*
                     * We shouldn't get here
                     */
                    snmp_log(LOG_ERR,
                             "problem encountered in docsDevFilterTosTable_handler: unknown column\n");

                }               /* switch on column */
            } else if (retVal == SNMP_ERR_NOSUCHNAME) {
                /*
                 * if V2 returns NOSUCH, netsnmp wants this handler
                 * to return NOERR, but we don't netsnmp_set_request_error
                 * or snmp_set_var_*. Tricky, tricky! 
                 */
                retVal = SNMP_ERR_NOERROR;
            } else {            /* retVal has a real error code */
                netsnmp_set_request_error(reqinfo, requests, retVal);
            }
            break;

        case MODE_SET_RESERVE1:
            printf
                ("docsDevFilterTosTable_handler: Mode = MODE_SET_RESERVE\n");
            switch (var->name[reginfo->rootoid_len + 1]) {
            case COLUMN_DOCSDEVFILTERTOSSTATUS:
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
                        netsnmp_set_request_error(reqinfo, requests,
                                                  retVal);
                }
                break;
            case COLUMN_DOCSDEVFILTERTOSANDMASK:
                if (requests->requestvb->type != ASN_OCTET_STR) {
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
                        netsnmp_set_request_error(reqinfo, requests,
                                                  retVal);
                }
                break;
            case COLUMN_DOCSDEVFILTERTOSORMASK:
                if (requests->requestvb->type != ASN_OCTET_STR) {
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
                        netsnmp_set_request_error(reqinfo, requests,
                                                  retVal);
                }
                break;
            default:
                /*
                 * We shouldn't get here 
                 */
                snmp_log(LOG_ERR,
                         "problem encountered in docsDevFilterTosTable_handler: unknown column\n");
            }
            break;

        case MODE_SET_RESERVE2:
        case MODE_SET_ACTION:
        case MODE_SET_UNDO:
        case MODE_SET_FREE:
            break;
        case MODE_SET_COMMIT:
            printf
                ("docsDevFilterTosTable_handler: Mode = MODE_SET_COMMIT\n");
            /*
             * really do the set! 
             */
            switch (var->name[reginfo->rootoid_len + 1]) {
            case COLUMN_DOCSDEVFILTERTOSSTATUS:

                retVal = v2Set(requests->requestvb->name,
                               requests->requestvb->name_length,
                               requests->requestvb->val.string,
                               requests->requestvb->val_len,
                               requests->requestvb->type, reqinfo);
                if (retVal != SNMP_ERR_NOERROR)
                    netsnmp_set_request_error(reqinfo, requests, retVal);
                break;
            case COLUMN_DOCSDEVFILTERTOSANDMASK:

                retVal = v2Set(requests->requestvb->name,
                               requests->requestvb->name_length,
                               requests->requestvb->val.string,
                               requests->requestvb->val_len,
                               requests->requestvb->type, reqinfo);
                if (retVal != SNMP_ERR_NOERROR)
                    netsnmp_set_request_error(reqinfo, requests, retVal);
                break;
            case COLUMN_DOCSDEVFILTERTOSORMASK:

                retVal = v2Set(requests->requestvb->name,
                               requests->requestvb->name_length,
                               requests->requestvb->val.string,
                               requests->requestvb->val_len,
                               requests->requestvb->type, reqinfo);
                if (retVal != SNMP_ERR_NOERROR)
                    netsnmp_set_request_error(reqinfo, requests, retVal);
                break;
            default:
                /*
                 * We shouldn't get here 
                 */
                snmp_log(LOG_ERR,
                         "problem encountered in docsDevFilterTosTable_handler: unknown column\n");
            }

            break;


        default:
            snmp_log(LOG_ERR,
                     "problem encountered in _handler: unsupported mode\n");
        }
    }
    return retVal;
}