//$.post('ajax/test.html') .done(function(data) { /* process */ }) .always(function() { setTimeout(doPoll, 5000); });



function doPoll(){
	//$.get('http://qa3mmc3.ele.tue.nl/all/dump')
	host=$("#host").val();
	$.get(host)
		.done(function(data) {
			process(data);  // process results here
		})
		.fail(function() {
			$("#dyndata").html("<h4 class='col-md-12'>could not get data from \"" + host + "\"</h2>");
		})
		.always(function() {
			setTimeout(doPoll,1000);
		})
}

function bool_to_color(booly) {
	if(booly) {
		return "success";
	} else {
		return "danger";
	}
}

function process(data){
	//console.log(data);
	
	if(data) {
		//blindly assuming this is the right data. Only acceptable for a demo.
	
		htxt = "";
	

		//debug	
		//htxt += ("debug = " + data.debug);


		//voltages
		if(data.voltages) {
			htxt += "<div class='col-xs-6 col-sm-3 col-md-3 col-lg-2'>";
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
					htxt += "<div class='col-xs-12 col-sm-9 col-md-8 col-lg-6'>";
					htxt += "<h3>Transceiver " + index + ": </h3>";

					htxt += "<table class='table table-bordered table-condensed table-striped'>";
						htxt += "<tr><td>VDD33_TX </td><td>" + value.VDD33_TX + "V</td>";
							htxt += "<td colspan='2'>Vendor information:</td></tr>";
						htxt += "<tr><td>VDD33_RX </td><td>" + value.VDD33_RX + "V</td>";
							htxt += "<td>name</td><td>"     + value.vendor_info.name     + "</td></tr>";
						htxt += "<tr><td>temp_tx1 </td><td>" + value.temp_TX1 + "C</td>";
							htxt += "<td>OUI</td><td>"      + value.vendor_info.OUI      + "</td></tr>";
						htxt += "<tr><td>temp_tx2 </td><td>" + value.temp_TX2 + "C</td>";
							htxt += "<td>part</td><td>"     + value.vendor_info.part     + "</td></tr>";
						htxt += "<tr><td>temp_rx1 </td><td>" + value.temp_RX1 + "C</td>";
							htxt += "<td>revision</td><td>" + value.vendor_info.revision + "</td></tr>";
						htxt += "<tr><td>temp_rx2 </td><td>" + value.temp_RX2 + "C</td>";
							htxt += "<td>serial</td><td>"   + value.vendor_info.serial   + "</td></tr>";
					htxt += "</table>";

					
					htxt += "<h4>channels:</h4>";
					htxt += "<table class='table table-bordered table-condensed table-striped'>";
						tmp_ch="<tr><td>channel number</td>";
						tmp_te="<tr><td>tx enable channel</td>";
						tmp_to="<tr><td>tx enable output</td>";
						tmp_re="<tr><td>rx enable channel</td>";
						tmp_ro="<tr><td>rx enable output</td>";
						tmp_lo="<tr><td>loss of signal (green=ok)</td>";

						$.each(value.channels, function(index_n, value_n) {
							tmp_ch+="<td style='min-width:30px'>"        +               value_n.channel            +   "</td>";
							tmp_te+="<td style='min-width:30px' class='" + bool_to_color(value_n.TX_enable_channel) + "'></td>";
							tmp_to+="<td style='min-width:30px' class='" + bool_to_color(value_n.TX_enable_output)  + "'></td>";
							tmp_re+="<td style='min-width:30px' class='" + bool_to_color(value_n.RX_enable_channel) + "'></td>";
							tmp_ro+="<td style='min-width:30px' class='" + bool_to_color(value_n.RX_enable_output)  + "'></td>";
							tmp_lo+="<td style='min-width:30px' class='" + bool_to_color(!(value_n.RX_LOS))         + "'></td>";
						});						
						
						tmp_ch+="</tr>";
						tmp_te+="</tr>";
						tmp_to+="</tr>";
						tmp_re+="</tr>";
						tmp_ro+="</tr>";
						tmp_lo+="</tr>";
						htxt += tmp_ch + tmp_te + tmp_to + tmp_re + tmp_ro + tmp_lo;

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
