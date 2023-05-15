/* 개발 지원용 Hidden URL 처리 */
'use strict';

// 내부 모듈 import
var util = require('util');
var os = require('os');


// 외부 모듈 import


// 서브 모듈 import


// Local Variable
var development_api = {
    category: 'development',
    api_set: [
        {method: 'GET',
            api: '/health',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/dev/util/ps',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/dev/util/memusage',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/dev/util/processinfo',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/dev/util/osinfo',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: '/dev/util/geterror',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.development_api) {
    RESTAPI.v1.development_api = development_api.api_set;
}


/**
 *  Method : GET
 *  API : /dev/util/memusage
 *  Description : Memory Usage 관련 정보 전달
 *  */
exports.memUsage = function (request, response) {
    response.send(process.memoryUsage());
};


/**
 *  Method : GET
 *  API : /dev/util/geterror
 *  Description : 마지막 발생한 error의 정보
 *  */
exports.getError = function (request, response) {
    response.send(RESTAPI.lastError);
};

/**
 *  Method : GET
 *  API : /dev/util/ps
 *  Description : 실행중인 Process List 전달
 *  */
exports.ps = function (request, response) {
    var spawn = require('child_process').spawn;
    var ps = spawn('ps', []);

    ps.stdout.on('data', function (data) {
        RESTAPI.log('ps \n' + data);
        response.send(data);
    });
};


/**
 *  Method : GET
 *  API : /dev/util/processinfo
 *  Description : Node Process 관련 정보 전달
 *  */
exports.processinfo = function (request, response) {
    var result = util.format(
        '{"process.env": %s,' +
            '"process.version": "%s",' +
            '"process.versions": %s,' +
            '"process.arch": "%s",' +
            '"process.platform": "%s",' +
            '"process.memoryUsage": %s,' +
            '"process.uptime": "%s"}',
        JSON.stringify(process.env),
        process.version,
        JSON.stringify(process.versions),
        process.arch,
        process.platform,
        JSON.stringify(process.memoryUsage()), process.uptime()
    );

    response.send(JSON.parse(result));
};


/* OS 관련 정보 */
/**
 *  Method : GET
 *  API : /dev/util/osinfo
 *  Description : OS 관련 정보 전달
 *  */
exports.osinfo = function (request, response) {
    var result = util.format(
        '{"os.hostname": "%s",' +
            '"os.type": "%s",' +
            '"os.platform": "%s",' +
            '"os.arch": "%s",' +
            '"os.release": "%s",' +
            '"os.uptime": "%s",' +
            '"os.loadavg": "%s",' +
            '"os.totalmem": "%s",' +
            '"os.freemem": "%s",' +
            '"os.cpus": %s,' +
            '"os.getNetworkInterfaces": %s}',
        os.hostname(),
        os.type(),
        os.platform(),
        os.arch(),
        os.release(),
        os.uptime(),
        os.loadavg(),
        os.totalmem(),
        os.freemem(),
        JSON.stringify(os.cpus()),
        JSON.stringify(os.getNetworkInterfaces())
    );

    response.send(JSON.parse(result));
};


/* Health Check용 API */
/**
 *  Method : GET
 *  API : /health
 *  Description : pid, memory, uptime 등의 정보 전달
 *  */
exports.healthCheck = function (request, response) {
    response.send({
        pid: process.pid,
        memory: process.memoryUsage(),
        uptime: process.uptime()
    });
};