//$.post('ajax/test.html') .done(function(data) { /* process */ }) .always(function() { setTimeout(doPoll, 5000); });



function doPoll(){
	//$.post('http://qa3mmc3.ele.tue.nl/all/dump')
	$.get('http://qa3mmc3.ele.tue.nl/all/dump')
		.done(function(data) {
			process(data);  // process results here
		})
		.always(function() {
			//setTimeout(doPoll,5000);
		})
}

function process(data){
	console.log(data);
	
	if(data) {
		//blindly assuming this is the right data. Only acceptable for a demo.
	
		htxt = "";
	

		//debug	
		//htxt += ("debug = " + data.debug);


		//voltages
		if(data.voltages) {
			htxt += "<div class='col-md-3'>";
				htxt += "<h3>Board voltages:</h3>";
				htxt += "<table class='table table-bordered table-condensed table-striped'>";
					$.each(data.voltages, function(index, value) {
						htxt += "<tr>";
							htxt += "<td>" + index + "</td>";
							htxt += "<td>" + value + "</td>";
						htxt += "</tr>";
					});
				htxt += "</table>";
			htxt += "</div>";
		}


		//tranceivers
		if(data.transceivers) {
			htxt += "<div id='transceivers'>";

			$.each(data.transceivers, function(index, value) {
				if(value.connected && value.ready) {
					htxt += "<div class='col-md-6'>";
					htxt += "<h3>Transceiver " + index + ": </h3>";

					htxt += "<table class='table table-bordered table-condensed table-striped'>";
						htxt += "<tr><td>VDD33_TX </td><td>" + value.VDD33_TX + "</td></tr>";
						htxt += "<tr><td>VDD33_RX </td><td>" + value.VDD33_RX + "</td></tr>";
						htxt += "<tr><td>temp_tx1 </td><td>" + value.temp_TX1 + "</td></tr>";
						htxt += "<tr><td>temp_tx2 </td><td>" + value.temp_TX2 + "</td></tr>";
						htxt += "<tr><td>temp_rx1 </td><td>" + value.temp_RX1 + "</td></tr>";
						htxt += "<tr><td>temp_rx2 </td><td>" + value.temp_RX2 + "</td></tr>";
					htxt += "</table>";

					
					htxt += "<table class='table table-bordered table-condensed table-striped'>";
						$.each(value.channels, function(index_n, value_n) {
							console.log(index_n);
							console.log(value_n);

						});						
					htxt += "</table>";

					htxt += "</div>";
				}
			});
			
			htxt += "</div>";
		}



		$("#dyndata").html(htxt);
	}
}


doPoll();
