/**
 * Created by ssyun on 14. 3. 3.
 */
'use strict';

var http = require('http');
var fs = require('fs');
var util = require('util');
var socketio = require('socket.io');

var TEST_PORT = 52273;

var server = http.createServer(function (request, response) {
//    fs.readFile('socketio_test.html', function(error, data) {
    fs.readFile('socketio_test.html', function (error, data) {
        if (error) {
            console.log(error);
        } else {
            response.writeHead(200, { 'Content-Type': 'text/html' });
            response.end(data);
        }
    });
}).listen(TEST_PORT, function () {
        console.log('Server runnging at http://127.0.0.1:%d', TEST_PORT);
    });

var clientCount = 0;
var timerId = 0;
var io = socketio.listen(server);
io.set('log level', 2);

io.sockets.on('connection', function (socket) {
    var msg = util.format('{"Client Connected": %s}', JSON.stringify(socket.handshake.address));
    console.log(msg);
    io.sockets.emit('Connection', JSON.parse(msg));

    if (clientCount === 0) {
        timerId = setInterval(function () {
            var msg = util.format('{"MemUsage": %s}', JSON.stringify(process.memoryUsage()));
            io.sockets.emit('MemUsage', JSON.parse(msg));
        }, 5000);

        console.log('MemUsage Event On');
    }

    clientCount++;

    socket.on('disconnect', function () {
        msg = util.format('{"Client DisConnected": %s}', JSON.stringify(socket.handshake.address));
        console.log(msg);
        io.sockets.emit('Connection', JSON.parse(msg));

        clientCount--;
        if (clientCount === 0) {
            clearInterval(timerId);
            console.log('MemUsage Event Off');
        }
    });


});
