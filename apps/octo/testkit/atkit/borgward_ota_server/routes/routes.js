var path = require('path');
var mime = require('mime');
var fs = require('fs');
var url = require('url');
var md5 = require('../src/md5');
var multer = require('multer');
var upload = multer({dest: 'upload/'});
var otainfo = require('../src/otainfo');
var async = require('async');

// ##############################################
// Global variable
// ##############################################
var g_ip_address = getIPAddress();
var g_latest_ota_info = {};
var ErrorCode =
{
	NO_ERROR:				0,
	SIGNATURE_IS_INVALID:	1,
	CURRENTVERSION_IS_NULL: 2,
	DEVICEID_IS_NULL: 		3,
	PDSN_IS_NOT_EXIST:		4,
	VERSION_IS_NOT_FOUND:	5,
	VERSION_IS_LATEST:		6,
	SERVICE_NOT_FOUND:		7,
};

// ##############################################
// Router
// ##############################################
exports.setRouter = function(app) {
	// Router Setting

	app.get('/rs/:filename', download_proc);
	app.post('/upload', upload.single('userfile'), post_upload_proc);

	app.post('/api/update-center/checkHuVersion', checkHuVersion_proc);
	app.post('/api/update-center/reportVersion', reportVersion_proc);


	app.get('*', default_proc);
	app.post('*', default_proc);
};

// ##############################################
// Router Method
// ##############################################
// GET Methods ---------------------------------------------------
function download_proc(req, res) {
	var req_name = req.params.filename;
	var file = __dirname + '/../upload/' + req_name;

	var filename = path.basename(file);
	var mimetype = mime.lookup(file);

	console.log("Download file( " + filename + " )");

	res.setHeader('Content-disposition', 'attachment; filename=' + filename);
	res.setHeader('Content-type', mimetype);

	var filestream = fs.createReadStream(file);
	filestream.pipe(res);
};

function default_proc(req, res) {
	var resData = {};

	console.log(req.path);
	console.log(req.body);

	resData = getErrorResult(ErrorCode.SERVICE_NOT_FOUND);
	res.send(resData);
};

// POST Methods ---------------------------------------------------
function post_upload_proc(req, res) {
	if( req.file === undefined ||
		req.file.destination === undefined ||
		req.file.originalname === undefined) {
		console.log('Illegal request.');
		res.status(400).send('Please select file first!');
		return;
	}

	var file_md5 = md5.getCheckSum(req.file.path);
	otainfo.find({md5: file_md5}, function(err, docs){
		if(err){
			console.error(err);
			res.satus(500).send(err);
			return;
		}
		console.log('docs.length = ' + docs.length);

		if(docs.length == 0) {
			// new file
			var target = req.file.destination + req.file.originalname;
			fs.rename(req.file.path, target, function(err){
				if(err) {
					console.log(err);
					res.status(500).send(err);
				}
				else
				{
					var obj = {};
					obj.filePath = target;
					obj.fileUrl = 'http://' + g_ip_address + ':' + global.PORT_NUM + '/rs/' + req.file.originalname;;
					obj.md5 = file_md5;
					obj.fileSize = getFilesizeInBytes(target);

					var newOtaInfo = new otainfo(obj); 
					newOtaInfo.save(function(err, newOtaInfo){
						if(err) {
							console.error(err);
							res.status(500).send('Fail to file upload - ' + req.file.originalname);
						} else {
						
							console.log("save success.!\n" + JSON.stringify(obj));
							res.status(200).send('Uploaded: '+req.file.originalname+'( '+obj.fileSize+' bytes )');
							send_otainfo();
						}
					});
				}
			});
		} else {
			// already existed file
			console.log("There is already same file.!\n");
			res.status(500).send("There is already same file.");
			fs.unlink(req.file.path, function(err){
				if(err) {
					console.error(err);
				} else {
					console.log('Delete file : ' + req.file.path);
				}
			});
		}


	});
				
};

function checkHuVersion_proc(req, res) {
	var resData = {};
	var query = url.parse(req.url, true).query;

	console.log("\n\n#####################################");
	console.log("New Request!! IP : " + getUserIP(req));
	console.log("#####################################");
	console.log(req.path);
	console.log(query);
	console.log(req.body);

	var appkey = query.appkey;
	var sign = query.sign;
	var deviceId = req.body.deviceId;
	var currentVersion = req.body.currentVersion;
	
	console.log('====================================');
	console.log('appkey' + ' = ' + appkey);
	console.log('sign' + ' = ' + sign);
	console.log('deviceId' + ' = ' + deviceId);
	console.log('currentVersion' + ' = ' + currentVersion);
	console.log('====================================');

	async.series([
		// 1. Check signature with appkey and sign
		function(callback)
		{
			// Return value is always true because of i can't know signature algorithm.
			var ret = true;

			console.log("1. check signature with appkey and sign");
			if(appkey == undefined || sign == undefined) {
				callback(ErrorCode.SIGNATURE_IS_INVALID);
			} else {
				if(ret == false) {
					console.log("- Fail : Invalid signature!\n");
					callback(ErrorCode.SIGNATURE_IS_INVALID);
				} else {
					console.log("- Success!!\n");
					callback(null, ErrorCode.NO_ERROR);
				}
			}
		},
		// 2. Check form data : currentVersion and deviceId
		function(callback)
		{
			console.log("2. Check form data : currentVersion and deviceId");
			if(currentVersion == undefined) {
				console.log("- Fail : currentVersion is NULL!! \n");
				callback(ErrorCode.CURRENTVERSION_IS_NULL);
			} else if(deviceId == undefined) {
				console.log("- Fail : deviceId is NULL!! \n");
				callback(ErrorCode.DEVICEID_IS_NULL);
			} else {
				console.log("- Success!!\n");
				callback(null, ErrorCode.NO_ERROR);
			}
		},
		// 3. Check deviceId validation
		function(callback)
		{
			console.log("3. Check deviceId validation");
			// Return value is always true because of i can't know device pool.
			var ret = true;

			if(ret == false) {
				console.log("- Fail : deviceId is not exist!! \n");
				callback(ErrorCode.PDSN_IS_NOT_EXIST);
			} else {
				console.log("- Success!!\n");
				callback(null, ErrorCode.NO_ERROR);
			}
		},
		// 4. Check Whether Latest version or not
		function(callback)
		{
			console.log("4. Check Whether Latest version or not");
			otainfo.find().sort({'versionCode':-1}).exec(function(err, otainfos){
				if(err) {
					console.log("- Fail : Error!!\n");
					console.error(err);
					callback(ErrorCode.VERSION_IS_NOT_FOUND);
				} else {
					if(otainfos.length == 0){
						console.log("- Fail : There is no informations in the DB. Add some ota info to DB first.\n");
						callback(ErrorCode.VERSION_IS_NOT_FOUND);
					} else {
						var latest_version = otainfos[0].versionCode;
						g_latest_ota_info = otainfos[0];
						
						console.log('- Latest version : ' + latest_version + ' Current version : ' + currentVersion);

						if(currentVersion == latest_version) {
							console.log("- Fail : Your device is already latest. \n");
							callback(ErrorCode.VERSION_IS_LATEST);
						} else {
							console.log("- It Needs OTA!! \n");
							callback(null, ErrorCode.NO_ERROR);
						}
					}
				}

			});
		},
		// 5. Check need to upgrade
		function(callback)
		{
			console.log("5. Check need to upgrade");
			otainfo.find({'versionCode':currentVersion}, function(err, otainfos){
				if(err) {
					console.log("- Fail : Error!!\n");
					console.error(log);
					callback(ErrorCode.VERSION_IS_NOT_FOUND);
				} else {

					if(otainfos.length == 0) {
						console.log("- Fail : Can't find your version at DB.\n");
						callback(ErrorCode.VERSION_IS_NOT_FOUND);
					} else {
						console.log("- Success!! \n");
						callback(null, ErrorCode.NO_ERROR);
					}
				}
			});
		}],
		// 6. Send Result
		function(err, results) {
			if(err) {
				console.log('ERROR CODE : ' + err);
				if(err == ErrorCode.VERSION_IS_LATEST) {
					resData.updateModel = 0;
					resData.status = "SUCCEED";
				} else {
					resData = getErrorResult(err);
				}
			} else {
				console.log("Send OTA Info !!");
				resData = getOTAInformation();
			}

			// Send result.
			console.log(resData);
			res.send(resData);
		}
	);

};

function reportVersion_proc(req, res) {
	var resData = {};
	console.log(req.path);
	console.log(req.body);

	resData.errorMessage = "Parameter is invalid";
	resData.extMessage = " deviceId is null ";
	resData.status = "FAILED";
	resData.errorCode = "Update-Center.0001";
	res.send(resData);
};

// ##############################################
// Local Method
// ##############################################
function getErrorResult(errType)
{
	var err_result = {};
	switch(errType)
	{
		case ErrorCode.SIGNATURE_IS_INVALID:
			err_result.errorMessage = "The signature is invalid";
			err_result.errorCode = "APP.0006";
			break;

		case ErrorCode.CURRENTVERSION_IS_NULL:
			err_result.errorMessage = "Parameter is invalid";
			err_result.extMessage = " currentVersion is null ";
			err_result.errorCode = "Update-Center.0001";
			break;

		case ErrorCode.DEVICEID_IS_NULL:
			err_result.errorMessage = "Parameter is invalid";
			err_result.extMessage = " deviceId is null ";
			err_result.errorCode = "Update-Center.0001";
			break;

		case ErrorCode.PDSN_IS_NOT_EXIST:
			err_result.errorMessage = "pdsn is not exist !";
			err_result.errorCode = "Update-Center.0018";
			break;

		case ErrorCode.VERSION_IS_NOT_FOUND:
			err_result.errorMessage = "Version is not found";
			err_result.extMessage = "currentVersion is not exist";
			err_result.errorCode = "Update-Center.0002";
			break;
		
		case ErrorCode.SERVICE_NOT_FOUND:
			err_result.errorMessage = "Service Not Found";
			err_result.errorCode = "SYS.0003";
			break;
			
		default:
			break;
	}
	err_result.status = "FAILED";

	return err_result;
};

function getOTAInformation()
{
	var ota_info = {};
	var objOtaInfo= {};
	var patchList = [];

	ota_info.updateModel = 1;

	objOtaInfo.md5 					= g_latest_ota_info.md5
	objOtaInfo.appName 				= g_latest_ota_info.appName;
	objOtaInfo.versionCode 			= g_latest_ota_info.versionCode;
	objOtaInfo.fileSize 			= g_latest_ota_info.fileSize;
	objOtaInfo.versionId 			= g_latest_ota_info.versionId;
	objOtaInfo.fileUrl				= g_latest_ota_info.fileUrl; 
	objOtaInfo.versionDetail 		= g_latest_ota_info.versionDetail;
	objOtaInfo.dependencyVersionCode= g_latest_ota_info.dependencyVersionCode;

	ota_info.fullFile = objOtaInfo;
	ota_info.patchList = patchList;
	ota_info.status = "SUCCEED";

	return ota_info;
}

function getFilesizeInBytes(filename) {
	const stats = fs.statSync(filename);
	const fileSizeInBytes = stats.size;
	return fileSizeInBytes;
};

function getIPAddress() {
	var interfaces = require('os').networkInterfaces();
	for (var devName in interfaces) {
		var iface = interfaces[devName];
		for (var i = 0; i < iface.length; i++) {
			var alias = iface[i];
			if (alias.family === 'IPv4' && alias.address !== '127.0.0.1' && !alias.internal)
				return alias.address;
		}
	}
	return '0.0.0.0';
};

function getUserIP(req){
    var ipAddress;

    if(!!req.hasOwnProperty('sessionID')){
        ipAddress = req.headers['x-forwarded-for'];
    } else{
        if(!ipAddress){
            var forwardedIpsStr = req.header('x-forwarded-for');

            if(forwardedIpsStr){
                var forwardedIps = forwardedIpsStr.split(',');
                ipAddress = forwardedIps[0];
            }
            if(!ipAddress){
                ipAddress = req.connection.remoteAddress;
            }
        }
    }

	if(ipAddress && ipAddress.startsWith("::ffff:")) {
		ipAddress = ipAddress.replace("::ffff:", "");
	}
	
    return ipAddress;
}
