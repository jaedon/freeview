/* Player 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var player_api = {
    category: 'player',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/players',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/players/[playerId]',
            url: undefined,
            parameters: []},
        {method: 'PUT',
            api: RESTAPI.serverInfo.apiv1path+'/players/[playerId]',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.player_api) {
    RESTAPI.v1.player_api = player_api.api_set;
}

var GET_PLAYERS_ARGS = player_api.api_set[0].parameters;
var GET_PLAYERSBYID_ARGS = player_api.api_set[1].parameters;
//var PUT_PLAYERSBYID_ARGS = player_api.api_set[2].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/players
 *  Description : STB내에 존재하는 Player에 대한 정보 요청
 *  */
exports.getPlayers = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_PLAYERS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_PLAYER_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_PLAYER_Get() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : GET
 *  API : /stb-api/v1/players/:playerId
 *  Description : 특정 Player에 대한 정보 요청
 *  */
exports.getPlayerById = function (request, response) {
    var playerId = request.param('playerId');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_PLAYERSBYID_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    /* playerID 값은 Number type 이어야 한다. */
    var nPlayerId = Number(playerId);
    if ((typeof nPlayerId !== 'number') || isNaN(nPlayerId)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nPlayerId;
    var arg3 = JSON.stringify(query);
    binder.REST_BIND_PLAYER_GetOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_PLAYER_GetOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : PUT
 *  API : /stb-api/v1/players/:playerId
 *  Description : Player Control..  play, stop, ,,,
 *  */
exports.putPlayer = function (request, response) {
    var playerId = request.param('playerId');

    // playerID 값은 Number type 이어야 한다.
    var nPlayerId = Number(playerId);
    if ((typeof nPlayerId !== 'number') || isNaN(nPlayerId)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nPlayerId;
    var arg3 = JSON.stringify(request.body);
    binder.REST_BIND_PLAYER_PutOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_PLAYER_PutOne() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};