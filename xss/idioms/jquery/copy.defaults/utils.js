var util_propText = function(cmpId) {
	var strId = "#__l" + cmpId.attr("id");
	return $(strId).html();
}

var util_propText = function(cmpId, new_text) {
	var strId = "#__l" + cmpId.attr("id");
	$(strId).html(new_text);
}

