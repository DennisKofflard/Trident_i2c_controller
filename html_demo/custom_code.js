//$.post('ajax/test.html') .done(function(data) { /* process */ }) .always(function() { setTimeout(doPoll, 5000); });



function doPoll(){
	//$.post('http://qa3mmc3.ele.tue.nl/all/dump')
	$.get('http://qa3mmc3.ele.tue.nl/all/dump')
		.done(function(data) {
			alert(data);  // process results here
		})
		.always(function() {
			setTimeout(doPoll,5000);
		})
}


doPoll();
