property key_press = false;

on girl1.click()
{
	girl1.animation = "g1_down";
	girl1.lock = true;	
	girl1.animation = "g1_jump";
	girl1.lock = true;	
	girl1.animation = "g1_idle";
}

on girl2.click()
{
	girl2.animation = "g2_cry";
	girl2.bounce = true;
	girl2.lock = true;	
	girl2.animation = "g2_idle";
}

on keydown(keycode)
{
	if(keycode == LEFT_ARROW)
	{
		monster.animation = "m_running";	
		key_press = true;
	}
}

on keyup(keycode)
{
	if(keycode == LEFT_ARROW)
	{
		monster.animation = "m_idle";	
		key_press = false;
	}		
}

on updates()
{		
	if(key_press)		
		monster.x -= 0.2;		
}


