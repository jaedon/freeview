var network = require('network');
var netmask = require('netmask').Netmask;


network.get_public_ip(function(err, ip) {
	console.log('===== public ip =====');
	console.log( err || ip );
});

network.get_private_ip(function(err, ip) {
	console.log('===== private ip =====');
	console.log( err || ip );
});

network.get_gateway_ip(function(err, ip) {
	console.log('===== gateway ip =====');
	console.log( err || ip );
});
network.get_network_interfaces_list(function(err, list) {
	console.log('===== network interfaces =====');
	console.log( err || list );
});




network.get_active_network_interface(function(err, obj) {
	if (err) {
		console.log(err);
		throw err;
	}
	console.log('===== active network interface =====');
	console.log(obj);
	console.log('====================================');
	


	var block = new netmask(obj.ip_address + '/' + obj.netmask);

	console.log(block.base);
	console.log(block.mask);
	console.log(block.hostmak);
	console.log(block.bitmask);
	console.log(block.size);
	console.log(block.broadcast);
	console.log(block.first);
	console.log(block.last);
	console.log(block.toString());

	console.log('##### net mask test #####');
	ip = '192.168.0.5';
	console.log(ip + '  ' + block.contains(ip));
	ip = '192.168.100.5';
	console.log(ip + '  ' + block.contains(ip));
	ip = '192.168.0.254';
	console.log(ip + '  ' + block.contains(ip));
	ip = '224.168.0.254';
	console.log(ip + '  ' + block.contains(ip));
	console.log('#########################');
	
});
