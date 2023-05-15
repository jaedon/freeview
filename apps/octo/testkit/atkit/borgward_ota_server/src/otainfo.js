// src/fillFile.js

var mongoose = require('mongoose');
var Schema = mongoose.Schema;

var otainfo_schema= new Schema({
	// It will show to user.
	appName					: String,
	versionId				: Number,
	versionDetail			: String,
	versionCode				: String,
	fileSize				: Number,
	md5						: String,
	fileUrl					: String,
	dependencyVersionCode	: String,

	// It will be used internally.
	filePath				: String
});

module.exports = mongoose.model('otainfo', otainfo_schema);
