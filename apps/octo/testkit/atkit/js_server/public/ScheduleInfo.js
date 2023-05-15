
var init = 0;
var socket;

var schedules;
var recordings;

function startSocketIO()
{
	if(init === 0 )
	{
		socket = io.connect();

		socket.on('toclient', function(data){
			console.log(data);
		});

		socket.on('stdout', function(data){
			console.log("STDOUT!");
			console.log(data);
			console.log(JSON.stringify(data));
		});
		socket.on('stderr', function(data){
			console.log("STDERR!");
			console.log(JSON.stringify(data));
		});
		socket.on('error', function(data){
			console.log("ERROR!");
			console.log(JSON.stringify(data));
		});
		socket.on('exception', function(data){
			console.log("EXCEPTION!");
			console.log(JSON.stringify(data));
			//alert(JSON.stringify(data));
		});
		socket.on('scheduleInfo', function(data){
			schedules = data.schedules;
			console.log('Number of Schedules( %d )', schedules.length);
			console.log(data);


			for(i=0 ; i < schedules.length ; ++i) {
				var array = [];
				console.log(schedules[i]);
				array.push(String(schedules[i].svc_name));
				array.push(schedules[i].event_name);
				array.push(new Date(schedules[i].start_time * 1000));
				array.push(new Date(schedules[i].end_time * 1000));


				schedule_data.push(array);
			}


			drawScheduleTimeline();

		});

		socket.on('recordingInfo', function(data){
			recordings = data.recordings;
			console.log('Number of Recordings( %d )', recordings.length);
			console.log(data);

			for(i=0 ; i < recordings.length ; ++i) {
				var array = [];
				console.log(recordings[i]);
				array.push(String(recordings[i].svc_name));
				array.push(recordings[i].content_name);
				array.push(new Date(recordings[i].start_time * 1000));
				array.push(new Date(recordings[i].end_time * 1000));


				recording_data.push(array);
			}


			drawRecordingTimeline();

		});

		init = 1;
	}
}

function getCurrentTime()
{
	var dt = new Date();
	var month = dt.getMonth()+1;
	var d = dt.getFullYear() + '-' + month + '-' + dt.getDate() + ' ' + dt.getHours() + ':' + dt.getMinutes() + ':' + dt.getSeconds();
	return d;
}


function scheduleInfoButtonClicked()
{	
	var msg = {};

	// initilize to schedule data
	schedule_data.splice(0, schedule_data.length);

	// get_schedules
	socket.emit('get_schedules', msg);

}

function recordingInfoButtonClicked()
{	
	var msg = {};

	// initilize to recording data
	schedule_data.splice(0, recording_data.length);

	// get_recordings
	socket.emit('get_recordings', msg);
}

function schItemSelected()
{
	var select = document.getElementById('sch_name_select');
	var name = select.options[select.selectedIndex].text;

	console.log("SCH itemSelected!!!!");

	schedule_data.splice(0, schedule_data.length);

	for(i=0 ; i < schedules.length ; ++i) {
		var array = [];
		console.log(schedules[i]);
		array.push(String(schedules[i].svc_name));
		if(name == "event_name"){
			array.push(schedules[i].event_name);
		} else if(name == "event_id"){
			array.push(String(schedules[i].event_id));
		} else if(name == "type"){
			array.push(String(schedules[i].type));
		} else if(name == "seriesId"){
			array.push(String(schedules[i].seriesId));
		} else if(name == "programId"){
			array.push(String(schedules[i].programId));
		} else if(name == "seriesCRID"){
			array.push(schedules[i].seriesCRID);
		} else if(name == "programCRID"){
			array.push(schedules[i].programCRID);
		} else {
			array.push(schedules[i].event_name);
		}
		array.push(new Date(schedules[i].start_time * 1000));
		array.push(new Date(schedules[i].end_time * 1000));


		schedule_data.push(array);
	}


	drawScheduleTimeline();
}

function recItemSelected()
{
	var select = document.getElementById('rec_name_select');
	var name = select.options[select.selectedIndex].text;

	console.log("REC itemSelected!!!!");

	recording_data.splice(0, recording_data.length);

	for(i=0 ; i < recordings.length ; ++i) {
		var array = [];
		console.log(recordings[i]);
		array.push(String(recordings[i].svc_name));
		if(name == "content_name"){
			array.push(recordings[i].content_name);
		} else if(name == "content_id"){
			array.push(String(recordings[i].content_id));
		} else if(name == "content_url"){
			array.push(recordings[i].content_url);
		} else if(name == "rec_status"){
			switch(recordings[i].rec_status)
			{
				case 0:
					array.push("START");
					break;
				case 1:
					array.push("RECORDING");
					break;
				case 2:
					array.push("COMPLETED");
					break;
				case 3:
				default :
					array.push("INCOMPLETED");
					break;
			}
		} else if(name == "rec_endreason"){
			array.push(String(recordings[i].rec_endreason));
		} else if(name == "video_type"){
			switch(recordings[i].video_type)
			{
				case 1:
					array.push("SD");
					break;
				case 2:
					array.push("HD");
					break;
				default :
					array.push("UNKNOWN");
					break;
			}
		} else {
			array.push(recordings[i].content_name);
		}
		array.push(new Date(recordings[i].start_time * 1000));
		array.push(new Date(recordings[i].end_time * 1000));


		recording_data.push(array);
	}


	drawRecordingTimeline();
}

function sch_outputUpdate(vol) {
	document.querySelector('#sch_width').value = 'width : ' + vol;

	options_schedule.width = vol;
	console.log("REC TimeLine - Resize width( %d )\n", options_schedule.width);
	drawScheduleTimeline();
}

function rec_outputUpdate(vol) {
	document.querySelector('#rec_width').value = 'width : ' + vol;

	options_recording.width = vol;
	console.log("REC TimeLine - Resize width( %d )\n", options_recording.width);
	drawRecordingTimeline();
}
