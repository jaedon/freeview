var dgram = require('dgram');
var server = dgram.createSocket('udp4');

var retMsg = new Buffer('Receive OK');

server.on('message', function(msg, rinfo) {
	console.log('server got: ' + msg + ' from ' + rinfo.address + ':' + rinfo.port);
	server.send(retMsg, 0, retMsg.length, rinfo.port, rinfo.address);
});

server.on('listening', function() {
	var address = server.address();
	console.log('server listening ' + address.address + ':' + address.port);
});

server.bind(12345);
