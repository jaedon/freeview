/**
 * Created by ssyun on 2014-09-03.
 */
'use strict';

// 전역 객체 선언
if (!global.RESTAPI) {
    global.RESTAPI = {};
}
if (!global.RESTAPI.v1) {
    global.RESTAPI.v1 = {};
}

// 내부 모듈 import
var http = require('http');
var util = require('util');
var url = require('url');
var fs = require('fs');

// 외부 모듈 import
var express = require('express');
var bodyParser = require('body-parser');
//var multer = require('multer');
//var cookieParser = require('cookie-parser');
//var session = require('express-session');
var methodOverride = require('method-override');

require('colors');
var network = require('network');
var netmask = require('netmask').Netmask;
var myNetMask;

// 서브 모듈 import
require('./resterror');
require('./restutil');
require('./restsockio');
var restBinder = require('./restbinder');


// local variable
var HTTP_PORT_NUMBER = 37564;
RESTAPI.serverInfo = {};
RESTAPI.serverInfo.address = '127.0.0.1';
RESTAPI.serverInfo.netmask = '255.0.0.0';
RESTAPI.serverInfo.gateway = undefined;
RESTAPI.serverInfo.httpPort = HTTP_PORT_NUMBER;

RESTAPI.lastError = {};
var nErrCount = 0;

// 알수 없는 오류로 인한 서버의 사망 방지
process.on('uncaughtException', function (err) {
    RESTAPI.elog('----------------------------UnCaughtException---------------------------------');
    RESTAPI.elog(err);
    RESTAPI.lastError.module = 'uncaughtException';
    RESTAPI.lastError.errCount = nErrCount++;
    RESTAPI.lastError.err = err;
    RESTAPI.elog('------------------------------------------------------------------------------');
});

// Process 종료시 처리할 것들?
process.on('exit', function () {
    restBinder.unregistNotifiers();

    RESTAPI.elog('Server exit.....................................!!');
});


// Network interface 정보 수집...  Fail시 3초마다 retry
getNetworkInfo();

// C Binder init...  Fail시 3초마다 retry
initBinder();


// express 객체 생성
var app = express();


// set middleware ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// production / development 모드별 동작 설정
if ('production' === app.get('env')) {
    console.log = function () {
    };
    console.warn = function () {
    };
    console.error = function () {
    };
} else {
    if ('development' === app.get('env')) {
        var logger = require('morgan');
        var errorHandler = require('errorhandler');
        RESTAPI.log('NODE_ENV :: Development Mode');

        // option : combined, common, dev, short, tiny
        // - combined - :remote-addr - :remote-user [:date] ":method :url HTTP/:http-version" :status :res[content-length] ":referrer" ":user-agent"
        // - common - :remote-addr - :remote-user [:date] ":method :url HTTP/:http-version" :status :res[content-length]
        // - dev - :method :url :status :response-time ms - :res[content-length]
        // - short - :remote-addr :remote-user :method :url HTTP/:http-version :status :res[content-length] - :response-time ms
        // - tiny - :method :url :status :res[content-length] - :response-time ms
        app.use(logger('dev'));
        app.use(errorHandler());
    }
}

//app.use(cookieParser());

//app.use(session({
//    secret: 'humax_rest_api',
//    key: 'jstb',
//    cookie: {
//        maxAge: 60 * 1000
//    }
//}));

// parse application/json
app.use(bodyParser.json());
// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: true }));
// Multer is a node.js middleware for handling multipart/form-data.
// app.use(multer());
// Lets you use HTTP verbs such as PUT or DELETE in places where the client doesn't support it.
app.use(methodOverride());

// CORS 문제 해결을 위한 Header Option 삽입
app.use(function (req, res, next) {
    res.header('Access-Control-Allow-Origin', '*');
    next();
});

// TODO: 요청한 Client의 IP를 확인하여 IP별 처리
app.use(function (req, res, next) {
    if (RESTAPI.serverInfo.address === req.connection.remoteAddress) {
        req.ipType = 'localhost';
    } else {
        // network interface가 활성화 되지 않은 경우가 있다.
        if(myNetMask === undefined) {
            req.ipType = 'unknown';
        } else {
            if (myNetMask.contains(req.connection.remoteAddress) === true) {
                req.ipType = 'LAN';
            } else {
                req.ipType = 'WAN';
            }
        }
    }

    RESTAPI.log('Request : ' + req.path);
    RESTAPI.log('Client Address = ' + req.connection.remoteAddress);
    RESTAPI.log('Client IP type = ' + req.ipType);
    next();
});

// remote service가 off일 경우에는 다른 모든 API는 deny하고 remote-service 관련 요청만 accept함
app.use(function (req, res, next) {
    if (RESTAPI.config.remoteService === 0 && req.path !== '/stb-api/v1/access-settings') {
        RESTAPI.elog('Denied API( %s ) because RESTAPI.config.remoteService "off" !!!', req.path);
        RESTAPI.sendError(res, 'ERR_ACCESS_DENY');
    } else {
        next();
    }
});


// SessionID가 없는경우 일단 재요청해서 SessionID를 발급받도록... 향후 인증을 어떻게 ?
//app.use(function(req, res, next) {
//    var parsedObject = url.parse(req.url);
//    if (req.sessionID === 'undefined') {
//        RESTAPI.log('Undefined sessionID..  redirect : ' + parsedObject.pathname);
//        res.redirect(parsedObject.pathname);
//    }
//    next();
//});

// router 설정
require('./routes').setRouter(app);

// HTTP 서버 실행
startHttpServer();

// HTTPS 서버 실행
//startHttpsServer();



// -----------------------------------------------------------------------------
// Local functions...
// -----------------------------------------------------------------------------
// active network interface의 정보를 확인. 실패시 3초 간격으로 재 수행
function getNetworkInfo() {
    network.get_active_network_interface(function(err, obj) {

        if (err) {
            RESTAPI.elog(err);
            RESTAPI.lastError.module = 'get_active_network_interface';
            RESTAPI.lastError.errCount = nErrCount++;
            RESTAPI.lastError.err = err;
            RESTAPI.elog('Not found Active interface!!!!!!');
            setTimeout(getNetworkInfo, 3000);
        } else {
            RESTAPI.log('========= Active Network Interface =========');
            RESTAPI.log(obj);
            RESTAPI.log('============================================');

            RESTAPI.serverInfo.address = obj.ip_address;
            RESTAPI.serverInfo.netmask = obj.netmask;
            RESTAPI.serverInfo.gateway = obj.gateway_ip;
            myNetMask = new netmask(RESTAPI.serverInfo.address + '/' + RESTAPI.serverInfo.netmask);

            require('./routes').setURL();
        }
    });
}

// stb 기능을 사용하기 위한 binder layer의 초기화..  실패시 3초 간격으로 재 수행
function initBinder() {
    RESTAPI.log('REST_BIND_Init() ...........  ');
    // Rest Binder Init..  C Library 초기화...
    restBinder.binder.REST_BIND_Init.async(function (err, res) {
        if (err) {
            RESTAPI.elog(err);
            RESTAPI.elog('Error binder init!!!!!!');
            RESTAPI.lastError.module = 'REST_BIND_Init';
            RESTAPI.lastError.errCount = nErrCount++;
            RESTAPI.lastError.err = err;
            setTimeout(initBinder, 3000);
        }
        if (res !== 0) {
            RESTAPI.elog('ERROR :: Fail to REST_BIND_Init() !!!!');
            setTimeout(initBinder, 3000);
        }
        RESTAPI.log('REST_BIND_Init() ...........  SUCCESS\n\n');

        // STB의 Event를 받기 위해, Notifiers를 등록
        restBinder.registNotifiers();
    });
}

// HTTP Server Start
function startHttpServer() {
    var httpServer = http.createServer(app).listen(HTTP_PORT_NUMBER, function () {
        RESTAPI.log('##############################################################');
        RESTAPI.log('REST API Server running at http://%s:%d', RESTAPI.serverInfo.address, HTTP_PORT_NUMBER);
        RESTAPI.log('##############################################################\n\n');

        // socket.io start!!
        RESTAPI.startSocketIO(httpServer);

        RESTAPI.log('-------------------------------------------------------------------');
        RESTAPI.log('RESTAPI global variables...');
        RESTAPI.log('-------------------------------------------------------------------');
        RESTAPI.log(RESTAPI);
        RESTAPI.log('-------------------------------------------------------------------');
    });
}


// HTTPS Server Start
//function startHttpsServer() {
//    var https = require('https');
//    var HTTPS_PORT_NUMBER = HTTP_PORT_NUMBER + 1;
//    RESTAPI.serverInfo.httpsPort = HTTPS_PORT_NUMBER;
//    var options = {
//        key: fs.readFileSync('/usr/woon/servkey.pem'),
//        cert: fs.readFileSync('/usr/woon/servcert.pem')
//    };
//    https.createServer(options, app).listen(HTTPS_PORT_NUMBER, function () {
//        RESTAPI.log('##############################################################');
//        RESTAPI.log('REST API Server running at https://%s:%d', RESTAPI.serverInfo.address, HTTPS_PORT_NUMBER);
//        RESTAPI.log('##############################################################\n\n');
//    });
//}
