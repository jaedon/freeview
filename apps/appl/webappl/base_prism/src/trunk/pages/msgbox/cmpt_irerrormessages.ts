///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/cmpt_irerrormessages.js
/* jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

module mComponent {
    export class irErrorMessages extends __COMPONENT_KERNEL__ {
        UNKNOWN_ERROR: string = "";

        constructor() {
            super();
        }

        create(aParam: any): any {
            this.UNKNOWN_ERROR ="Unknown Error.";
            return this;
        }

        destroy(): void {
            this.$.remove();
        }

        _cas_ir_get_errorMsgString(aParam: string): string {
            var irErrString: string = this.UNKNOWN_ERROR;
            var irErrCode: any = aParam;

            switch (aParam)
            {
                /* Message Errors */
                case "E100-2":
                    irErrString = hx.l('LOC_CAS_IR_MSG_BAD_OP_CODE_OR_SEQ');
                    break;
                case "F101-2":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                    break;
                case "F102-2":
                    irErrString = hx.l('LOC_CAS_IR_MSG_FORMAT_ERR');
                    break;

                /* General Service Status Messages from Service Status Message */
                case "D100-3":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_OK');
                    break;
                case "E101-3":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                    break;

                /* DVB EMM Service */
                case "D100-5":
                    irErrString = hx.l('LOC_CAS_IR_MSG_EMM_SVC_OK');
                    break;
                case "E101-5":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                    break;

                /* DVB Descramble Service */
                case "D100-8":
                    irErrString = hx.l('LOC_CAS_IR_MSG_DESCRAMBLER_SVC_OK');
                    break;
                case "E101-8":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                    break;

                /* PVR Record Service */
                case "D100-27":
                    irErrString = hx.l('LOC_CAS_IR_MSG_PVR_REC_SVC_OK');
                    break;
                case "E101-27":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                    break;

                /* PVR Playback Service */
                case "D100-28":
                    irErrString = hx.l('LOC_CAS_IR_MSG_PVR_PLAYBACK_SVC_OK');
                    break;
                case "E101-28":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                    break;

                /* PVR DRM Service */
                case "D100-36":
                    irErrString = hx.l('LOC_CAS_IR_MSG_PVR_DRM_SVC_OK');
                    break;
                case "E101-36":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                    break;

                /* Smart Card Resource */
                case "D00-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CARD_IN');
                    break;
                case "E04-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INSERT_CARD');
                    break;
                case "E05-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNKNOWN_CARD');
                    break;
                case "E06-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CARD_FAIL');
                    break;
                case "I07-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CHECK_CARD');
                    break;
                case "E144-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_WRONG_HOME_NETWORK');
                    break;

                /* EMM Source (CAT) Resource */
                case "D100-6":
                    irErrString = hx.l('LOC_CAS_IR_MSG_EMM_SOURCE_OK');
                    break;
                case "D101-6":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_MATCH');
                    break;
                case "E102-6":
                    irErrString = hx.l('LOC_CAS_IR_MSG_OUT_OF_EMM_FILTER');
                    break;
                case "D103-6":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_CAT');
                    break;
                case "D104-6":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_VENDOR_ID');
                    break;
                case "E105-6":
                    irErrString = hx.l('LOC_CAS_IR_MSG_OUT_OF_CHANNELS');
                    break;

                /* ECM Source (PMT) Resource */
                case "D100-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_ECM_SOURCE_OK');
                    break;
                case "D101-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_PMT');
                    break;

                /* CA-Status */
                case "E16-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_PERMISSION_ID');
                    break;
                case "E18-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NOT_AVAILABLE_LOCATION_ID');
                    break;
                case "E19-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SUBSCRIPTION_EXPIRED_ID');
                    break;
                case "E20-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NOT_AVAILABLE_LOCATION_ID');
                    break;
                case "E21-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "D29-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_DESCRAMBLED_ID');
                    break;
                case "E30-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E32-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_TEMP_BLOCK_ID');
                    break;
                case "E33-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CHECK_IF_CORRECT_SMARTCARD_ID');
                    break;
                case "I34-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_VALIDATING_SUBSCRIPTION_ID');
                    break;
                case "I55-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SUBSCRIPTION_UPDATE_ID');
                    break;
                case "E80-35":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E81-35":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E100-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E101-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "D102-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_VENDOR_ID');
                    break;
                case "I102-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "D103-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_I34_9_ID');
                    break;
                case "E104-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "D105-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_FTA_SERVICE_ID');
                    break;
                case "E106-9":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INSERT_CORRECT_SMARTCARD_ID');
                    break;
                case "E107-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NOT_AUTHORIZED_SMARTCARD_ID');
                    break;
                case "E108-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E109-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E116-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E117-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E118-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E110-4": case "E111-4": case "E112-4": case "E113-4": case "E114-4": case "E115-4":
                case "E119-4": case "E121-4": case "E122-4": case "E123-4":
                irErrString = hx.l('LOC_CAS_IR_MSG_RESERVED_SMART_CARD_RETURN_CODE_ID');
                break;
                case "E120-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CARD_SYNCHRONIZED_ID');
                    break;
                case "E124-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E129-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_VIEWING_RIGHTS_EXPIRED_ID');
                    break;
                case "E133-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E136-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_AGE_RESTRICTED_ID');
                    break;
                case "E137-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E138-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_PINCODE_BLOCKED_ID');
                    break;
                case "I139-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_VALIDATING_SUBSCRIPTION_ID');
                    break;
                case "E140-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NOT_AVAILABLE_LOCATION_ID');
                    break;
                case "E141-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_VIEW_THIS_CHANNEL_ID');
                    break;
                case "E142-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INSERT_SECONDARYCARD_INTO_PRIMARY_DECODER_ID');
                    break;
                case "E143-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INSERT_SECONDARYCARD_INTO_PRIMARY_DECODER_01_ID');
                    break;
                case "E144-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_WRONG_HOME_NETWORK');
                    break;
                case "E145-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_PERMISSION_ON_SECONDARY_TV_ID');
                    break;
                case "E151-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_TIMESTAMP_NOT_SUPPLIED_ID');
                    break;
                case "E152-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_PREVIEW_TIME_OUT_ID');
                    break;
                case "D200-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_AGE_RESTRICTED_ID');
                    break;
                case "E201-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INSERT_CORRECT_SMARTCARD_ID');
                    break;

                /* DVB Descrambler Resource */
                case "D100-10":
                    irErrString = hx.l('LOC_CAS_IR_MSG_DESCRAMBLER_OK_ID');
                    break;
                case "E101-10":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CANT_ALLOC_DESC_ID');
                    break;

                /* Monitoring Resource */
                case "D100-11":
                    irErrString = hx.l('LOC_CAS_IR_MSG_MONITOR_ENABLE_ID');
                    break;
                case "E101-11":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NOT_SUP_FOR_SVC_ID');
                    break;

                /* IPPV Module Status */
                /*	case "D22-12":
                 irErrString = hx.l('LOC_CAS_IR_MSG_EVENT_PURCHASE_ID');
                 break;
                 case "D23-12":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PREVIEW_PERIOD_ID');
                 break;
                 case "E24-12":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PREV_PERIOD_EXP_ID');
                 break;
                 case "D25-12":
                 irErrString = hx.l('LOC_CAS_IR_MSG_EVT_ALREADY_PURCH_ID');
                 break;
                 case "D26-12":
                 irErrString = hx.l('LOC_CAS_IR_MSG_EVT_RECORDED_ID');
                 break;
                 case "D27-12":
                 irErrString = hx.l('LOC_CAS_IR_MSG_NO_PURCHASES_ID');
                 break;
                 case "D28-12":
                 irErrString = hx.l('LOC_CAS_IR_MSG_CREDIT_LIMIT_ID');
                 break;
                 */

                /* SC Marriage Module Status */
                case "E17-13":
                    irErrString = hx.l('LOC_CAS_IR_MSG_I34_9_ID');
                    break;

                /* Session Management Resource */
                case "D100-29":
                    irErrString = hx.l('LOC_CAS_IR_MSG_OK_ID');
                    break;
                case "E101-29":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_MSK_ID');
                    break;
                case "E102-29":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_SUBSCRIPTION_ID');
                    break;
                case "E103-29":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SESSION_STOR_FAIL_ID');
                    break;
                case "E104-29":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SESSION_DATA_ERR_ID');
                    break;
                case "D105-29":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SESSION_INITIALIZING_ID');
                    break;

                /* Session Management DRM Resource */
                /*	case "E110-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_UNKNOWN_IR_PVR_PHASE_ID');
                 break;
                 case "E111-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_REC_KEY_ERROR_ID');
                 break;
                 case "E112-37":
                 irErrString = hx.l('LOC_CAS_IR_UC_MSG_E130_PVR_NOT_ALLOWED');
                 break;
                 case "I113-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_ACQUIRING_INIT_CONTENT_RIGHTS_ID');
                 break;
                 case "I114-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_ACQUIRING_INIT_CONTENT_RIGHTS_ID');
                 break;
                 case "E115-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_CHAIN_RECORDINGS_FAILURE_ID');
                 break;
                 case "D116-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PEEKING_CONTENT_RIGHTS_ID');
                 break;
                 case "E117-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E132_4_ID');
                 break;
                 case "E118-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PERMANENT_RECORDING_NOT_ALLOWED_ID');
                 break;
                 case "E153-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_SKR_VER_NOT_MATCH_ID');
                 break;
                 case "D116-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PEEKING_CONTENT_RIGHTS_ID');
                 break;
                 case "E117-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E132_4_ID');
                 break;
                 case "E118-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PERMANENT_RECORDING_NOT_ALLOWED_ID');
                 break;
                 case "E153-37":
                 irErrString = hx.l('LOC_CAS_IR_MSG_SKR_VER_NOT_MATCH_ID');
                 break;
                 case "E16-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E127_4_ID');
                 break;
                 case "E30-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_DRM_AUTHENTICATION_FAIL_ID');
                 break;
                 case "E33-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_INVALID_SMARTCARD_ID');
                 break;
                 case "D126-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PVR_DRM_OK_ID');
                 break;
                 case "E127-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E127_4_ID');
                 break;
                 case "E128-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E128_4_ID');
                 break;
                 case "E129-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_OUTSIDE_VIEWING_WINDOW_ID');
                 break;
                 case "E130-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E134_4_ID');
                 break;
                 case "E131-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E131_4_ID');
                 break;
                 case "E132-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_E132_4_ID');
                 break;
                 case "E133-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_UNABLE_TO_ACQUIRE_CONTENT_RIGHTS_ID');
                 break;
                 case "E134-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_RECORD_IS_NOT_ALLOWED_ID);
                 break;
                 case "E135-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_RECORD_IS_NOT_ALLOWED_ID');
                 break;
                 case "E136-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_MATURITY_RATING_BLOCKED_ID');
                 break;
                 case "E137-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_MATURITY_RATING_PIN_ERROR_ID');
                 break;
                 case "E138-4":
                 irErrString = hx.l('LOC_CAS_IR_MSG_MATURITY_RATING_PIN_BLOCKED_ID');
                 break;
                 */

                /* Crypto Operation Resource*/
                case "D100-30":
                    irErrString = hx.l('LOC_CAS_IR_MSG_OK_ID');
                    break;
                case "E101-30":
                    irErrString = hx.l('LOC_CAS_IR_MSG_ENCRYPT_NOT_AVAIL_ID');
                    break;
                case "D102-30":
                    irErrString = hx.l('LOC_CAS_IR_MSG_KEY_NOT_LOADED_ID');
                    break;

                /* Copy Control Resource */
                case "D100-31":
                    irErrString = hx.l('LOC_CAS_IR_MSG_OK_ID');
                    break;
                case "E101-31":
                    irErrString = hx.l('LOC_CAS_IR_MSG_RESOURCE_UNAVAIL_ID');
                    break;

                /* LPPV Module Status */
                /*	case "D82-42":
                 irErrString = hx.l('LOC_CAS_IR_MSG_EVENT_PURCHASE_ID');
                 break;
                 case "D83-48":
                 irErrString = hx.l('LOC_CAS_IR_MSG_PREVIEW_PERIOD_ID');
                 break;
                 case "D85-48":
                 irErrString = hx.l('LOC_CAS_IR_MSG_EVT_ALREADY_PURCH_ID');
                 break;
                 case "E87-48":
                 irErrString = hx.l('LOC_CAS_IR_MSG_TICKET_RECORD_STORE_FULL_ID');
                 break;
                 case "E88-48":
                 irErrString = hx.l('LOC_CAS_IR_MSG_NO_ORDER_ID');
                 break;
                 case "E89-48":
                 irErrString = hx.l('LOC_CAS_IR_MSG_NO_BUY_WINDOW_ID');
                 break;
                 case "E91-48":
                 irErrString = hx.l('LOC_CAS_IR_MSG_CREDIT_LIMIT_ID');
                 break;
                 case "E92-48":
                 irErrString = hx.l('LOC_CAS_IR_MSG_TOO_EXPENSIVE_ID');
                 break;
                 case "E93-49":
                 irErrString = hx.l('LOC_CAS_IR_MSG_WRONG_PPV_MODE_ID');
                 break;
                 */

                /* General SI Messages */
                case "E37-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_UNKNOWN');
                    break;
                case "E38-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_NOT_RUNNING');
                    break;
                case "E39-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_LOCATING_SVC');
                    break;
                case "E40-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_DECODER_MEM_FULL');
                    break;
                case "E41-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_NOT_AVAILABLE');
                    break;
                case "E42-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_PARENTAL_LOCK');
                    break;
                case "E43-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NOT_ALLOWED_COUNTRY');
                    break;
                case "E44-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NOT_EIT');
                    break;
                case "E45-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_NOT_ALLOWED');
                    break;
                case "E46-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_READ_SI');
                    break;
                case "E47-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_BOUQUET_BLOCK');
                    break;

                /* IRD Tuning Error Messages */
                case "E48-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_SIGNAL');
                    break;
                case "E49-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_LNB_OVERLOAD');
                    break;
                case "E50-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NO_SVC_AVAILABLE');
                    break;
                case "E52-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SEARCHING_SIGNAL');
                    break;
                case "E64-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INVALID_TUNE');
                    break;

                /* Miscellaneous Error Messages */
                case "E00-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_NOT_SCRAMBLED');
                    break;
                case "E53-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INCORRECT_PIN');
                    break;
                case "E54-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_IPPV_OK');
                    break;
                case "E56-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CAM_NOT_COMPATIBLE');
                    break;
                case "E57-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_UNKNOWN_CHANNEL');
                    break;
                case "E58-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CHANNEL_NOT_AVAILABLE');
                    break;
                case "E66-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_BLOCKED');
                    break;
                case "E67-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INSERT_NAGRAVISION_SMARTCARD_ID');
                    break;
                case "E68-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NAGRAVISION_SMARTCARD_NOT_CORRECTLY_ID');
                    break;
                case "E69-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_FTA_SVC_IS_BLOCKED_ID');
                    break;

                /* Optional Error Messages */
                case "E01-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_INSERT_CAM');
                    break;
                case "E02-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CAM_EEPROM_FAIL');
                    break;
                case "E03-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CA_MODULE_FAIL');
                    break;
                case "E14-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_DESCRAMBLED_ID');
                    break;
                case "E35-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_SVC_DESCRAMBLED_ID');
                    break;
                case "E36-32":
                    irErrString = hx.l('LOC_CAS_IR_MSG_NAGRAVISION_SMARTCARD_NOT_CORRECTLY_ID');
                    break;

                /* Error Messages supported on Multiview STBs */
                /*	case "E70-32":
                 irErrString = hx.l('LOC_CAS_IR_MSG_SERVICE_UNAVAILABLE_SECOND_TV_ID');
                 break;
                 case "E71-32":
                 irErrString = hx.l('LOC_CAS_IR_MSG_SONG_INFORMATION_CANNOT_BE_VIEWED_ID');
                 break;
                 case "E72-32":
                 irErrString = hx.l('LOC_CAS_IR_MSG_RESCAN_PROGRESS_ID');
                 break;
                 case "E73-32":
                 irErrString = hx.l('LOC_CAS_IR_MSG_TVLINK_OVERLOAD_ID');
                 break;
                 case "E74-32":
                 irErrString = hx.l('LOC_CAS_IR_MSG_LNB1_OVERLOAD_ID');
                 break;
                 case "E75-32":
                 irErrString = hx.l('LOC_CAS_IR_MSG_LNB2_OVERLOAD_ID');
                 break;
                 case "E76-32":
                 irErrString = hx.l('LOC_CAS_IR_MSG_TV2_DISABLED_ID');
                 break;
                 */

                /* New Irdeto Error Messages from New Specification */
                case "E154-4":
                    irErrString = hx.l('LOC_CAS_IR_MSG_CWSK_KEY_VER_MISMATCH_ID');
                    break;

                default :
                    irErrString = this.UNKNOWN_ERROR;
                    break;
            }

            return irErrString;
        }

        _cas_ir_make_regular_error_msg(irErrCode: string): string {
            var irErrMsg: string = this.UNKNOWN_ERROR;
            var irErrString: string = this._cas_ir_get_errorMsgString(irErrCode);
            var LangObj: any = hx.svc.SETTING_UTIL.getLanguage();
            var menuLang: string = LangObj.LangMenu;

            if (irErrString == this.UNKNOWN_ERROR) {
                irErrMsg = this.UNKNOWN_ERROR;
            } else {
                if ( menuLang == "ara" || menuLang == "per") { // if menu language is Persian or Arabic,
                    irErrMsg = irErrString + " " + irErrCode ;
                } else {
                    irErrMsg = irErrCode + " " + irErrString;
                }
            }

            return irErrMsg;
        }

        _cas_ir_make_irregular_error_msg(paramMsg: string): string {
            var irErrMsg = this.UNKNOWN_ERROR;
            var	strArray = paramMsg.split(" ");
            var irErrPrefix = strArray[0];

            switch (irErrPrefix)
            {
                case "VIEW_WITH_PREVIEW":
                    var previewMinues: string = strArray[1] || "0"; // return strArray[1] if strArray[1] is not null or undefined, otherwise return "0";
                    irErrMsg = hx.l('LOC_CAS_IR_MSG_PREVIEW_TIME_PERIOD_ID') + ": " + previewMinues + hx.l('LOC_MINS_ID') ; // if it is Middle East Language, the strings automatically change.
                    break;
                default :
                    irErrMsg = this.UNKNOWN_ERROR;
                    break;
            }

            return irErrMsg;
        }
    }
}
export = mComponent;