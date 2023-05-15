/**
 * Created by ssyun on 2014-04-28.
 */
'use strict';

// 내부 모듈 import
var util = require('util');


// 외부 모듈 import
var socketio = require('socket.io');


// 서브 모듈 import


// Local Variable
var io;

/**
 * socket.io 실행을 위한 start 함수
 * @param server - socket.io와 연동할 http server 객체
 */
var startSocketIO = function (server) {
    RESTAPI.log('START Socket.IO.........................');
    io = socketio.listen(server);
    io.sockets.on('connection', function (socket) {
        var client = {};
        RESTAPI.log('[Socket.io] Client Connected : ' + JSON.stringify(socket.handshake.address));

        client['user-agent'] = socket.handshake.headers['user-agent'];
        client.address = socket.handshake.address.address;
        client.port = socket.handshake.address.port;

        // 다른 Client들에게 Connect를 알림
        io.emit('clientConnect', client);

        socket.on('disconnect', function() {
            var client = {};
            RESTAPI.log('[Socket.io] Client Disconnected : ' + JSON.stringify(socket.handshake.address));

            client['user-agent'] = socket.handshake.headers['user-agent'];
            client.address = socket.handshake.address.address;
            client.port = socket.handshake.address.port;

            // 다른 Client들에게 Disconnect를 알림
            io.emit('clientDisconnect', client);
        });
    });
};
exports.startSocketIO = startSocketIO;
global.RESTAPI.startSocketIO= startSocketIO;


/**
 * Event Emitter.. 일단 public 통신으로 모든 client에게 전달
 * @param name - 전달할 event 이름
 * @param message - 전달할 event 내용, JSON Object로 전달되어야 함.
 */
var emitEvent = function (name, message) {
    // message는 이미 JSON Object
    io.sockets.emit(name, message);
};
exports.emitEvent = emitEvent;
global.RESTAPI.emitEvent= emitEvent;