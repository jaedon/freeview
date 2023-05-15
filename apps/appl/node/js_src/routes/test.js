/**
 * Created by ssyun on 14. 3. 14.
 * test API를 처리하는 모듈
 */
'use strict';

// 내부 모듈 import
var fs = require('fs');
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;


// Local Variable
var test_api = {
    category: 'test',
    api_set: [
        {method: 'GET',
            api: '/test/socketio',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/test/session',
            url: undefined,
            parameters: ['key=testkey1']},
        {method: 'GET',
            api: '/test/exitserver',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/test/pincode',
            url: undefined,
            parameters: ['pin=0000']},
        {method: 'GET',
            api: '/jstbpage/*',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/webapps/*',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.test_api) {
    RESTAPI.v1.test_api = test_api.api_set;
}


/**
 *  Method : GET
 *  API : /test/socketio
 *  Description : SocketIO Test를 위한 API. Test용 HTML Page를 제공
 *  */
exports.testSocketio = function (request, response) {
    fs.readFile('/home/node/test/socket.io/socketio_test.html', function (error, data) {
        if (error) {
            RESTAPI.log(error);
            RESTAPI.sendError(response, 'ERR_PAGE_NOTFOUND');
        } else {
            response.writeHead(200, { 'Content-Type': 'text/html' });
            response.end(data);
        }
    });
};

/**
 *  Method : GET
 *  API : /test/session
 *  Description : Session Test를 위한 API.
 *  */
exports.testSession = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    var store = request.sessionStore;

    if (query.key === 'testkey1') {
        request.session.validUser = true;
        response.send('Welcome to Valid User ^^');
        return;
    }

    store.get(request.sessionID, function (err, sess) {
        if (err) {
            RESTAPI.log(err);
        }

        if (sess) {
            RESTAPI.log(sess);
            if (sess.validUser === true) {
                response.send('Welcome to Valid User ^^');
                return;
            }
        }

        response.send('Get Out Invalid User!!!!!!!');
    });

//    var output = {};
//    output.cookies = request.cookies;
//    output.session = request.session;
//
//    request.session.testKey = 'this is test key';
//    RESTAPI.log('-------------------------------------------');
//    RESTAPI.log(request.sessionStore);
//    store.get(request.sessionID, function(err, sess) {
//        if(err) {
//            RESTAPI.log(err);
//        }
//
//        RESTAPI.log('#############################');
//        RESTAPI.log(sess);
//        RESTAPI.log('#############################');
//    })
//    RESTAPI.log('-------------------------------------------\n\n');
//    request.session.now = (new Date()).toUTCString();
//
//    response.send(output);
};


/**
 *  Method : GET
 *  API : /test/exitserver
 *  Description : 서버 강제 종료 테스트
 *  */
exports.exitServer = function (request, response) {
    RESTAPI.log('Request : ' + request.path);
    RESTAPI.log('#############################');
    RESTAPI.log('Node Server Exit By /test/exitserver API !!!!!!');
    RESTAPI.log('#############################');
    response.sendStatus(200);

    process.abort();
};


/**
 *  Method : GET
 *  API : /test/pincode?pin=xxxx
 *  Description : PIN code 확인 테스트용 API
 *  */
exports.pincodeVerify = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    if (query.pin) {
        binder.REST_BIND_PINCODE_VerifyPincode.async(query.pin, function (err, res) {
            if (err) {
                throw err;
            }
            RESTAPI.log('PIN CODE( ' + query.pin + ' ) is ' + res );
            if (res) {
                response.send('Valid PIN code...');
            } else {
                response.send('Invalid PIN Code!!!!!!');
            }
        });
    }
};


var JSTB_HOME = '/usr';
/**
 *  Method : GET
 *  API : /jstbtest/:page
 *  Description : Session Test를 위한 API.
 *  */
exports.testJSTB = function (request, response) {
    var parsedObject = url.parse(request.url);
    var ext = RESTAPI.extractExt(parsedObject.pathname);
    var mimeType = RESTAPI.getMineTypeByExt(ext);

    RESTAPI.log('ext = %s, mimetype = %s', ext, mimeType);

    fs.readFile(JSTB_HOME + parsedObject.pathname, function (error, data) {
        if (error) {
            RESTAPI.log(error);
            RESTAPI.sendError(response, 'ERR_PAGE_NOTFOUND');
        } else {
            response.writeHead(200, { 'Content-Type': mimeType });
            response.end(data);
            //response.send(data);
        }
    });
};

var WEB_APP_HOME = '/usr/browser';
exports.testWebApp = function (request, response) {
    var parsedObject = url.parse(request.url);
    var ext = RESTAPI.extractExt(parsedObject.pathname);
    var mimeType = RESTAPI.getMineTypeByExt(ext);

    RESTAPI.log('ext = %s, mimetype = %s', ext, mimeType);

    fs.readFile(WEB_APP_HOME + parsedObject.pathname, function (error, data) {
        if (error) {
            RESTAPI.log(error);
            RESTAPI.sendError(response, 'ERR_PAGE_NOTFOUND');
        } else {
            response.writeHead(200, { 'Content-Type': mimeType });
            response.end(data);
            //response.send(data);
        }
    });
};
