/* Tuner 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
// TODO: 구현 및 Binding 완료 되면 아래 주석을 풀어서 global 객체에 삽입할 것
var tuner_api = {
    category: 'tuner',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/tuners',
            url: undefined,
            parameters: ['startidx', 'cnt', 'member']},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/tuners/[tunerUid]',
            url: undefined,
            parameters: ['member']}
    ]
};
if (!RESTAPI.v1.tuner_api) {
    RESTAPI.v1.tuner_api = tuner_api.api_set;
}

var GET_TUNERS_ARGS = tuner_api.api_set[0].parameters;
var GET_TUNERBYIDX_ARGS = tuner_api.api_set[1].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/tuners
 *  Description : 모든 Tuner에 대한 요청
 *  */
exports.getTuners = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_TUNERS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_TUNER_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_TUNER_Get() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : GET
 *  API : /stb-api/v1/tuners/:tunerUid
 *  Description : Tuner Inform 요청
 *  */
exports.getTunerByUid = function (request, response) {
    var uid = request.param('tunerUid');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_TUNERBYIDX_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    /* playerID 값은 Number type 이어야 한다. */
    var nTunerId = Number(uid);
    if ((typeof nTunerId !== 'number') || isNaN(nTunerId)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nTunerId;
    var arg3 = JSON.stringify(query);
    binder.REST_BIND_TUNER_GetOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_TUNER_GetOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};