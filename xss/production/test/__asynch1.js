importScripts("resig.js");
importScripts("pixastic.core.js");
importScripts("actions.js");
importScripts("pixastic-manager.js");
importScripts("pixastic-utils.js");
self.onmessage = function(message) {
var mypreset = 
new pixastic.Preset();
mypreset.deserialize(message.data.mypreset);
var image = message.data.image;
mypreset.apply(image);
var __wwmsg1 = {image_data : image,};
postMessage(__wwmsg1);
}
