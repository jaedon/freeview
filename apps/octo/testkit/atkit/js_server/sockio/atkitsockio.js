var fs = require('fs');
var util = require('util');
var socketio = require('socket.io');
var spawn = require('child_process').spawn;
var stb_getpid = require('./stb_getpid');

var proc_list = [];
var meminfoTimer;
var runMeminfo = 0;
var runGetSchedules = 0;
var runGetRecordings = 0;
var runMonitor = 0;

var TOP_DIR 				= '/home/atkit/'
var TC_PATH_PREFIX 			= TOP_DIR + 'tc/';
var USERAPP_PATH_PREFIX 	= TOP_DIR + 'userapp/';
var MANIFEST_PATH_PREFIX 	= TOP_DIR + 'manifest/';
var RESULT_PATH_PREFIX 		= TOP_DIR + 'result/';

var MANIFEST_EXT 			= '.manifest';
var RESULT_EXT 				= '.result';


// socket.io
//var io = socketio.listen(httpServer);
var io;

var startSocketIO = function(server) {
	io = socketio.listen(server);

	io.sockets.on('connection', function(socket) {
		socket.emit('toclient', {msg:'Websocket Connected!'});
		var msg = util.format('{"client": %s}', JSON.stringify(socket.handshake.address));
		console.log(msg);

		// get all schedules
		socket.on('get_schedules', function(data){
			get_schedules_proc(socket, data);
		});

		// get all recorgins
		socket.on('get_recordings', function(data){
			get_recordings_proc(socket, data);
		});

		// start meminfo
		socket.on('meminfo_start', function(data){
			meminfo_start_proc(socket, data);
		});

		// stop meminfo
		socket.on('meminfo_stop', function(data){
			meminfo_stop_proc(socket, data);
		});

		// start monitor
		socket.on('monitor_start', function(data){
			monitor_start_proc(socket, data);
		});

		// stop monitor
		socket.on('monitor_stop', function(data){
			monitor_stop_proc(socket, data);
		});
	});
};

exports.startSocketIO = startSocketIO;
global.startSocketIO = startSocketIO;


// Get All Schedule
var get_schedules_proc = function(socket, data) {
	if(runGetSchedules === 0){
		runGetSchedules = 1;

		get_all_schedule(function(err, result){
			var msg = {};
			if(err){
				console.log(err);
				msg.schedules = {};
			} else {
				console.log(result);
				msg.schedules = result.schedules;
			}
			socket.emit('scheduleInfo', msg);
			runGetSchedules = 0;
		});
	}
};

// Get All Recording
var get_recordings_proc = function(socket, data) {
	if(runGetRecordings === 0){
		runGetRecordings = 1;

		get_all_recording(function(err, result){
			var msg = {};
			if(err){
				console.log(err);
				msg.recordings = {};
			} else {
				console.log(result);
				msg.recordings = result.recordings;
			}
			socket.emit('recordingInfo', msg);
			runGetRecordings = 0;
		});
	}
};

// Start get mem information
var meminfo_start_proc = function(socket, data) {
	if(runMeminfo === 0) {
		var count = 0;
		console.log('meminfo_start');
		console.log(data);

		for(i = 0 ; i < data.proc_name.length ; ++i ) { 
			var proc_name = data.proc_name[i];
			stb_getpid.find(proc_name, function(error, result) {
				if(error) {
					console.log('####### get pid error #######');
					console.log(error);

				} else {
					var proc_info = {};
					proc_info.name = result.name;
					proc_info.pid = result.pid;
					proc_list.push(proc_info);
				}
				count++;
				if(data.proc_name.length == count) {

					// send first meminfo
					send_meminfo(socket);

					// refresh interval. minimum 3sec
					var interval = 3;
					if(data.duration > 3) {
						interval = data.duration;
					}
					meminfoTimer = setInterval(function() {
						send_meminfo(socket);
					}, interval*1000);

				}
			});
		}

		runMeminfo = 1;
	}
};

// Stop get mem information
var meminfo_stop_proc = function(socket, data) {
	proc_list.splice(0, proc_list.length);
	clearInterval(meminfoTimer);
	console.log('meminfo_stop');
	console.log(data);

	runMeminfo = 0;

};

var send_meminfo = function(socket) {
	var msg = {};
	msg.meminfo = [];
	for(i = 0; i < proc_list.length ; ++i) {
		var info_path = '/proc/' + proc_list[i].pid + '/status';
		var result = {};
		var mem_info = {};
		var data;
		var start;
		var end;
		var VmSize;
		var VmRSS;

		try{
			data = fs.readFileSync(info_path, 'utf8');
			start = data.indexOf('VmSize:');
			end = data.indexOf('kB', start+7);
			VmSize = data.substring(start+7, end);

			start = data.indexOf('VmRSS:');
			end = data.indexOf('kB', start+6);
			VmRSS = data.substring(start+6, end);

			mem_info.vmsize = parseInt(VmSize);
			mem_info.vmrss = parseInt(VmRSS);

		}catch(e){
			console.log(e);
			// maybe process was crashed
			mem_info.vmsize = 0;
			mem_info.vmrss = 0;
		}

		result.proc_name = proc_list[i].name;
		result.mem_info = mem_info;
		msg.meminfo.push(result);

	}

	// If would you like to watch the send message then print below. 
	//console.log(msg);
	socket.emit('meminfo', msg);
};

// run userapp : get_all_schedule
var get_all_schedule = function(callback) {
	var app_name = 'get_all_schedule';
	var runPath = USERAPP_PATH_PREFIX + app_name;
	var resultPath = RESULT_PATH_PREFIX + app_name + RESULT_EXT;

	var app = spawn(runPath, []);

	app.stdout.on('data', function(data){
		console.log(data);
	});

	app.stderr.on('data', function(data){
		console.log(data);
	});

	app.on('close', function(code){
		fs.readFile(resultPath, 'utf8', function(err, data){
			var result = {};
			if(err){
				console.log(err);
				result.error = err;
			} else {
				result = JSON.parse(data);
				console.log(result);
			}

			if(typeof callback === 'function'){
				callback(err, result);
			}
		});
	});
};

// run userapp : get_all_recording
var get_all_recording = function(callback) {
	var app_name = 'get_all_recording';
	var runPath = USERAPP_PATH_PREFIX + app_name;
	var resultPath = RESULT_PATH_PREFIX + app_name + RESULT_EXT;

	var app = spawn(runPath, []);

	app.stdout.on('data', function(data){
		console.log(data);
	});

	app.stderr.on('data', function(data){
		console.log(data);
	});

	app.on('close', function(code){
		fs.readFile(resultPath, 'utf8', function(err, data){
			var result = {};
			if(err){
				console.log(err);
				result.error = err;
			} else {
				result = JSON.parse(data);
				console.log(result);
			}

			if(typeof callback === 'function'){
				callback(err, result);
			}
		});
	});
};

// Start hama monitor information
var monitor_start_proc = function(socket, data) {
	var monitorInfoTimer;
	if(runMonitor === 0) {
		console.log('monitor_start');

		var app_name = 'atkit_ua_monitor';
		var runPath = USERAPP_PATH_PREFIX + app_name;

		var app = spawn(runPath, []);

		app.stdout.on('data', function(data){
			console.log(data);
		});

		app.stderr.on('data', function(data){
			console.log(data);
		});

		app.on('close', function(code){
			clearInterval(monitorInfoTimer);
			console.log(app_name + ' is closed. ret code( ' + code + ' )');
			runMonitor = 0;
		});

		// First information
		setTimeout(function() {
			send_monitorInfo(socket);
		}, 1000);

		var interval = 3;
		if(data.interval > 3) {
			interval = data.interval;
		}
		monitorInfoTimer = setInterval(function() {
			send_monitorInfo(socket);
		}, interval * 1000);

		runMonitor = 1;
	}
};

var send_monitorInfo = function(socket) {
	var app_name = 'atkit_ua_monitor';
	var resultPath = RESULT_PATH_PREFIX + app_name + RESULT_EXT;

	fs.readFile(resultPath, 'utf8', function(err, data){
		var result = {};
		if(err){
			console.log(err);
			result.error = err;
		} else {
			result = JSON.parse(data);
			//console.log(result);
		}

		if(typeof callback === 'function'){
			callback(err, result);
		}

		socket.emit('monitorinfo', result);
	});

};

// Stop monitor
var monitor_stop_proc = function(socket, data) {

	var cmd = 'killall';
	var app_name = 'atkit_ua_monitor';
	var app = spawn(cmd, ['-9', app_name]);

	app.stdout.on('data', function(data){
		console.log(data);
	});

	app.stderr.on('data', function(data){
		console.log(data);
	});

	app.on('close', function(code){
		console.log('killall -9 ' + app_name + ' return( ' + code + ' )');
	});
};


var emitEvent = function(name, message) {
	//console.log('emit( ' + name + ' ) : ' + message);
	io.sockets.emit(name, message);
};

exports.emitEvent = emitEvent;
global.emitEvent = emitEvent;
