
var init = 0;
var monitorStarted = 0;
var socket;

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
		socket.on('monitorinfo', function(data){

			console.log(data);

			if(monitorStarted === 0)
			{
				var titleArray = ['Date', 'Free Mem'];
				memfree_data[0] = titleArray;
				monitorStarted = 1;
			}	
			
			if(data.cpuRate)
			{
				gauge_data.setValue(0, 1, data.cpuRate);
			}
			if(data.memRate)
			{
				gauge_data.setValue(1, 1, data.memRate);
			}
			drawGaugeChart();

			if(data.memFreeSize)
			{ 
				var memfree_current = [];
				memfree_current[0] = getCurrentTime();
				memfree_current[1] = data.memFreeSize;
				memfree_data.push(memfree_current);
			}
			drawMemFreeChart();


			if(data.procInfo)
			{
				// UsedMemRate sort and draw chart
				data.procInfo.sort(function(a,b){
					return b.usedCpuRate - a.usedCpuRate;
				});
				var sumOfCpuRate = 0;
				cpu_donut_data.splice(0, cpu_donut_data.length);
				cpu_donut_data.push(['Process', 'UsedCpuRate']);
				for(i = 0 ; i < data.procInfo.length ; ++i)
				{
					sumOfCpuRate += data.procInfo[i].usedCpuRate;
					cpu_donut_data.push([data.procInfo[i].processName, data.procInfo[i].usedCpuRate]);
				}
				cpu_donut_data.push(['Other or Idle', 100-sumOfCpuRate]);
				drawCpuDonutChart();


				// UsedMemRate sort and draw chart
				data.procInfo.sort(function(a,b){
					return b.usedMemRate - a.usedMemRate;
				});
				var sumOfMemRate = 0;
				mem_donut_data.splice(0, mem_donut_data.length);
				mem_donut_data.push(['Process', 'UsedMemRate']);
				for(i = 0 ; i < data.procInfo.length ; ++i)
				{
					sumOfMemRate += data.procInfo[i].usedMemRate;
					mem_donut_data.push([data.procInfo[i].processName, data.procInfo[i].usedMemRate]);
				}
				mem_donut_data.push(['Other or Free', 100-sumOfMemRate]);
				drawMemDonutChart();
			}
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


function startMonitorButtonClicked()
{	
	if(monitorStarted === 0)
	{
		var msg = {};

		var interval = document.getElementById('interval');
		if(interval.value && interval.value > 3)
		{
			msg.interval = interval.value;
		}
		else
		{
			// default 3 seconds
			msg.interval = 3;
		}

		memfree_data.splice(0, memfree_data.length);

		console.log('start monitor! - interval( ' + msg.interval + ' )');

		// start monitor
		socket.emit('monitor_start', msg);
	}

}

function stopMonitorButtonClicked()
{	
	var msg = {};

	console.log('stop monitor!');

	socket.emit('monitor_stop', msg);
	
	monitorStarted = 0;
}
