var util_labelPropText = function(cmpId, new_value) {
	var strId = "#__l" + cmpId.attr("id");
	if(new_value === undefined) {
		return $(strId).html();
	} else {
		$(strId).html(new_value);
	}
}

var util_checkboxPropLeft = function(cmpId, new_value) {
	var strId = "#__l" + cmpId.attr("id");
	if(new_value) {
		cmpId.css( { left: new_value } );
		$(strId).css( { left: new_value + 30 } );
	}
}

var util_checkboxPropTop = function(cmpId, new_value) {
	var strId = "#__l" + cmpId.attr("id");
	if(new_value) {
		cmpId.css( { top: new_value } );
		$(strId).css( { top: new_value - 2 } );
	}
}
