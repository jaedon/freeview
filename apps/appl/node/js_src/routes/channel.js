/**
 * Created by ssyun on 14. 3. 14.
 * Channel 관련 요청을 처리하는 모듈
 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');
var fs = require('fs');

// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var channel_api = {
    category: 'channel',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/channels',
            url: undefined,
            parameters: ['member', 'listtype', 'startno', 'startidx', 'endno', 'endidx', 'cnt']},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/channels/[channelUid]',
            url: undefined,
            parameters: ['member']}
    ]
};
if (!RESTAPI.v1.channel_api) {
    RESTAPI.v1.channel_api = channel_api.api_set;
}

var GET_CHANNELS_ARGS = channel_api.api_set[0].parameters;
var GET_CHANNELSBYUID_ARGS = channel_api.api_set[1].parameters;


/**
 *  Method : GET
 *  API : /stb-api/v1/channels
 *  Description : ChnnelList 요청에 대한 응답. Query에 대한 파싱 및 응답 처리는 C Binder 내부에서 처리
 *  */
exports.getChannels = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_CHANNELS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_CHAN_GetChannels.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_CHAN_GetChannels() Return OK...');

        var resObj = JSON.parse(res);
        // 현재는 URL이 상대경로만 올라오므로, full URL을 만들어 줌
        makeThumbnailURL(resObj);
        RESTAPI.sendResult(response, JSON.stringify(resObj));

        //RESTAPI.sendResult(response, res);
    });
};

/**
 *  Method : GET
 *  API : /stb-api/v1/channels/:channelUid
 *  Description : 특정 Player에 대한 정보 요청
 *  */
exports.getChannelByUid = function (request, response) {
    var channelUid = request.param('channelUid');
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_CHANNELSBYUID_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    // playerID 값은 Number type 이어야 한다.
    var nChannelUid = Number(channelUid);
    if ((typeof nChannelUid !== 'number') || isNaN(nChannelUid)) {
        RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = nChannelUid;
    var arg3 = JSON.stringify(query);
    binder.REST_BIND_CHAN_GetChannelOne.async(arg1, arg2, arg3, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_CHAN_GetChannelOne() Return OK...');

        var resObj = JSON.parse(res);
        // 현재는 URL이 상대경로만 올라오므로, full URL을 만들어 줌
        makeThumbnailURL(resObj);
        RESTAPI.sendResult(response, JSON.stringify(resObj));

        //RESTAPI.sendResult(response, res);
    });
};


var THUMBNAIL_HOME = '//mnt/hd1/.cache/';
/**
 * Thumbnail 요청에 대하여 Thumbnail file을 전달하기 위한 함수. epg thumbnail도 공용으로 사용함.
 * @param request
 * @param response
 */
exports.thumbnailProc = function (request, response) {
    var name = request.param('name');
    var ext = RESTAPI.extractExt(name);
    var mimeType = RESTAPI.getMineTypeByExt(ext);

    RESTAPI.log('ext = %s, mimetype = %s', ext, mimeType);

    fs.readFile(THUMBNAIL_HOME + name, function (error, data) {
        if (error) {
            RESTAPI.log(error);
            RESTAPI.sendError(response, 'ERR_PAGE_NOTFOUND');
        } else {
            // Channel Logo의 경우 확장자는 .jpg 이지만 사실은 png 이다. 보정한다.
            if (request.path.indexOf('/thumbnails/channel/') !== -1) {
                mimeType = 'image/png';
                RESTAPI.log('Channel Logo... MimeType Correction ( image/jpeg -> image/png )');
            }
            response.writeHead(200, { 'Content-Type': mimeType });
            response.end(data);
        }
    });
};


/**
 * Channel의 thumbnail 값에 대하여 STB내의 상대 경로로 전달되는 값을 IP:Port 를 포함한 Full URL로 변경
 * @param resObj - Binder로 부터 전달된 결과 JSON Object
 */
function makeThumbnailURL (resObj) {
    for (var channel in resObj.Channels) {
        if (resObj.Channels.hasOwnProperty(channel)) {
            if (resObj.Channels[channel].thumbnail) {
                resObj.Channels[channel].thumbnail.url = RESTAPI.serverInfo.serverurl + resObj.Channels[channel].thumbnail.url;
            }
        }
    }
}