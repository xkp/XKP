property currentImage : int = 0;

//TODO: implement into android java an efficient method to 
//      iterate through all xss resource 
property resources = ["img1", "img2", "img3", "img4", "img5", "img6", "img7", "img8"];

on changer.click()
{
	currentImage++;
	currentImage = currentImage % resources.size;
	string currentResource = resources[currentImage];
	album.src = currentResource;
	identifier.caption = currentResource;
}
