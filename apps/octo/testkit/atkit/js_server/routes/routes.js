var fs = require('fs');
var url = require('url');
var querystring = require('querystring');
var spawn = require('child_process').spawn;
var multer = require('multer');
var express    = require('express');
var bodyParser = require('body-parser');

var TOP_DIR 				= '/home/atkit/'
var TC_PATH		 			= TOP_DIR + 'tc/';
var USERAPP_PATH		 	= TOP_DIR + 'userapp/';
var MANIFEST_PATH		 	= TOP_DIR + 'manifest/';
var RESULT_PATH		 		= TOP_DIR + 'result/';

var MANIFEST_EXT 			= '.manifest';
var RESULT_EXT 				= '.result';

var tc_app_upload = multer({ dest: TC_PATH });
var user_app_upload = multer({ dest: USERAPP_PATH });
var manifest_upload = multer({ dest: MANIFEST_PATH });

exports.setRouter = function(app) {
	// Router Setting
	app.get('/', get_main_page);
	app.get('/atkit', get_atkit_page);
	app.get('/upload/tc_app', get_upload_tc_app);
	app.get('/upload/user_app', get_upload_user_app);
	app.get('/list/tc', get_tc_list);
	app.get('/list/manifest', get_manifest_list);
	app.get('/list/userapp', get_userapp_list);
	app.get('/tc/:app_name', run_tc_proc);
	app.get('/manifest/:app_name', get_manifest);
	//app.get('/app/:app_name', run_app_proc);
	app.get('/meminfo', meminfo_proc);
	app.get('/scheduleinfo', scheduleinfo_proc);
	app.get('/monitor', monitor_proc);

	app.post('/upload/tc_app', tc_app_upload.single('uploadFile'), post_upload_tc_app);
	app.post('/upload/user_app', user_app_upload.single('uploadFile'), post_upload_user_app);
	app.post('/upload/manifest', manifest_upload.single('uploadFile'), post_upload_manifest);

	//app.post('/tc/:app_name', run_tc_proc);
	app.post('/manifest/:app_name', post_update_manifest);
	//app.post('/app/:app_name', run_app_proc);

	app.post('/linuxcmd', post_linux_cmd);
};


// GET Methods ---------------------------------------------------
function get_main_page(req, res) {
	fs.readFile('./public/index.html', function (error, data) {
		if (error) {
			console.log(error);
		} else {
			res.writeHead(200, { 'Content-Type': 'text/html' });
			res.end(data);
		}
	});
};

function get_atkit_page(req, res) {
	fs.readFile('./public/AppkitTestKit.html', function (error, data) {
		if (error) {
			console.log(error);
		} else {
			res.writeHead(200, { 'Content-Type': 'text/html' });
			res.end(data);
		}
	});
};

function meminfo_proc(req, res) {

	fs.readFile('./public/MemInfo.html', function (error, data) {
		if (error) {
			console.log(error);
		} else {
			res.writeHead(200, { 'Content-Type': 'text/html' });
			res.end(data);
		}
	});
};

function scheduleinfo_proc(req, res) {

	fs.readFile('./public/ScheduleInfo.html', function (error, data) {
		if (error) {
			console.log(error);
		} else {
			res.writeHead(200, { 'Content-Type': 'text/html' });
			res.end(data);
		}
	});
};

function monitor_proc(req, res) {

	fs.readFile('./public/Monitor.html', function (error, data) {
		if (error) {
			console.log(error);
		} else {
			res.writeHead(200, { 'Content-Type': 'text/html' });
			res.end(data);
		}
	});
};

function get_upload_tc_app(req, res) {
	fs.readFile('./uploadTC.html', function (error, data) {
		if (error) {
			console.log(error);
		} else {
			res.writeHead(200, { 'Content-Type': 'text/html' });
			res.end(data);
		}
	});
};

function get_upload_user_app(req, res) {
	fs.readFile('./uploadUserApp.html', function (error, data) {
		if (error) {
			console.log(error);
		} else {
			res.writeHead(200, { 'Content-Type': 'text/html' });
			res.end(data);
		}
	});
};

function get_tc_list(req, res) {
	fs.readdir('./tc/', function(err, files){
		if(err) {
			throw err;
		}
		console.log(files);

		var result = {tc_app_list:[]};
		result.tc_app_list = files;
		res.send(result);
	});
};

function get_manifest_list(req, res) {
	fs.readdir('./manifest/', function(err, files){
		if(err) {
			throw err;
		}
		console.log(files);

		var result = {tc_manifest_list:[]};
		result.tc_manifest_list = files;
		res.send(result);
	});
};

function get_userapp_list(req, res) {
	fs.readdir('./userapp/', function(err, files){
		if(err) {
			throw err;
		}
		console.log(files);

		var result = {user_app_list:[]};
		result.user_app_list = files;
		res.send(result);
	});
};

// POST Methods ---------------------------------------------------
function post_upload_tc_app(req, res) {
	var target = req.file.destination + req.file.originalname;

	fs.rename(req.file.path, target, function(err){
		if(err) {
			console.log(err);
			res.status(500).send('Fail to file upload..');
		} else {
			console.log('TC-App is uploaded( %s )', target);
			res.status(200).send('Success to file upload - ' + req.file.originalname);

			spawn('chmod', ['755', target]);
		}
	});
};

function post_upload_user_app(req, res) {
	var target = req.file.destination + req.file.originalname;

	fs.rename(req.file.path, target, function(err){
		if(err) {
			console.log(err);
			res.status(500).send('Fail to file upload..');
		} else {
			console.log('User-App is uploaded( %s )', target);
			res.status(200).send('Success to file upload - ' + req.file.originalname);

			spawn('chmod', ['755', target]);
		}
	});
};

function post_upload_manifest(req, res) {
	var target = req.file.destination + req.file.originalname;

	fs.rename(req.file.path, target, function(err){
		if(err) {
			console.log(err);
			res.status(500).send('Fail to file upload..');
		} else {
			console.log('Manifest is uploaded( %s )', target);
			res.status(200).send('Success to file upload - ' + req.file.originalname);

			spawn('chmod', ['644', target]);
		}
	});
};

function post_update_manifest(req, res) {
	var app_name= req.params.app_name;
	var target = MANIFEST_PATH + app_name;
	
	console.log(app_name);	
	console.log(target);
	console.log(req.body.data);

	// Delete old manifest file
	if(fs.existsSync(target) == true)
	{
		try {
			fs.unlinkSync(target);
		} catch(e) {
			console.log('Error :: fs.unlinkSync( %s )', target);
			console.log(e);
		}
		console.log('Success to fs.unlinkSync(%s)', target);
	}	

	fs.writeFile(target, req.body.data, function(err) {
		var result = {};
		if(err) {
			console.log(err);
			res.status(500).send('Fail to file update..');
		}else {
			console.log('Manifest is updated( %s )', target);
			res.status(200).send('Success to file update - ' + target);

			spawn('chmod', ['644', target]);
		} 
	});	
};

function run_tc_proc(req, res) {
	var app_name= req.params.app_name;
	run_app(req, res, app_name, 'tc');
};

function get_manifest(req, res) {
	var app_name= req.params.app_name;

	console.log(app_name);

	fs.readFile('./manifest/' + app_name, function(err, data){
		var result = {};
		if(err) {
			console.log(err);
			result.error = err;
		} else {
			result = JSON.parse(data);
			console.log(result);
		}

		//send result
		res.status(200).send(result);
	});
};

function run_app_proc(req, res) {
	var app_name = req.params.app_name;
	run_app(req, res, app_name, 'userapp');
};


// run linux command
function post_linux_cmd(req, res) {
	var cmdObject = req.body;
	var cmdString = cmdObject.cmd;
	var args = cmdObject.args;

	var run_cmd;

	console.log('Request Linux CMD');
	console.log(cmdObject);
	if(args != undefined)
	{
		// run with arguments
		run_cmd = spawn(cmdString, args);
	}
	else
	{
		// no argument command
		run_cmd= spawn(cmdString, []);
	}

	run_cmd.on('error', function(err){
		var message = {};
		message.msg = err.path + ': command not found';
		console.log(err);
		emitEvent('error', message);

		var result = {};
		result.returnCode = -1;
		res.status(200).send(result);
	});

	run_cmd.stdout.on('data', function(data){
		var message = {};
		message.msg = data;
		console.log(message.msg);
		emitEvent('stdout', message);
	});

	run_cmd.stderr.on('data', function(data){
		var message = {};
		message.msg = data;
		console.log(message.msg);
		emitEvent('stderr', message);
	});

	run_cmd.on('close', function(code){
		var result = {};
		result.returnCode = code;
		
		//send result
		res.status(200).send(result);
	});
};


function run_app(req, res, app_name, type) {

	var app_file;
	var manifest_file;
	var result_file;
	var runApp;

	if(type === 'tc') {
		app_file = TC_PATH + app_name;
	} else if(type === 'userapp') {
		app_file = USERAPP_PATH + app_name;
	} else {
		app_file = TC_PATH + app_name;
	}

	manifest_file = MANIFEST_PATH + app_name + MANIFEST_EXT;
	result_file = RESULT_PATH + app_name + RESULT_EXT;

	// Delete result_file that was created before
	if(fs.existsSync(result_file) == true)
	{
		try {
			fs.unlinkSync(result_file);
		} catch(e) {
			console.log('Error :: fs.unlinkSync( %s )', result_file);
			console.log(e);
		}
		console.log('Success to fs.unlinkSync(%s)', result_file);
	}

	// No arguments
	runApp = spawn(app_file, []);

	console.log('##################################################');
	console.log('Run :: ' + app_file);
	console.log('##################################################');

	runApp.stdout.on('data', function(data){
		var message = {};
		message.msg = data;
		console.log(message.msg);
		emitEvent('stdout', message);
	});

	runApp.stderr.on('data', function(data){
		var message = {};
		message.msg = data;
		console.log(message.msg);
		emitEvent('stderr', message);
	});

	// If close event is received, must send result that has been written in [app name].result json file.
	runApp.on('close', function(code){

		console.log('##################################################');
		console.log(app_file + ' Return ::' + code);
		console.log('##################################################');

		fs.readFile(result_file, 'utf8',  function(err, data) {
			var result = {};
			if(err) {
				console.log(err);
				result.error = err;
			} else {

				result = JSON.parse(data);
				console.log(result);
			}

			//send result
			res.status(200).send(result);

		});
	});
};
