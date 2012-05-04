self.onmessage = function(message) {
var mypreset = message.data.mypreset;
var image = message.data.image;
mypreset.apply(image);
var __wwmsg1 = {image_data : image,};
postMessage(__wwmsg1);
}
