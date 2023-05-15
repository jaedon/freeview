var init = 0; 
var socket;

var g_ota_info;

function startSocketIO()
{
    if(init === 0 )
    {    
        socket = io.connect();
		window.io_socket = socket;

        socket.on('toclient', function(data){
            console.log(data);
			socket.emit('get_ota_info', data);
		
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
        socket.on('otaInfo', otainfo_proc);

        init = 1; 
    }    
}

console.log("Test");

var otainfo_proc = function(data) {
	var item = document.getElementById('listForm');
	var info_html = "";
	g_ota_info = data;
	console.log('GET OTA INFO - Length( %d )', data.length);
	console.log(data);
	var td_style = '<td width=300 style="table-layout:fixed; word-break:break-all;" bgcolor="#aaaaaa" align=center>\n';

	info_html = '<table cellpadding="0" cellspacing="0">\n';
	info_html += '<tr>\n';
	info_html += '<td width=400 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>appName</strong>\n';
	info_html += '</td>\n';
	info_html += '<td width=150 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>versionId</strong>\n';
	info_html += '</td>\n';
	info_html += '<td width=150 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>versionDetail</strong>\n';
	info_html += '</td>\n';
	info_html += '<td width=150 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>versionCode</strong>\n';
	info_html += '</td>\n';
	info_html += '<td width=150 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>dependencyVersionCode</strong>\n';
	info_html += '</td>\n';
	info_html += '<td width=500 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>fileUrl</strong>\n';
	info_html += '</td>\n';
	info_html += '<td width=150 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>fileSize</strong>\n';
	info_html += '</td>\n';
	info_html += '<td width=400 bgcolor="#aaaaaa" align=center>\n';
	info_html += '<strong>md5</strong>\n';
	info_html += '</td>\n';
	info_html += '</tr>\n';
	for(i = 0 ; i < data.length ; ++i)
	{
		info_html += '<tr id="ota_tr'+i+'" ondblclick="javascript:showEditPopup(event, g_ota_info['+i+'])">\n';

		info_html += '<td width=400 align=center>\n';
		info_html += data[i].appName + '\n';
		info_html += '</td>\n';

		info_html += '<td width=150 align=center>\n';
		info_html += data[i].versionId + '\n';
		info_html += '</td>\n';

		info_html += '<td width=150 align=center>\n';
		info_html += data[i].versionDetail + '\n';
		info_html += '</td>\n';

		info_html += '<td width=150 align=center>\n';
		info_html += data[i].versionCode + '\n';
		info_html += '</td>\n';

		info_html += '<td width=150 align=center>\n';
		info_html += data[i].dependencyVersionCode + '\n';
		info_html += '</td>\n';

		info_html += '<td width=500 align=center>\n';
		info_html += '<a href="' + data[i].fileUrl + '" target="_blank">\n';
		info_html += data[i].fileUrl + '\n';
		info_html += '</a>\n';
		info_html += '</td>\n';

		info_html += '<td width=150 align=center>\n';
		info_html += data[i].fileSize + '\n';
		info_html += '</td>\n';

		info_html += '<td width=400 align=center>\n';
		info_html += data[i].md5 + '\n';
		info_html += '</td>\n';

		info_html += '</tr>\n';
	}
	info_html += '</table>\n';
	item.innerHTML = info_html;
};

function showEditPopup(event, ota_info){
	window.name = "OTA_main";
	window.current_ota_info = ota_info;
	var child = window.open("EditPopup.html", "ota_edit", "width=700, height=350, left=100, top=50");
};

var emit_event = function (sock_event, data)
{
	console.log(sock_event);
	console.log(data);

	socket.emit(sock_event, data);

};
window.emit_event = emit_event;
