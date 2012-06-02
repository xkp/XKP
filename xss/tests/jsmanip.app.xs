on update(){
	if(host.needs_update)
	{
		img1.contrast2.amount = host.contrast_amount;
		if(image0)	//The question is necessary because if the resource is not loaded yet, image0 will be 'undefined'
		{
			image0.rgb1.red = host.rgb_red;
			image0.rgb1.green = host.rgb_green;
			image0.rgb1.blue = host.rgb_blue;
		}
		plane1.emboss1.height = host.emboss_height;
		plane1.emboss1.angle = host.emboss_angle;
		plane1.emboss1.elevation = host.emboss_elevation;
		material1.noise1.amount = host.anoise_amount;
		material1.noise1.density = host.anoise_density;
		material1.noise1.mono = host.anoise_mono;
		host.needs_update = false;
	}
}