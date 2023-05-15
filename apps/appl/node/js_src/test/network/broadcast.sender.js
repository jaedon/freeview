var network = require('network');
var netmask = require('netmask').Netmask;
var dgram = require('dgram');

process.on('exit', function(){
	console.log('Process Exit!!!');
});


network.get_active_network_interface(function(err, obj) {
	if (err) {
		console.log(err);
		throw err;
	}


	var block = new netmask(obj.ip_address + '/' + obj.netmask);

	var message = new Buffer('TEST DATA!!');
	var client = dgram.createSocket('udp4');

	client.bind();
	client.on('listening', function() {
		client.setBroadcast(true);
		setInterval(function() {
			client.send(message, 0, message.length, 12345, block.broadcast, function(err, bytes) {
				if (err) {
					console.log(err);
				}
				console.log('Send dgram %d Bytes', bytes);

			});
		}, 3000);

	});
	client.on('message', function(message){
		console.log('Get message back: ', message.toString());

	});

});
