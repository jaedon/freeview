var fs = require('fs');
var util = require('util');
var socketio = require('socket.io');
var otainfo = require('../src/otainfo');

// socket.io
var io;

var startSocketIO = function(server) {
	console.log('Start socket.io !!');
	io = socketio.listen(server);


	io.sockets.on('connection', function(socket) {
		socket.emit('toclient', {msg:'Websocket Connected!'});
		var msg = util.format('{"client": %s}', JSON.stringify(socket.handshake.address));
		console.log(msg);

		// get all ota_info
		socket.on('get_ota_info', function(data){
			get_otainfo_proc(socket, data);
		});

		// update ota_info
		socket.on('update_ota_info', function(data){
			edit_otainfo_proc(socket, data);
		});

		// Delete ota_info
		socket.on('delete_ota_info', function(data){
			delete_otainfo_proc(socket, data);
		});

		// Delete all ota_info
		socket.on('delete_all_ota_info', function(data){
			delete_all_otainfo_proc(socket, data);
		});
	});
};

exports.startSocketIO = startSocketIO;
global.startSocketIO = startSocketIO;

var send_otainfo = function() {
	otainfo.find().sort({'versionCode':-1}).exec(function(err, fullfiles){
		if(err) {
			console.error(err);
		}
		io.sockets.emit('otaInfo', fullfiles);
		
//		socket.emit('otaInfo', fullfiles);
	});
};
exports.send_otainfo = send_otainfo;
global.send_otainfo = send_otainfo;

// Get OTA Info
var get_otainfo_proc = function(socket, data) {
	send_otainfo();
};

// Edit OTA Info
var edit_otainfo_proc = function(socket, data) {
	
	otainfo.findById(data._id, function(err, info){
		info.appName = data.appName;
		if(typeof data.versionId === 'number') {
			info.versionId = data.versionId;
		}
		info.versionDetail = data.versionDetail;
		info.versionCode = data.versionCode;
		info.dependencyVersionCode = data.dependencyVersionCode;
		info.save(function(err){
			if(err) {
				console.error(err);
			}
			else
			{
				console.log("Update Success\n");
				console.log(info);
				send_otainfo();
			}
		});

	});
};

// Delete OTA Info
var delete_otainfo_proc = function(socket, data) {

	otainfo.remove({_id:data._id}, function(err, output){
		if(err) {
			console.error(err);
		} else {
			console.log("Delete Success.\n");
			console.log(output);
			send_otainfo();

			fs.unlink(data.filePath, function(err){
				if(err) {
					console.error(err);
				} else {
					console.log('Delete file : ' + data.filePath);
				}
			});
		}

	});
};

// Delete all OTA Info
var delete_all_otainfo_proc = function(socket, data) {

	otainfo.remove({}, function(err, output){
		if(err) {
			console.error(err);
		} else {
			console.log("Collection removed!!\n");
			console.log(output);
			send_otainfo();

			files = fs.readdirSync("upload");
			for(var i = 0 ; i < files.length; ++i)
			{
				var file = files[i];
				fs.unlinkSync('upload/'+file);
				console.log('Delete file : ', 'upload/'+file);
			}
		}

	});
};

var emitEvent = function(name, message) {
	//console.log('emit( ' + name + ' ) : ' + message);
	io.sockets.emit(name, message);
};

exports.emitEvent = emitEvent;
global.emitEvent = emitEvent;
