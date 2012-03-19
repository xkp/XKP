property currentImage : int = -1;
property resources = [
	"worth_1", 
	"worth_2", 
	"worth_3", 
	"worth_4", 
	"worth_5", 
	"worth_6", 
	"worth_7"
];

on imgGallery.click()
{
	application.currentImage++;
	application.currentImage = application.currentImage % application.resources.size;
	string currentResource = application.resources[application.currentImage];
	imgGallery.src = currentResource;
}

on btnGallery.click()
{
	if(photos.is_loaded)
		photos.load();
	
	lblStatus.caption = "Loading resources...";
	swtScene.active = "imgGallery";
}

on photos.loaded()
{
	lblStatus.caption = "Resources was loaded completely...";
	
	imgGallery.click();
}
