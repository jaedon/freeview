/* Schedule 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var schedule_api = {
    category: 'schedule',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/schedules',
            url: undefined,
            parameters: ['member']},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/schedules/[scheduleUid]',
            url: undefined,
            parameters: ['member']},
        {method: 'POST',
            api: RESTAPI.serverInfo.apiv1path+'/schedules',
            url: undefined,
            parameters: []},
        {method: 'PUT',
            api: RESTAPI.serverInfo.apiv1path+'/schedules/[scheduleUid]',
            url: undefined,
            parameters: []},
        {method: 'DELETE',
            api: RESTAPI.serverInfo.apiv1path+'/schedules/[scheduleUid]',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.schedule_api) {
    RESTAPI.v1.schedule_api = schedule_api.api_set;
}

var GET_SCHEDULES_ARGS = schedule_api.api_set[0].parameters;
var GET_SCHEDULEBYUID_ARGS = schedule_api.api_set[1].parameters;


/**
 *  Method : GET
 *  API : /stb-api/v1/schedules
 *  Description : 모든 Schedule에 대한 요청
 *  */
exports.getSchedules = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_SCHEDULES_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_SCHEDULE_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_SCHEDULE_Get() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : GET
 *  API : /stb-api/v1/schedules/:scheduleUid
 *  Description : Schedule Inform 요청
 *  */
exports.getScheduleByUid = function (request, response) {
    var uid = request.param('scheduleUid');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    var nUid = Number(uid);
    if ((typeof nUid !== 'number') || isNaN(nUid)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_SCHEDULEBYUID_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nUid;
    var arg3 = JSON.stringify(query);
    binder.REST_BIND_SCHEDULE_GetOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_SCHEDULE_GetOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : POST
 *  API : /stb-api/v1/schedules
 *  Description : Schedule 예약 요청
 *  */
exports.postSchedule = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_SCHEDULE_Post.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_SCHEDULE_Post() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : PUT
 *  API : /stb-api/v1/schedules/:scheduleUid
 *  Description : Schedule Update 요청
 *  */
exports.putSchedule = function (request, response) {
    var uid = request.param('scheduleUid');

    var nUid = Number(uid);
    if ((typeof nUid !== 'number') || isNaN(nUid)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nUid;
    var arg3 = JSON.stringify(request.body);
    binder.REST_BIND_SCHEDULE_PutOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_SCHEDULE_PutOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : DELETE
 *  API : /stb-api/v1/schedules/:scheduleUid
 *  Description : Schedule Delete 요청
 *  */
exports.deleteSchedule = function (request, response) {
    var uid = request.param('scheduleUid');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    var arg1 = request.sessionID;
    var arg2 = Number(uid);
    if ((typeof arg2 !== 'number') || isNaN(arg2)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }
    binder.REST_BIND_SCHEDULE_DeleteOne.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_SCHEDULE_DeleteOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};