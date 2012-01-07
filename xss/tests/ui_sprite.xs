property clicks = 1;
property key_press = false;

on btn1.click()   
{	
	lbl1.caption = "Got " + clicks + " smileys";
	clicks++;	
}

on girl.click()
{
	girl.animate("g_down");	
}

on keydown(keycode)
{
	if(keycode == LEFT_ARROW)
		key_press = true;		
}

on keyup(keycode)
{
	if(keycode == LEFT_ARROW)
		key_press = false;		
}

on updates()
{	
	girl.animate("g_idle");	
	if(key_press){
		monster.animate("m_running");
		monster.x -= 0.2;
	}
	else
		monster.animate("m_idle");
}


