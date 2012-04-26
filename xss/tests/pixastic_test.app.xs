on click()
{
	img3.bf0.apply = !img3.bf0.apply;
	img3.f1.apply = !img3.f1.apply;
	b_filter.apply = !b_filter.apply;	
	img1.imagedata = b_filter.apply(img1.imagedata);	
}
on init()
{	
	img2.imagedata = preset_1.apply(img2.imagedata);
	plane1.imagedata = preset_1.apply(plane1.imagedata);
	sphere_material.imagedata = preset_1.apply(sphere_material.imagedata);
	img1.imagedata = b_filter.apply(img1.imagedata);
}
on keydown(keycode)
{
	if(keycode == NUMPAD_PLUS)
	{
		b_filter.brightness += 5;
		img1.imagedata = b_filter.apply(img1.imagedata);	
	}
	if(keycode == NUMPAD_MINUS)
	{
		b_filter.brightness -= 5;
		img1.imagedata = b_filter.apply(img1.imagedata);	
	}
}