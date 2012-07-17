importScripts("../js/resig.js");
importScripts("../js/jsmanipulate/jsmanipulate.js");
importScripts("../js/jsmanipulate/jsm-manager.js");
importScripts("../js/jsmanipulate/jsm-utils.js");
self.onmessage = function(message) {
var mypreset = 
new jsmanip.Preset();
mypreset.deserialize(message.data.mypreset);
var image = message.data.image;
mypreset.apply(image);
var __wwmsg1 = {id: '__wwmsg1',result : image,};
postMessage(__wwmsg1);
}
