property left_key_press = false;
property right_key_press = false;

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
		monster.animation = "m_run_l";
		mario.animation = "mario_run_l";
		left_key_press = true;
	}
	if(keycode == RIGHT_ARROW)
	{
		monster.animation = "m_run_d";
		mario.animation = "mario_run_r";
		right_key_press = true;
	}
}

on keyup(keycode)
{
	if(keycode == LEFT_ARROW)
	{
		monster.animation = "m_idle";
		mario.animation = "m_stop_l";
		mario.lock = true;
		mario.animation = "m_idle_l";
		left_key_press = false;
	}	
	if(keycode == RIGHT_ARROW)
	{
		monster.animation = "m_idle";
		mario.animation = "m_stop_r";
		mario.lock = true;
		mario.animation = "m_idle_r";
		right_key_press = false;
	}	
}

on update()
{		
	if(left_key_press)
	{
		monster.x -= 0.2;	
		mario.x -= 0.2;	
	}
	if(right_key_press)
	{		
		monster.x += 0.2;
		mario.x += 0.2;	
	}
}


