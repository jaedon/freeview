/**
 * Created by ssyun on 2014-04-10.
 */
'use strict';

// Define Error Code/String
var restError = {
    ///////////////////////////////////////////////////////////////////////////////////////
    // 공통 에러( 1 ~ 99 )
    ///////////////////////////////////////////////////////////////////////////////////////
    // 일반 에러
    ERR_INTERNAL: {error: {errCode: 1, errString: 'Internal Error'}, status: 500},
    ERR_NOMEM: {error: {errCode: 2, errString: 'No Memory'}, status: 500},
    ERR_NOTREADY: {error: {errCode: 3, errString: 'Not Ready'}, status: 500},
    ERR_NODATA: {error: {errCode: 4, errString: 'No Data'}, status: 204},

    // URL 관련 에러
    ERR_UNSUPPORTED_API: {error: {errCode: 11, errString: 'Unsupported API'}, status: 404},
    ERR_PAGE_NOTFOUND: {error: {errCode: 12, errString: 'Page Not Found'}, status: 404},
    ERR_RESOURCE_NOTFOUND: {error: {errCode: 13, errString: 'Resource Not Found'}, status: 404},

    // Parameter 관련 에러
    ERR_INVALID_KEY: {error: {errCode: 21, errString: 'Invalid Key'}, status: 405},
    ERR_INVALID_VALUE: {error: {errCode: 22, errString: 'Invalid Value'}, status: 405},
    ERR_REQUIRE_MANDATORYKEY: {error: {errCode: 23, errString: 'Require Mandatory Key'}, status: 405},
    ERR_CMD_FAILED: {error: {errCode: 24, errString: 'Command Failed'}, status: 405},
    ERR_UNKNOWN_CMD: {error: {errCode: 25, errString: 'Unknown Command'}, status: 405},
    ERR_WRONG_SOURCE: {error: {errCode: 26, errString: 'Wrong Source'}, status: 405},
    ERR_WRONG_SOURCETYPE: {error: {errCode: 27, errString: 'Wrong Source Type'}, status: 405},

    // Permission 관련 에러
    ERR_REQUIRE_PIN: {error: {errCode: 91, errString: 'Require PIN Code'}, status: 401},
    ERR_INVALID_PIN: {error: {errCode: 92, errString: 'Invalid PIN Code'}, status: 401},
    ERR_INVALID_SESSION: {error: {errCode: 93, errString: 'Invalid Session'}, status: 401},

    // 외부접속 제한
    ERR_ACCESS_DENY: {error: {errCode: 99, errString: 'Remote-Access Was Disabled'}, status: 403},


    ///////////////////////////////////////////////////////////////////////////////////////
    // 모듈 관련 에러( 1000 ~ )
    ///////////////////////////////////////////////////////////////////////////////////////
    // 모듈 공통( 1000 ~ 1099 )

    // Channel 관련 에러( 1101 ~ 1199 )
    ERR_NO_TUNABLE_INFO: {error: {errCode: 1101, errString: 'Tunable infomation not available'}, status: 400},

    // Player 관련 에러( 1201 ~ 1299 )

    // Scan 관련 에러( 1301 ~ 1399 )

    // EPG 관련 에러( 1401 ~ 1499 )

    // Schedule 관련 에러( 1501 ~ 1599 )

    // Record 관련 에러( 1601 ~ 1699 )

    // Settings 관련 에러( 1701 ~ 1799 )

    // ...


    ///////////////////////////////////////////////////////////////////////////////////////
    // 성공( 0 )
    ///////////////////////////////////////////////////////////////////////////////////////
    ERR_OK: {error: {errCode: 0, errString: 'No Error'}, status: 200}
};
// TODO: 각 errString을 UTF8로 encoding 할 경우 아래 주석을 해제 함
//for (var item in restError) {
//    restError[item].error.errString = encodeURIComponent(restError[item].error.errString);
//}
exports.restError = restError;
global.RESTAPI.restError= restError;


/**
 * Client의 요청에 Error 응답 함
 * @param response - client로 응답하기 위한 response 객체
 * @param errName - restError의 에러객체 이름
 */
var sendError = function(response, errName) {
    var errObj = restError[errName];
    var retObj = {};
    retObj.error = errObj.error;

    response.status(errObj.status).send(retObj);
};
exports.sendError = sendError;
global.RESTAPI.sendError= sendError;


/**
 * Client의 요청에 정상 응답 함
 * Binder로 부터 전달된 JSON String을 object화 하여 전달.
 * @param response - client로 응답하기 위한 response 객체
 * @param result - Binder로 부터 리턴된 요청에 대한 result
 */
var sendResult = function(response, result) {
    var retValue;
    var retStatus = 200;
    try {
        retValue = JSON.parse(result);
    } catch (e) {
        RESTAPI.elog('########C Binder Error##########');
        RESTAPI.elog(e);
        RESTAPI.elog('################################');
        retValue = restError.ERR_INTERNAL;
    }

    // 결과의 errCode를 보고 http status를 구분
    for (var errObj in restError) {
        if (restError.hasOwnProperty(errObj)) {
            if (restError[errObj].error.errCode === retValue.error.errCode) {
                retStatus = restError[errObj].status;
                break;
            }
        }
    }


    response.status(retStatus).send(retValue);
};

exports.sendResult = sendResult;
global.RESTAPI.sendResult= sendResult;
