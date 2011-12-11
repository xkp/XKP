function importjs(dir, file) {
	var bodyID = document.getElementsByTagName("body")[0];
	var newScript = document.createElement('script');
	newScript.type = 'text/javascript';
	newScript.src = dir + file + ".js";

	bodyID.appendChild(newScript);
}

String.prototype.namespace = function(separator) {
  this.split(separator || '.').inject(window, function(parent, child) {
    return parent[child] == undefined ? parent[child] = { } : parent[child];
  });
}

String.prototype.unique_id = function() {
	return this.concat("_" + new Date().getTime());
}

String.prototype.space_by_under = function () {
	return this.replace(/\s/g, "_");
}

function random_color(format) {
	var rint = Math.round(0xffffff * Math.random());
	switch (format) {
	case 'hex':
		return ('#0' + rint.toString(16)).replace(/^#0([0-9a-f]{6})$/i, '#$1');
		break;

	case 'rgb':
		return 'rgb(' + (rint >> 16) + ',' + (rint >> 8 & 255) + ',' + (rint & 255) + ')';
		break;

	default:
		return rint;
		break;
	}
}

XMLHttpRequest.prototype.sendCanvas = function(the_url, params) {
    var BOUNDARY = "---------------------------1966284435497298061834782736";
    var rn = "\r\n";

    var req = "--" + BOUNDARY;

    for (var i in params) {
      req += rn + "Content-Disposition: form-data; name=\"" + i + "\"";
      if (typeof params[i] != "string") {
        var canvas = params[i];
        var url = canvas.toDataURL("image/png");
        var base64 = url.substr(url.indexOf(",") + 1);
        var bin = window.atob(base64);
        req += "; filename=\"image.png\"" + rn + "Content-type: image/png";

        req += rn + rn + bin + rn + "--" + BOUNDARY;
      } else {
        req += rn + rn + params[i] + rn + "--" + BOUNDARY;
      }
    }
    req += "--";

    this.open("POST", the_url, true);

    this.setRequestHeader("Content-Type", "multipart/form-data; boundary=" + BOUNDARY);
    this.setRequestHeader("X-FLIXSOR", "ping");

    this.sendAsBinary(req);
  };