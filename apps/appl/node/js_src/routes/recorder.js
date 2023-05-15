/**
 * Created by ssyun on 2014-08-21.
 */
/* Recorder 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var recorder_api = {
    category: 'recorder',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/recorders',
            url: undefined,
            parameters: ['member', 'startidx', 'endidx', 'cnt']},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/recorders/[recorderId]',
            url: undefined,
            parameters: ['member']},
        {method: 'PUT',
            api: RESTAPI.serverInfo.apiv1path+'/recorders/[recorderId]',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.recorder_api) {
    RESTAPI.v1.recorder_api = recorder_api.api_set;
}

var GET_RECORDERS_ARGS = recorder_api.api_set[0].parameters;
var GET_RECORDERBYID_ARGS = recorder_api.api_set[1].parameters;
//var PUT_RECORDERBYID_ARGS = recorder_api.api_set[2].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/recorders
 *  Description : STB내에 존재하는 Recorder에 대한 정보 요청
 *  */
exports.getRecorders = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_RECORDERS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_RECORDER_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_RECORDER_Get() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : GET
 *  API : /stb-api/v1/recorders/:recorderId
 *  Description : 특정 Recorder에 대한 정보 요청
 *  */
exports.getRecorderById = function (request, response) {
    var recorderId = request.param('recorderId');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_RECORDERBYID_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    /* recorderID 값은 Number type 이어야 한다. */
    var nRecorderId = Number(recorderId);
    if ((typeof nRecorderId !== 'number') || isNaN(nRecorderId)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nRecorderId;
    var arg3 = JSON.stringify(query);
    binder.REST_BIND_RECORDER_GetOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_RECORDER_GetOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : PUT
 *  API : /stb-api/v1/recorders/:recorderId
 *  Description : Recorder Control..
 *  */
exports.putRecorder = function (request, response) {
    var recorderId = request.param('recorderId');

    // recorderId 값은 Number type 이어야 한다.
    var nRecorderId = Number(recorderId);
    if ((typeof nRecorderId !== 'number') || isNaN(nRecorderId)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nRecorderId;
    var arg3 = JSON.stringify(request.body);
    binder.REST_BIND_RECORDER_PutOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_RECORDER_PutOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};