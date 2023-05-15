///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/cmpt_irerrormessages.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var mComponent;
    (function (mComponent) {
        var irErrorMessages = (function (_super) {
            __extends(irErrorMessages, _super);
            function irErrorMessages() {
                _super.call(this);
                this.UNKNOWN_ERROR = "";
            }
            irErrorMessages.prototype.create = function (aParam) {
                this.UNKNOWN_ERROR = "Unknown Error.";
                return this;
            };
            irErrorMessages.prototype.destroy = function () {
                this.$.remove();
            };
            irErrorMessages.prototype._cas_ir_get_errorMsgString = function (aParam) {
                var irErrString = this.UNKNOWN_ERROR;
                var irErrCode = aParam;
                switch (aParam) {
                    case "E100-2":
                        irErrString = hx.l('LOC_CAS_IR_MSG_BAD_OP_CODE_OR_SEQ');
                        break;
                    case "F101-2":
                        irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                        break;
                    case "F102-2":
                        irErrString = hx.l('LOC_CAS_IR_MSG_FORMAT_ERR');
                        break;
                    case "D100-3":
                        irErrString = hx.l('LOC_CAS_IR_MSG_SVC_OK');
                        break;
                    case "E101-3":
                        irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                        break;
                    case "D100-5":
                        irErrString = hx.l('LOC_CAS_IR_MSG_EMM_SVC_OK');
                        break;
                    case "E101-5":
                        irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                        break;
                    case "D100-8":
                        irErrString = hx.l('LOC_CAS_IR_MSG_DESCRAMBLER_SVC_OK');
                        break;
                    case "E101-8":
                        irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                        break;
                    case "D100-27":
                        irErrString = hx.l('LOC_CAS_IR_MSG_PVR_REC_SVC_OK');
                        break;
                    case "E101-27":
                        irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                        break;
                    case "D100-28":
                        irErrString = hx.l('LOC_CAS_IR_MSG_PVR_PLAYBACK_SVC_OK');
                        break;
                    case "E101-28":
                        irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                        break;
                    case "D100-36":
                        irErrString = hx.l('LOC_CAS_IR_MSG_PVR_DRM_SVC_OK');
                        break;
                    case "E101-36":
                        irErrString = hx.l('LOC_CAS_IR_MSG_INCOMPLETE_DEF');
                        break;
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
                    case "D100-9":
                        irErrString = hx.l('LOC_CAS_IR_MSG_ECM_SOURCE_OK');
                        break;
                    case "D101-9":
                        irErrString = hx.l('LOC_CAS_IR_MSG_NO_PMT');
                        break;
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
                    case "E110-4":
                    case "E111-4":
                    case "E112-4":
                    case "E113-4":
                    case "E114-4":
                    case "E115-4":
                    case "E119-4":
                    case "E121-4":
                    case "E122-4":
                    case "E123-4":
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
                    case "D100-10":
                        irErrString = hx.l('LOC_CAS_IR_MSG_DESCRAMBLER_OK_ID');
                        break;
                    case "E101-10":
                        irErrString = hx.l('LOC_CAS_IR_MSG_CANT_ALLOC_DESC_ID');
                        break;
                    case "D100-11":
                        irErrString = hx.l('LOC_CAS_IR_MSG_MONITOR_ENABLE_ID');
                        break;
                    case "E101-11":
                        irErrString = hx.l('LOC_CAS_IR_MSG_NOT_SUP_FOR_SVC_ID');
                        break;
                    case "E17-13":
                        irErrString = hx.l('LOC_CAS_IR_MSG_I34_9_ID');
                        break;
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
                    case "D100-30":
                        irErrString = hx.l('LOC_CAS_IR_MSG_OK_ID');
                        break;
                    case "E101-30":
                        irErrString = hx.l('LOC_CAS_IR_MSG_ENCRYPT_NOT_AVAIL_ID');
                        break;
                    case "D102-30":
                        irErrString = hx.l('LOC_CAS_IR_MSG_KEY_NOT_LOADED_ID');
                        break;
                    case "D100-31":
                        irErrString = hx.l('LOC_CAS_IR_MSG_OK_ID');
                        break;
                    case "E101-31":
                        irErrString = hx.l('LOC_CAS_IR_MSG_RESOURCE_UNAVAIL_ID');
                        break;
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
                    case "E154-4":
                        irErrString = hx.l('LOC_CAS_IR_MSG_CWSK_KEY_VER_MISMATCH_ID');
                        break;
                    default:
                        irErrString = this.UNKNOWN_ERROR;
                        break;
                }
                return irErrString;
            };
            irErrorMessages.prototype._cas_ir_make_regular_error_msg = function (irErrCode) {
                var irErrMsg = this.UNKNOWN_ERROR;
                var irErrString = this._cas_ir_get_errorMsgString(irErrCode);
                var LangObj = hx.svc.SETTING_UTIL.getLanguage();
                var menuLang = LangObj.LangMenu;
                if (irErrString == this.UNKNOWN_ERROR) {
                    irErrMsg = this.UNKNOWN_ERROR;
                }
                else {
                    if (menuLang == "ara" || menuLang == "per") {
                        irErrMsg = irErrString + " " + irErrCode;
                    }
                    else {
                        irErrMsg = irErrCode + " " + irErrString;
                    }
                }
                return irErrMsg;
            };
            irErrorMessages.prototype._cas_ir_make_irregular_error_msg = function (paramMsg) {
                var irErrMsg = this.UNKNOWN_ERROR;
                var strArray = paramMsg.split(" ");
                var irErrPrefix = strArray[0];
                switch (irErrPrefix) {
                    case "VIEW_WITH_PREVIEW":
                        var previewMinues = strArray[1] || "0"; // return strArray[1] if strArray[1] is not null or undefined, otherwise return "0";
                        irErrMsg = hx.l('LOC_CAS_IR_MSG_PREVIEW_TIME_PERIOD_ID') + ": " + previewMinues + hx.l('LOC_MINS_ID'); // if it is Middle East Language, the strings automatically change.
                        break;
                    default:
                        irErrMsg = this.UNKNOWN_ERROR;
                        break;
                }
                return irErrMsg;
            };
            return irErrorMessages;
        })(__COMPONENT_KERNEL__);
        mComponent.irErrorMessages = irErrorMessages;
    })(mComponent || (mComponent = {}));
    return mComponent;
});
