/* Channer-Scanner 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var channel_scanner_api = {
    category: 'channel-scanner',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/channel-scanners',
            url: undefined,
            parameters: ['channelList', 'channelType', 'channelStartIdx', 'channelCount']},
        {method: 'PUT',
            api: RESTAPI.serverInfo.apiv1path+'/channel-scanners',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.channel_scanner_api) {
    RESTAPI.v1.channel_scanner_api = channel_scanner_api.api_set;
}

var GET_SCANNERS_ARGS = channel_scanner_api.api_set[0].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/channel-scanners
 *  Description : 모든 Scanner에 대한 요청
 *  */
exports.getScanners = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_SCANNERS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_SCAN_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_SCAN_Get() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : PUT
 *  API : /stb-api/v1/channel-scanners
 *  Description : Scanner 설정 및 동작
 *  */
exports.putScanner = function (request, response) {
    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(request.body);
    binder.REST_BIND_SCAN_Put.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('C Function RETURN :: REST_BIND_SCAN_Put()');
        RESTAPI.log('RETURN( %s )', res);
        RESTAPI.sendResult(response, res);
    });
};