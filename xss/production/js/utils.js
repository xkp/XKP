function getElementPosition(element) {
	var elem=element, tagname="", x=0, y=0;
	while(elem && (typeof(elem) == "object") && (typeof(elem.tagName) != "undefined")) {
		y += elem.offsetTop;
		x += elem.offsetLeft;
		tagname = elem.tagName.toUpperCase();
		if(tagname == "BODY")
			elem=0;
		if(typeof(elem) == "object") {
			if(typeof(elem.offsetParent) == "object")
				elem = elem.offsetParent;
		}
	}
	return {x: x, y: y};
}
function importjs(dir, file) {
	var bodyID = document.getElementsByTagName("body")[0];
	var newScript = document.createElement('script');
	newScript.type = 'text/javascript';
	newScript.src = dir + file + ".js";

	bodyID.appendChild(newScript);
}

function measureText(pText, pFont) {
    var lDiv = document.createElement('lDiv');

    document.body.appendChild(lDiv);
    
    lDiv.style.font = pFont;
	if(!lDiv.style.font)
		lDiv.style.font = "10px sans-serif"
    lDiv.style.position = "absolute";
    lDiv.style.left = -1000;
    lDiv.style.top = -1000;

    lDiv.innerHTML = pText;

    var lResult = {
        width: lDiv.clientWidth,
        height: lDiv.clientHeight
    };

    document.body.removeChild(lDiv);
    lDiv = null;

    return lResult;
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