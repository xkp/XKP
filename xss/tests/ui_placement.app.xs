property clicks = 0;
property flag = true;

on btn1.click()   
{	
	lbl1.caption = "Got " + clicks + " smileys";
	clicks++;		
}

on btnRight.click()
{
	btn1.placement = "right";
	flag = true;
}

on btnLeft.click()
{
	btn1.placement = "left";
	flag = true;
}

on btnUp.click()
{
	btn1.placement = "top";
	flag = true;
}

on btnDown.click()
{
	btn1.placement = "bottom";
	flag = true;
}

on keydown(keycode)
{	
	if(keycode == LEFT_ARROW)
	{
		if(flag)
			test1.x -= 2;
		else
			btn1.x -= 2;
	}
	if(keycode == RIGHT_ARROW)
	{
		if(flag)
			test1.x += 2;
		else
			btn1.x += 2;
	}		
	if(keycode == UP_ARROW)
	{
		if(flag)
			test1.y -= 2;
		else
			btn1.y -= 2;
	}		
	if(keycode == DOWN_ARROW)
	{
		if(flag)
			test1.y += 2;
		else
			btn1.y += 2;
	}
	if(keycode == NUMPAD_PLUS)
	{
		test1.h += 2;
		test1.w += 2;
	}	
	if(keycode == NUMPAD_MINUS)
	{
		test1.h -= 2;
		test1.w -= 2;
	}
	if(keycode == CTRL)
		test1.hide();
	if(keycode == SHIFT)
		test1.show();
	if(keycode == SPACE)
	{
		btn1.placement = "none";
		flag = false;
	}
}


