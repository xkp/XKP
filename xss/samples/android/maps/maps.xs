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
	if(!photos.is_loaded)
	{
		lblStatus.caption = "Loading resources...";
		photos.load();
	}
	
	swtScene.active = "imgGallery";
}

on btnMaps.click()
{
	if(!maps.is_loaded)
	{
		lblStatus.caption = "Loading resources...";
		maps.load();
	}
	
	swtScene.active = "mapController";
}

on photos.loaded()
{
	lblStatus.caption = "Photos was loaded completely...";
	
	imgGallery.click();
}

on maps.loaded()
{
	lblStatus.caption = "Maps was loaded completely...";
	mapController.map_src = "holguin";
}
