var oapi = new OAPI("oapitest");

var	itemCollection = [];

var loading;

var Item = function(group, title, input, type, desc){
	var a = this;
	a.group = group.replace(/\s/,'_');
	a.title = title.replace(/\s/,'_');
	a.input = input;
	a.type = type;
	a.desc = desc;
}

Item.prototype.addItem = function(){
	var a = this;
	
	var jqGroup = $('#'+a.group); 
	
	if(jqGroup.length == 0){ // 해당그룹이 없을 때
		$('#items').append('<div class="divItem" id="'+ a.group + '" data-role="collapsible" data-collapsed="true" data-theme="e" data-content-theme="d">' +
				'<h3>' +a.group + '</h3>' +
				'<a class="btnItem '+ a.group +' ui-btn ui-btn-up-b ui-btn-corner-all ui-shadow" id="'+ a.group +'_'+ a.title +'" data-role="button" data-theme="b" href="#">' +
				'<span class="ui-btn-inner ui-btn-corner-all"><span class="ui-btn-text">'+a.title+'</span><span class="ui-icon ui-icon-plus ui-icon-shadow"></span></a></div>');
		 $('div[data-role="collapsible"]').collapsible();
	}
	else{ // 해당 그룹이 있을 때
		jqGroup.find('a:last').after(
				'<a class="btnItem '+a.group+' ui-btn ui-btn-up-b ui-btn-corner-all ui-shadow" id="' + a.group +'_'+ a.title + '" data-role="button" data-theme="b" href="#">'
				+'<span class="ui-btn-inner ui-btn-corner-all"><span class="ui-btn-text">'+a.title+'</span><span class="ui-icon ui-icon-plus ui-icon-shadow"></span></a>');
	}
}

function loadJSON(){
	$.getJSON('json/FILE_LIST.json', function(data){
		for(var i=0; i<data.files.length; i++){
			var url = 'json/';
			url += data.files[i];
			url += '.json';
			loadOAPI(url);
		}
	})
	.success(function(){console.log('FileList.json load success');})
	.error(function(){console.log('FileList.json load Error');});
}

function loadOAPI(url){
	$.getJSON(url, function(data){
		for(var i=0; i<data.items.length; i++){
			var index = data.items[i];
			var item = new Item(data.group, index.title, index.input, index.type, index.desc);
			item.addItem();
			itemCollection.push(item);
		}
	})
	.success(function(){
		$('.btnItem').bind('click', function() {
			var id = $(this).attr('id');
			for(var i=0; i<itemCollection.length; i++){
				if(id == itemCollection[i].group+'_'+itemCollection[i].title){
					$('#selected_item a span span:first').html(itemCollection[i].title+'<span class="ui-collapsible-heading-status"> click to collapse contents</span>');
					$('#descript_item').text(itemCollection[i].desc);
					$('#text_input').val(js_beautify(JSON.stringify(itemCollection[i].input)));
					if(itemCollection[i].type == 'call'){
						//todo call
						$('.output').hide();
						$('#btn_query').hide();
						$('#btn_call')
						.show()
						.bind('click', function(){
							oapi.call(JSON.parse($("#text_input").val()));
						});
					}else if(itemCollection[i].type == 'query'){
						//todo query
						$('.output').show();
						$('#text_output').val('');
						$('#btn_call').hide();
						$('#btn_query')
						.show()
						.bind('click',function(){
							loading.show();
							oapi.query(JSON.parse($('#text_input').val()), 
									function(message){ 
										var res = js_beautify(JSON.stringify(message)); 
										$('#text_output').val(res);
										loading.hide(); 
									},function(){ alert('Error'); });
						});
					}
					break;
				}
			}
		});
	})
	.error(function(){
		alert('ERROR : get JSON '+ url);
	});	
}

$(document).ready(function(){	
	loading = $('#loading');
	loading.hide();
	
	$('#btn_open').bind('click', function(){
		loading.show();
		var url=$('#text_url').val();
		oapi.open(url, function(){
			loadJSON();
			loading.hide();
			$.mobile.changePage('#main', 'slide');
			$('#btn_call').hide();
			$('#btn_query').hide();
			$('.output').hide();
			$('.accesscode').hide();
			
			$("#flip-b").bind('change', function(){
				var access = $('#flip-b option:selected').val();
				if(access == 'yes'){
					$('.accesscode').show();
				}else if(access == 'no'){
					$('.accesscode').hide();
					$('.accesscode_register').remove();
				}
			});
			/*access code register not ready
			$('#btn_register').bind('click', function(){
				var accesscode = $('#text_accesscode').val();
				$('#text_accesscode').val('');
				$('#grid3')
				.append('<lable class="accesscode_register" style="display:block">'+accesscode+'</label><textarea class="accesscode accesscode_register input ui-input-text ui-body-c ui-corner-all ui-shadow-inset" style="width:400px; height:100px;" title="AccessCode"></textarea>');
			});
			*/
		}, function(){
			loading.hide();
			alert('Fail');
		});
	});
	
});