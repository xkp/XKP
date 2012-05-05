importScripts("../js/resig.js");
importScripts("../js/pixastic/pixastic.core.js");
importScripts("../js/pixastic/actions.js");
importScripts("../js/pixastic/pixastic-manager.js");
importScripts("../js/pixastic/pixastic-utils.js");
self.onmessage = function(message) {
var mypreset = 
new pixastic.Preset();
mypreset.deserialize(message.data.mypreset);
var image = message.data.image;
mypreset.apply(image);
var __wwmsg1 = {image_data : image,};
postMessage(__wwmsg1);
}
