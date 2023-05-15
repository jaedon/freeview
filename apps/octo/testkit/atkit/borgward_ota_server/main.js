// main.js

var http = require('http');
var express = require('express');
var bodyParser = require('body-parser');

require('./sockio');


// local variable
var HTTP_PORT_NUMBER = 11223;
var app = express();

global.PORT_NUM = HTTP_PORT_NUMBER;
// ##############################
// Set error handler
// ##############################
process.on('uncaughtException', function(err) {
	console.log('###################################################');
	console.log('################## Exception!!!!! #################');
	console.log('###################################################');
	console.error(err);
	console.log('###################################################');
});

// ##############################
// Set express
// ##############################
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(function (req, res, next) {
    res.header('Access-Control-Allow-Origin', '*');
    next();
});
app.use(express.static('public'));

// ##############################
// Set router
// ##############################
require('./routes').setRouter(app);

// ##############################
// HTTP Server Start
// ##############################
var httpServer = http.createServer(app).listen(HTTP_PORT_NUMBER, function () {
	console.log('##############################################################');
	console.log('IP OTA Simulation Server : ' + HTTP_PORT_NUMBER);
	console.log('##############################################################\n\n');
});

startSocketIO(httpServer);


// ##############################
// Mongo DB
// ##############################
var mongoose = require('mongoose');
mongoose.Promise = global.Promise;

var db = mongoose.connection;
var otainfo = require('./src/otainfo');
var db_name = 'borgward';

// Set callback
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function(){

	console.log("Connected to mongod server : DB name( %s )", db_name);
});

// Connect mongo db
mongoose.connect('mongodb://localhost/'+db_name, {useMongoClient: true});

