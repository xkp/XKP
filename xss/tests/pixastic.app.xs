on mono_check1.click()
{
	img1.noise_filter.mono = mono_check.is_active;
}

on bsize_b2.click()
{
	bsize_bar.value += 2;
	img1.mosaic_filter.block_size += 1;
}

on bsize_b1.click()
{
	bsize_bar.value -= 2;
	img1.mosaic_filter.block_size -= 1;
}

on amount_b2.click()
{
	amount_bar.value += 2;
	img1.noise_filter.amount += 0.02;
}

on amount_b1.click()
{
	amount_bar.value -= 2;
	img1.noise_filter.amount -= 0.02;
}

on amount_b21.click()
{
	amount_bar1.value += 2;
	img1.glow_filter.amount += 0.02;
}

on amount_b11.click()
{
	amount_bar1.value -= 2;
	img1.glow_filter.amount -= 0.02;
}

on amount_b22.click()
{
	amount_bar2.value += 2;
	img1.sharpen_filter.amount += 0.02;
}

on amount_b12.click()
{
	amount_bar2.value -= 2;
	img1.sharpen_filter.amount -= 0.02;
}

on radius_b2.click()
{
	radius_bar.value += 2;
	img1.glow_filter.radius += 0.02;
}

on radius_b1.click()
{
	radius_bar.value -= 2;
	img1.glow_filter.radius -= 0.02;
}

on strength_b2.click()
{
	strength_bar.value += 2;
	img1.noise_filter.strength += 0.02;
}

on strength_b1.click()
{
	strength_bar.value -= 2;
	img1.noise_filter.strength -= 0.02;
}

on density_b2.click()
{
	density_bar.value += 2;
	img1.pointillize_filter.density += 0.1;
}

on density_b1.click()
{
	density_bar.value -= 2;
	img1.pointillize_filter.density -= 0.1;
}

on noise_b2.click()
{
	noise_bar.value += 2;
	img1.pointillize_filter.noise += 0.1;
}

on noise_b1.click()
{
	noise_bar.value -= 2;
	img1.pointillize_filter.noise -= 0.1;
}

on transparent_check.click()
{
	img1.pointillize_filter.transparent = transparent_check.is_active;
}

on mono_check.click()
{
	img1.edges_filter.mono = mono_check.is_active;
}

on invert_check.click()
{	
	img1.edges_filter.invert = invert_check.is_active;
}

on red_b2.click()
{
	red_bar.value += 2;
	img1.coloradjust_filter.red += 0.02;
}

on red_b1.click()
{
	red_bar.value -= 2;
	img1.coloradjust_filter.red -= 0.02;
}

on green_b2.click()
{
	green_bar.value += 2;
	img1.coloradjust_filter.green += 0.02;
}

on green_b1.click()
{
	green_bar.value -= 2;
	img1.coloradjust_filter.green -= 0.02;
}

on blue_b2.click()
{
	blue_bar.value += 2;
	img1.coloradjust_filter.blue += 0.02;
}

on blue_b1.click()
{
	blue_bar.value -= 2;
	img1.coloradjust_filter.blue -= 0.02;
}

on brightness_b2.click()
{
	brightness_bar.value += 2;
	img1.brightness_filter.brightness += 3;
}

on brightness_b1.click()
{
	brightness_bar.value -= 2;
	img1.brightness_filter.brightness -= 3;
}

on contrast_b2.click()
{
	contrast_bar.value += 2;
	img1.brightness_filter.contrast += 0.02;
}

on contrast_b1.click()
{
	contrast_bar.value -= 2;
	img1.brightness_filter.contrast -= 0.02;
}

on saturation_b2.click()
{
	saturation_bar.value += 2;
	img1.hsl_filter.saturation += 4;
}

on saturation_b1.click()
{
	saturation_bar.value -= 2;
	img1.hsl_filter.saturation -= 4;
}

on hue_b2.click()
{
	hue_bar.value += 2;
	img1.hsl_filter.hue += 7.2;
}

on hue_b1.click()
{
	hue_bar.value -= 2;
	img1.hsl_filter.hue -= 7.2;
}

on lightness_b2.click()
{
	lightness_bar.value += 2;
	img1.hsl_filter.lightness += 4;
}

on lightness_b1.click()
{
	lightness_bar.value -= 2;
	img1.hsl_filter.lightness -= 4;
}

on btnNext.click()
{	
	if (options.active < options.count - 1)
        options.active++;	
}

on btnPrev.click()
{	
    if(options.active > 0)
        options.active--;	
}

on flipv_label.drag(x, y)
{
	flipv_label.x = x - flipv_label.width/2;
	flipv_label.y = y - flipv_label.height/2;	
}
on flipv_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.flipv_filter.apply = !img1.flipv_filter.apply;
	}
	flipv_label.x = 610;
	flipv_label.y = 110;	
}

on hsl_label.drag(x, y)
{
	hsl_label.x = x - hsl_label.width/2;
	hsl_label.y = y - hsl_label.height/2;	
}
on hsl_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.hsl_filter.apply = !img1.hsl_filter.apply;
	}
	hsl_label.x = 610;
	hsl_label.y = 130;	
	hsl_options.switchable = img1.hsl_filter.apply;	
}

on edges_label.drag(x, y)
{
	edges_label.x = x - edges_label.width/2;
	edges_label.y = y - edges_label.height/2;	
}
on edges_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.edges_filter.apply = !img1.edges_filter.apply;
	}
	edges_label.x = 610;
	edges_label.y = 150;
	edges_options.switchable = img1.edges_filter.apply;
}

on pointillize_label.drag(x, y)
{
	pointillize_label.x = x - pointillize_label.width/2;
	pointillize_label.y = y - pointillize_label.height/2;	
}
on pointillize_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.pointillize_filter.apply = !img1.pointillize_filter.apply;
	}
	pointillize_label.x = 610;
	pointillize_label.y = 170;	
	pointillize_options.switchable = img1.pointillize_filter.apply;
}

on brightness_label.drag(x, y)
{
	brightness_label.x = x - brightness_label.width/2;
	brightness_label.y = y - brightness_label.height/2;	
}

on brightness_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.brightness_filter.apply = !img1.brightness_filter.apply;
	}
	brightness_label.x = 610;
	brightness_label.y = 190;	
	brightness_options.switchable = img1.brightness_filter.apply;
}

on invert_label.drag(x, y)
{
	invert_label.x = x - invert_label.width/2;
	invert_label.y = y - invert_label.height/2;	
}
on invert_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.invert_filter.apply = !img1.invert_filter.apply;
	}
	invert_label.x = 610;
	invert_label.y = 210;	
}

on sepia_label.drag(x, y)
{
	sepia_label.x = x - sepia_label.width/2;
	sepia_label.y = y - sepia_label.height/2;	
}
on sepia_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.sepia_filter.apply = !img1.sepia_filter.apply;
	}
	sepia_label.x = 610;
	sepia_label.y = 230;	
}

on coloradjust_label.drag(x, y)
{
	coloradjust_label.x = x - coloradjust_label.width/2;
	coloradjust_label.y = y - coloradjust_label.height/2;	
}
on coloradjust_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.coloradjust_filter.apply = !img1.coloradjust_filter.apply;
	}
	coloradjust_label.x = 610;
	coloradjust_label.y = 250;	
	coloradjust_options.switchable = img1.coloradjust_filter.apply;	
}

on noise_label.drag(x, y)
{
	noise_label.x = x - noise_label.width/2;
	noise_label.y = y - noise_label.height/2;	
}

on noise_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.noise_filter.apply = !img1.noise_filter.apply;
	}
	noise_label.x = 610;
	noise_label.y = 270;		
	noise_options.switchable = img1.noise_filter.apply;		
}

on blur_label.drag(x, y)
{
	blur_label.x = x - blur_label.width/2;
	blur_label.y = y - blur_label.height/2;	
}

on blur_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.blur_filter.apply = !img1.blur_filter.apply;
	}
	blur_label.x = 610;
	blur_label.y = 290;			
}

on edges2_label.drag(x, y)
{
	edges2_label.x = x - edges2_label.width/2;
	edges2_label.y = y - edges2_label.height/2;	
}

on edges2_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.edges2_filter.apply = !img1.edges2_filter.apply;
	}
	edges2_label.x = 610;
	edges2_label.y = 310;			
}

on glow_label.drag(x, y)
{
	glow_label.x = x - glow_label.width/2;
	glow_label.y = y - glow_label.height/2;	
}

on glow_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.glow_filter.apply = !img1.glow_filter.apply;
	}
	glow_label.x = 610;
	glow_label.y = 330;		
	glow_options.switchable = img1.glow_filter.apply;	
}

on sharpen_label.drag(x, y)
{
	sharpen_label.x = x - sharpen_label.width/2;
	sharpen_label.y = y - sharpen_label.height/2;	
}

on sharpen_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.sharpen_filter.apply = !img1.sharpen_filter.apply;
	}
	sharpen_label.x = 610;
	sharpen_label.y = 350;		
	sharpen_options.switchable = img1.sharpen_filter.apply;	
}

on solarize_label.drag(x, y)
{
	solarize_label.x = x - solarize_label.width/2;
	solarize_label.y = y - solarize_label.height/2;	
}

on solarize_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.solarize_filter.apply = !img1.solarize_filter.apply;
	}
	solarize_label.x = 610;
	solarize_label.y = 370;		
}

on fliph_label.drag(x, y)
{
	fliph_label.x = x - fliph_label.width/2;
	fliph_label.y = y - fliph_label.height/2;	
}

on fliph_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.fliph_filter.apply = !img1.fliph_filter.apply;
	}
	fliph_label.x = 610;
	fliph_label.y = 390;		
}

on mosaic_label.drag(x, y)
{
	mosaic_label.x = x - mosaic_label.width/2;
	mosaic_label.y = y - mosaic_label.height/2;	
}

on mosaic_label.dragend(x, y)
{
	if(x >= img1.x && x <= (img1.x + img1.width) && y >= img1.y && y <= (img1.y + img1.height))
	{
		img1.mosaic_filter.apply = !img1.mosaic_filter.apply;
	}
	mosaic_label.x = 610;
	mosaic_label.y = 410;		
	mosaic_options.switchable = img1.mosaic_filter.apply;	
}