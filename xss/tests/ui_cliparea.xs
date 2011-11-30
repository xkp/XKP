property clicks = 0;

on btn1.click()   
{	
	lbl1.caption = "Got " + clicks + " smileys";
	btn1.rotation += 10;
	clicks++;	
}

on keydown(keycode)
{
	if(ca1.isVisible()){
		if(keycode == LEFT_ARROW)
			ca1.x -= 2;
		
		if(keycode == RIGHT_ARROW)
			ca1.x += 2;
		
		if(keycode == UP_ARROW)
			ca1.y -= 2;
		
		if(keycode == DOWN_ARROW)
			ca1.y += 2;
	}
	if(ca2.isVisible()){
		if(keycode == A)
			ca2.x -= 2;
		
		if(keycode == D)
			ca2.x += 2;
		
		if(keycode == W)
			ca2.y -= 2;
		
		if(keycode == S)
			ca2.y += 2;
	}
}


