importScripts("resig.js");
importScripts("jsmanipulate.js");
importScripts("jsm-manager.js");
importScripts("jsm-utils.js");
self.onmessage = function(message) {
var mypreset = 
new jsmanip.Preset();
mypreset.deserialize(message.data.mypreset);
var image = message.data.image;
mypreset.apply(image);
var __wwmsg1 = {image_data : image,};
postMessage(__wwmsg1);
}
