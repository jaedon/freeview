/* Record 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var record_api = {
    category: 'record',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/records',
            url: undefined,
            parameters: ['member', 'startuid', 'enduid', 'cnt']},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/records/[recordUid]',
            url: undefined,
            parameters: []},
        {method: 'POST',
            api: RESTAPI.serverInfo.apiv1path+'/records',
            url: undefined,
            parameters: []},
        {method: 'DELETE',
            api: RESTAPI.serverInfo.apiv1path+'/records/[recordUid]',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.record_api) {
    RESTAPI.v1.record_api = record_api.api_set;
}

var GET_RECORDS_ARGS = record_api.api_set[0].parameters;
var GET_RECORDBYUID_ARGS = record_api.api_set[1].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/records
 *  Description : 모든 Record에 대한 요청
 *  */
exports.getRecords = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_RECORDS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_RECORD_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_RECORD_Get() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};

/**
 *  Method : GET
 *  API : /stb-api/v1/records:recordUid
 *  Description : Record Inform 요청
 *  */
exports.getRecordByUid = function (request, response) {
    var uid = request.param('recordUid');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    var nUid = Number(uid);
    if ((typeof nUid !== 'number') || isNaN(nUid)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_RECORDBYUID_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nUid;
    var arg3 = JSON.stringify(query);
    binder.REST_BIND_RECORD_GetOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_RECORD_GetOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : POST
 *  API : /stb-api/v1/records
 *  Description : Recording 요청
 *  */
exports.postRecord = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_RECORD_Post.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_RECORD_Post() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : DELETE
 *  API : /stb-api/v1/records/:recordUid
 *  Description : Recording Delete 요청
 *  */
exports.deleteRecord = function (request, response) {
    var uid = request.param('recordUid');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    var arg1 = request.sessionID;
    var arg2 = Number(uid);
    if ((typeof arg2 !== 'number') || isNaN(arg2)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }
    binder.REST_BIND_RECORD_DeleteOne.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_RECORD_DeleteOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};