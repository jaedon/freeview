/**
 * Created by ssyun on 2014-04-10.
 */
'use strict';

// 전역 객체 선언
if (!global.RESTAPI.restUtil) {
    global.RESTAPI.restUtil = {};
}

// 내부 모듈 import
var util = require('util');
var fs = require('fs');


// 외부 모듈 import


// 서브 모듈 import


// Local Variable



// Load rest-config
var configFileName = '/home/node/rest.config.json';
fs.readFile(configFileName, 'utf8', function (error, data) {
    if (error) {
        RESTAPI.elog(error);
    } else {
        RESTAPI.log('Rest Config.... ==========================================');
        RESTAPI.config = JSON.parse(data);
        RESTAPI.log(RESTAPI.config);
        RESTAPI.log('==========================================================');
    }
});


/**
 * Remote-Access를 on/off 할 경우 그 설정을 파일에 기록 함
 * @param onOff
 */
var setRemoteService = function (onOff) {
    RESTAPI.config.remoteService = onOff;
    RESTAPI.log('Set remote service : ' + onOff);
    fs.writeFile(configFileName, JSON.stringify(RESTAPI.config, null, 4), function(error) {
        if (error) {
            RESTAPI.elog(error);
        } else {
            RESTAPI.log('Save Config Success( %s ) !!!!!!!!!!!!!!!!!!!!!!!!!', configFileName);
        }
    });
};
exports.setRemoteService = setRemoteService;
global.RESTAPI.restUtil.setRemoteService = setRemoteService;
global.RESTAPI.setRemoteService = setRemoteService;


/**
 * Query문이 유효한지 검사하기 위한 함수
 * argumentSet 이외의 argument가 query문에 포함되어 있는지 검사하여 유효성을 검사한다.
 * @param query - 요청온 query문의 전문
 * @param argumentSet - 검사할 argument의 배열 set
 * @returns {boolean} - Query문의 유효성 여부
 */
var isValidQuery = function (query, argumentSet) {
    var args = Object.keys(query);

    for (var i in args) {
        if (args.hasOwnProperty(i)) {
            if (argumentSet.indexOf(args[i]) < 0) {
                RESTAPI.log('Invalid Query( %s )!!! Argument( %s ) - ', JSON.stringify(query), args[i]);
                return false;
            }
        }
    }
    return true;
};
exports.isValidQuery = isValidQuery;
global.RESTAPI.restUtil.isValidQuery= isValidQuery;


/**
 * Custom console log...  Normal
 */
var log = function () {
    var msg = util.format.apply(this, arguments);
    console.log('REST API |'.bold.red.whiteBG + '  ' + msg.yellow);
};
exports.log = log;
global.RESTAPI.restUtil.log = log;
global.RESTAPI.log = log;


/**
 * Custom console log...  Error
 */
var elog = function () {
    var msg = util.format.apply(this, arguments);
    console.log('REST API |'.bold.white.redBG + '  ' + msg.red);
};
exports.elog = elog;
global.RESTAPI.restUtil.elog = elog;
global.RESTAPI.elog = elog;


/**
 * path를 통해 확장자를 추출하기 위한 파싱 함수.
 * @param path - 전체 경로
 * @returns {*} - 파일 확장자
 */
var extractExt = function(path) {
    var len = path.length;
    var last = path.lastIndexOf("."); //확장자 추출
    if (last === -1) { //. 를 발견하지 못한다면.
        return false; //확장자가 없음.
    }
    //var ext = path.substr(last, len - last );  //확장자 추출 (. 포함)
    var ext = path.substring(last, len);  //확장자 추출 (. 포함)
    ext = ext.toLowerCase(); //소문자로.
    return ext; //.jpg
};
exports.extractExt = extractExt;
global.RESTAPI.restUtil.extractExt = extractExt;
global.RESTAPI.extractExt = extractExt;


/**
 * 확장자에 따른 각 mimetype을 리턴해 주는 함수
 * @param ext - 파일의 확장자
 * @returns {*} - 해당 파일의 mimetype
 */
var getMineTypeByExt = function(ext) {
    var mimeType;
    switch (ext) {
        case '.html':
            mimeType = 'text/html';
            break;

        case '.css':
            mimeType = 'text/css';
            break;

        case '.js':
            mimeType = 'text/javascript';
            break;

        case '.xml':
            mimeType = 'text/xml';
            break;

        case '.jpg':
            mimeType = 'image/jpeg';
            break;

        case '.png':
            mimeType = 'image/png';
            break;

        case '.bmp':
            mimeType = 'image/bmp';
            break;

        case '.gif':
            mimeType = 'image/gif';
            break;

        default:
            mimeType = 'text/html';
            break;
    }
    return mimeType;
};
exports.getMineTypeByExt = getMineTypeByExt;
global.RESTAPI.restUtil.getMineTypeByExt = getMineTypeByExt;
global.RESTAPI.getMineTypeByExt = getMineTypeByExt;
