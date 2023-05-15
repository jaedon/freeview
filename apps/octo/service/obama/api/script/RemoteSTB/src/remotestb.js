var oapi = new OAPI('HelloOAPI');

function loadServiceList(list) {
	// function comapreLCN(a, b) {
		// return a.lcn - b.lcn;
	// }
	// function compareName(a, b) {
		// return a.name.localeCompare(b.name);
	// }
	// if($("input[type='radio']:first").attr("checked")) {
	// list.service.sort(comapreLCN);
	// } else {
	// list.service.sort(compareName);
	// }

	var data = '';
	for(var i = 0; i < list.service.length; i++) {
		var service = list.service[i];
		data += '<li>';
		data += '<a href="#" class="serviceItem" id="';
		data += 'HDVB://tripleid.' + service.svcid + "." + service.tsid + "." + service.onid;
		//data += list.service[i].svc_uid;
		data += '">';
		//data += '<strong>' + list.service[i].lcn + '</strong>';
		data += service.name;
		data += '</a>';
		data += '<a href="#" class="serviceDetail" id="prog:';
		data += service.svc_uid;
		data += '">';
		data += 'Programmes </a>';
		data += '</li>';
	}
	$("#servicelist").html(data);
	$("#servicelist").listview("refresh");
	
	$(".serviceItem").bind("click", function(event, ui) {
		bindChannel(this.id);
	});

	$(".serviceDetail").bind("click", function(event, ui) {
		alert(this.id);
	});
}

function bindChannel(url) {
	console.log("bindChannel: " + url);

	oapi.call({
		"mediactrl" : {
			"target" : "live",
			"operation" : "stop",
			"property" : {
				"destination" : "av0"
			}
		}
	});
	oapi.call({
		"mediactrl" : {
			"target" : "live",
			"operation" : "start",
			"property" : {
				"destination" : "av0",
				"url" : url
			}
		}
	});
}


$(document).ready(function() {
	$("#submit").bind("click", function(event, ui) {
		var url = $("#url").val();

		oapi.open(url, openSuccess, openFail);
		function openSuccess() {
			console.log("openSuccess");

			oapi.query({
				"meta" : {
					"target" : "svclist",
					"operation" : "get",
					"property" : {
						"searched" : false
					}
				}
			}, function(message) {
				if(message.meta.target == "svclist") {
					$.mobile.changePage("#ServiceList", "slide");

					loadServiceList(message.meta.property.svclist);
				}//console.log(message);

			}, function() {
				console.log("cannot get service list");
			});
		}

		function openFail() {
			console.log("openFail");
			alert("oapi.open(" + url + ") failed!");
		}

	});

	$("#sync").bind("click", function(event, ui) {
		alert("Sync");
	});
});
//
// //$(document).bind("mobileinit", function() {
// $(document).ready(function() {
// console.log('$(document).ready');
//
// $("#Home").bind("pagebeforecreate", function(e) {
//
// console.log('$("#Home").live("pagebeforecreate"');
// $("#submit").bind("click", function(event, ui) {
// var url = $("#url").val();
//
// oapi.open(url, openSuccess, openFail);
// function openSuccess() {
// console.log("openSuccess");
// $.mobile.changePage("#ServiceListPage", "slide");
// }
//
// function openFail() {
// console.log("openFail");
// alert("oapi.open(" + url + ") failed!");
// }
// });
// });
//
// $("#ServiceList").bind("pagebeforecreate", function(e) {
// console.log('$("#ServiceList").live("pagebeforecreate"');
// $("#sync").bind("click", function(event, ui) {
// alert("Sync");
// });
//
// $(".serviceItem").bind("click", function(event, ui) {
// alert(this.id);
// });
//
// $(".serviceDetail").bind("click", function(event, ui) {
// alert(this.id);
// });
// });
//
// });