on click()
{
	img1.imagedata = preset1.apply(img1.imagedata);	
}
on init()
{	
	
}
on keydown(keycode)
{
	if(keycode == NUMPAD_PLUS)
	{
		// b_filter.amount += 1;
		img2.imagedata = b_filter.apply(img2.imagedata);
		// img3.bf0.amount += 1;
	}
	if(keycode == NUMPAD_MINUS)
	{
		// b_filter.amount -= 1;
		img2.imagedata = b_filter.apply(img2.imagedata);	
		// img3.bf0.amount -= 1;
	}
}